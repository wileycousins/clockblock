config   = require './config'
mongoose = require 'mongoose'
Schema = mongoose.Schema
Email = Schema.Types.Email

Product = new Schema
  name: String
  price: Number
  image: String

User = new Schema
  email:
    type      : Email
    required  : true
    unique    : true
  first_name: String
  last_name: String
  purchased_products: [Product]

exports.User = mongoose.model 'User', User
