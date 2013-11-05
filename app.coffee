###
Module dependencies.
###
config      = require './config'
express     = require 'express'
stylus      = require 'stylus'
nib         = require 'nib'
path        = require 'path'
http        = require 'http'
https       = require 'https'
path        = require 'path'
mongoose    = require 'mongoose'
exec        = require('child_process').exec
fs          = require 'fs'
if process.env.NODE_ENV != 'production'
  privateKey  = fs.readFileSync './ssl/server.key', 'utf8'
  certificate = fs.readFileSync './ssl/server.crt', 'utf8'
else
  privateKey  = fs.readFileSync './ssl/gandi.key', 'utf8'
  certificate = fs.readFileSync './ssl/gandi.crt', 'utf8'

credentials = 
  key: privateKey
  cert: certificate

#passport.use "email", new LocalStrategy(
  #usernameField: "email"
#, (email, password, done) ->
  #process.nextTick ->
    #User.authEmail email, password, done
#)

#passport.serializeUser (user, done) ->
  #done null, user.id

#passport.deserializeUser (id, done) ->
  #User.findById id, (err, user) ->
    #done err, user

# connect the database
mongoose.connect config.mongodb

# create app, server, and web sockets
app = express()
if process.env.NODE_ENV != 'production'
  server = https.createServer(credentials, app)
else
  server = http.createServer(app)

compile = (str, path) ->
  stylus(str).set('filename', path).use(nib())

app.configure ->
  app.set "port", process.env.PORT or 3000
  app.set "views", __dirname + "/views"
  app.set "view engine", "jade"
  
  # use the connect assets middleware for Snockets sugar
  app.use require("connect-assets")()
  app.use express.favicon()
  app.use express.logger(config.loggerFormat)
  app.use express.bodyParser()
  app.use express.methodOverride()
  app.use express.cookieParser(config.sessionSecret)
  app.use express.session(secret: "shhhh")
  #app.use passport.initialize()
  #app.use passport.session()
  app.use app.router
  app.use stylus.middleware
        src: path.join(__dirname,'assets')
        compile: compile
  app.use express.static path.join __dirname, "assets"  
  app.use express.static path.join __dirname, "public"  
  app.use express.errorHandler()  if config.useErrorHandler

require("./urls")(app)

server.listen app.get("port"), ->
  console.log "Express server listening on port " + app.get("port")

