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
    stripeToken = req.body.stripeToken
    charge =
      amount: 150*100
      currency: 'USD'
      card: stripeToken

    name = req.body.name
    email = req.body.email
    phone = req.body.phone
    address = req.body.address
    city = req.body.city
    state = req.body.state
    zip = req.body.zip
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

  app.all "*", isAdmin

  app.all "/login", (req, res) ->
      return res.redirect('/orders')

  app.get "/orders", (req, res) ->
    Users.find().populate('purchased_products').exec (err, users) ->
      if err
        console.log err
        return res.send err
      res.render "orders.jade", users: users

  app.get "/purchase", (req, res) ->
    Products.find().exec (err, clocks) ->
      if err
        console.log err
      return res.render 'purchase', num: clocks.length

  app.get "/confirmation", (req, res) ->
    Users.findOne().exec (err, user) ->
      if err
        console.log err
      Products.find().exec (err, clocks) ->
        if err
          console.log err
        return res.render 'emailTemplates/confirmation', user: user, num: clocks.length, url: 'http://127.0.0.1:3000'

  app.get "/confirmation/:user", (req, res) ->
    res.redirect '/orders'

  app.post "/confirmation/:user", (req, res) ->
    Users.findById(req.params.user).exec (err, user) ->
      if err
        console.log err
      console.log req.body.clock
      Products.find({purchase_date:{$lte:req.body.clock}}).exec (err, clocks) ->
        if err
          console.log err
        mailer.confirmation user, clocks.length
        return res.render 'emailTemplates/confirmation', user: user, num: clocks.length, url: 'http://127.0.0.1:3000'
