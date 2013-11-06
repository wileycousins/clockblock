models    = require './models'
config    = require './config'
stripe    = require('stripe')(config.stripe)
Users     = models.User
Products  = models.Product
mailer    = require './mailer'

module.exports = (app) ->
  # UI routes
  app.get "/", (req, res) ->
    res.render "index.jade"
  

  if process.env.NODE_ENV != 'production'
    app.get "/purchase", (req, res) ->
      return res.render 'purchase'
  
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
      emai: email
      name: name
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
            user.save()
            mailer.newPurchase user
            return res.render 'purchase'
  
