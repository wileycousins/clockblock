models    = require './models'
stripe    = require('stripe')('sk_test_n06Ogoe7k2fAfGwEsLohPmZV')
Users     = models.User
Products  = models.Product

module.exports = (app) ->
  # UI routes
  app.get "/", (req, res) ->
    res.render "index.jade"
  
  
  app.post "/purchase", (req, res) ->
    stripeToken = req.body.stripeToken
    console.log stripeToken
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
          res.send "error... bummer man"
        else
          product = new Products()
          product.save (err, product) ->
            if err
              console.log err
              return res.send "error creating purchase record in db, sorry"
            user.purchased_products.addToSet product
            user.save()
            res.json charge
            console.log 'cha-ching!'
  
