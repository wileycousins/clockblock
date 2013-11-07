nodemailer  = require 'nodemailer'
config      = require './config'
jade        = require 'jade'
fs          = require 'fs'

newPurchaseTemplate = jade.compile fs.readFileSync "./views/emailTemplates/newPurchase.jade"
confirmationTemplate = jade.compile fs.readFileSync "./views/emailTemplates/confirmation.jade"
errorTemplate = jade.compile fs.readFileSync "./views/emailTemplates/error.jade"


# create reusable transport method (opens pool of SMTP connections)
smtpTransport = nodemailer.createTransport "SMTP",
  host: "smtp.webfaction.com"
  port: 465
  secureConnection: true
  auth:
    user: "robot_wileycousins"
    pass: config.emailPW

# setup e-mail data with unicode symbols
mailOptions =
  from: "Wiley Cousins <robot@wileycousins.com>"
  to: "le dudes <dev@wileycousins.com>"
  subject: "clockblock"

# send them an email
exports.confirmation = (user, num) ->
  if num < 10
    num = "0#{num.toString()}"
  mailOptions.to = user.email
  mailOptions.subject = 'clockblock purchase confirmation'
  mailOptions.html = confirmationTemplate
      user: user
      url: config.url
      num: num
  smtpTransport.sendMail mailOptions, (error, res) ->
    if error
      sendEmailError user, num, error, res
      console.log error
    else
      console.log "Message sent: " + res.message

# send us an email
exports.newPurchase = (user, num) ->
  mailOptions.subject = '[clockblock] new purchase'
  to: "le dudes <dev@wileycousins.com>"
  mailOptions.html = newPurchaseTemplate
      user: user
      url: config.url
  if process.env.NODE_ENV == 'production'
    smtpTransport.sendMail mailOptions, (error, res) ->
      if error
        console.log error
      else
        console.log "Message sent: " + res.message
  exports.confirmation user, num

# send us an email if we got an error emailing them
exports.sendEmailError = (user, num , error, res) ->
  to: "HALP <dev@wileycousins.com>"
  mailOptions.html = errorTemplate
      user: user
      url: config.url
      num: num
      error: error
      res: res
  mailOptions.subject = '[clockblock] [error] send mail error'
  smtpTransport.sendMail mailOptions, (error, res) ->
    if error
      console.log error
    else
      console.log "Message sent: " + res.message
