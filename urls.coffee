models    = require './models'
config    = require './config'
stripe    = require('stripe')(config.stripe)
Users     = models.User
Products  = models.Product
mailer    = require './mailer'

module.exports = (app) ->
  # UI routes 
  app.get "/", (req, res) ->
    res.render "index.jade", stripe_js: config.stripe_js

  app.post "/purchase", (req, res) ->
    name = req.body.name
    email = req.body.email
    phone = req.body.phone
    address = req.body.address
    city = req.body.city
    state = req.body.state
    zip = req.body.zip

    stripeToken = req.body.stripeToken
    charge =
      name: name
      amount: 150*100
      currency: 'USD'
      card: stripeToken

    Users.findOne(
      name: name
      email: email
      phone: phone
      address: address
      city: city
      state: state
      zip: zip
    ).exec (err, user) ->
      if err
        console.log err
        return res.send "error finding user in db, sorry"
      if !user
        console.log "new user"
        user = new Users
          name: name
          email: email
          phone: phone
          address: address
          city: city
          state: state
          zip: zip
        user.save()
      stripe.charges.create charge, (err, charge) ->
        if err
          console.log err
          return res.render 'error'
        else
          product = new Products buyer: user
          product.save (err, product) ->
            if err
              console.log err
              return res.send "error creating purchase record in db, sorry"
            user.purchased_products.addToSet product
            user.save (err, user) ->
              if err
                console.log "error saving user post add product: #{err}"
              Products.find( purchase_date: {$lt:(new Date()).toJSON()} ).exec (err, clocks)->
                mailer.newPurchase user, clocks.length
                return res.render 'purchase', num: clocks.length
  
  isAdmin = (req, res, next) -> 
    if process.env.NODE_ENV.match('production') && (!req.session.auth?.match('so-good') && !req.body.password?.match(process.env.ADMIN_PASSWORD))
      return res.render 'login'
    else
      req.session['auth'] = 'so-good'
      next()


  app.all "/login", isAdmin, (req, res) ->
    return res.redirect('/orders')

  app.get "/orders", isAdmin, (req, res) ->
    Users.find().populate('purchased_products').sort("purchased_products.purchase_date").exec (err, users) ->
      if err
        console.log err
        return res.send err
      return res.render "orders.jade", users: users

  app.get "/purchase", isAdmin, (req, res) ->
    Products.find().exec (err, clocks) ->
      if err
        console.log err
      return res.render 'purchase', num: clocks.length

  app.get "/confirmation", isAdmin, (req, res) ->
    Users.findOne().exec (err, user) ->
      if err
        console.log err
      Products.find().exec (err, clocks) ->
        if err
          console.log err
        return res.render 'emailTemplates/confirmation', user: user, num: clocks.length, url: 'http://127.0.0.1:3000'

  app.get "/confirmation/:user", isAdmin, (req, res) ->
    res.redirect '/orders'

  app.post "/confirmation/:user", isAdmin, (req, res) ->
    Users.findById(req.params.user).exec (err, user) ->
      if err
        console.log err
      console.log req.body.clock
      Products.find({purchase_date:{$lte:req.body.clock}}).exec (err, clocks) ->
        if err
          console.log err
        num = clocks.length
        if num < 10
          num = "0#{num.toString()}"
        mailer.confirmation user, num
        return res.render 'emailTemplates/confirmation', user: user, num:num, url: config.url

  app.post "/update/:user", isAdmin, (req, res) ->
    console.log "updating"
    Users.findById(req.params.user).populate("purchased_products").exec (err, user) ->
      if err || !user
        console.log err
        return res.send err
      user.name = req.body.name
      user.phone = req.body.phone
      user.email = req.body.email
      user.address = req.body.address
      user.city = req.body.city
      user.state = req.body.state
      console.log "updated the fields"
      console.log user
      user.save()
    res.redirect '/orders'
