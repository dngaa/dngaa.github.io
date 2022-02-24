var config = {}

config.port = process.env.PORT;
config.redis_port = process.env.HEROKU_REDIS_PORT
config.redis_host = process.env.HEROKU_REDIS_HOST
config.redis_password = process.env.HEROKU_REDIS_PASSWORD

config.isActive = function() {
  return process.env.HEROKU !== undefined;
}

module.exports = config;
