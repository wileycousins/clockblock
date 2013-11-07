config   = require './config'
mongoose = require 'mongoose'
Schema = mongoose.Schema

Product = new Schema
  name:
    type: String
    default: "clockblock"
  price:
    type: Number
    default: 150
  image:
    type: String
    default: "http://clockblocknola.com/images/clockblock.jpg"
  purchase_date:
    type: Date
    default: (new Date()).toJSON()
  buyer:
    type: Schema.ObjectId
    ref: 'User'

User = new Schema
  email:
    type      : String
    required  : true
  name: String
  phone: String
  address: String
  city: String
  state: String
  zip: String
  purchased_products: [
    type: Schema.ObjectId
    ref: 'Product'
  ]

exports.User = mongoose.model 'User', User
exports.Product = mongoose.model 'Product', Product
