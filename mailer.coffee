nodemailer  = require 'nodemailer'
config      = require './config'
jade        = require 'jade'
fs          = require 'fs'

newPurchaseTemplate = jade.compile fs.readFileSync "./views/emailTemplates/newPurchase.jade"
confirmationTemplate = jade.compile fs.readFileSync "./views/emailTemplates/confirmation.jade"


# create reusable transport method (opens pool of SMTP connections)
smtpTransport = nodemailer.createTransport("SMTP",
  host: "smtp.webfaction.com"
  port: 465
  secureConnection: true
  auth:
    user: "robot_wileycousins"
    pass: config.emailPW
)

# setup e-mail data with unicode symbols
mailOptions =
  from: "Wiley Cousins <robot@wileycousins.com>"
  to: "le dudes <dev@wileycousins.com>"
  subject: "clockblock"

exports.confirmation = (user, num) ->
  # send them an email
  if num < 10
    num = "0#{num.toString()}"
  mailOptions.html = confirmationTemplate
      user: user
      url: config.url
      num: num
  mailOptions.to = user.email
  mailOptions.subject = 'clockblock purchase confirmation'
  smtpTransport.sendMail mailOptions, (error, response) ->
    if error
      console.log error
    else
      console.log "Message sent: " + response.message

exports.newPurchase = (user, num) ->
  # send us an email
  mailOptions.html = newPurchaseTemplate
      user: user
      url: config.url
  mailOptions.subject = '[clockblock] new purchase'
  if process.env.NODE_ENV == 'production'
    smtpTransport.sendMail mailOptions, (error, response) ->
      if error
        console.log error
      else
        console.log "Message sent: " + response.message
  exports.confirmation user, num

# if you don't want to use this transport object anymore, uncomment following line
#smtpTransport.close(); // shut down the connection pool, no more messages
