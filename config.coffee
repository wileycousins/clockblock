exports.loggerFormat = "dev"
exports.useErrorHandler = true
exports.enableEmailLogin = true
exports.mongodb = process.env.MONGO_DB || "mongodb://localhost/clockblock"
exports.sessionSecret = "super duper bowls"
exports.useLongPolling = process.env.USE_LONG_POLLING || false
exports.emailPW = process.env.EMAIL_PW
exports.url = process.env.SITE_URL || "http://127.0.0.1:3000"
