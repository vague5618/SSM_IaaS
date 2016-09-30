  var path = require('path');
  var favicon = require('serve-favicon');
  var logger = require('morgan');
  var cookieParser = require('cookie-parser');
  var bodyParser = require('body-parser');
  var routes = require('./routes/index');
  var users = require('./routes/users');
  var select = require('./routes/select');
  var express = require('express');
  var app = express();
  var session = require('express-session');
  var redisStore = require('connect-redis')(session);
  var redisClient = require('redis').createClient();

// view engine setup
  app.set('views', path.join(__dirname, 'views'));
  app.set('view engine', 'ejs');
  app.engine('html', require('ejs').renderFile);

  app.use(logger('dev'));

  app.use(session({
    secret: 'kkkk2014',
    store: new redisStore({ host: '211.189.19.30', port: 6700, client: redisClient}),
    saveUninitialized: false,
    resave: false
  }));

  app.use(bodyParser.json());
  app.use(bodyParser.urlencoded({extended: false}));
  app.use(cookieParser());
  app.use(require('stylus').middleware(path.join(__dirname, 'public')));
  app.use(express.static(path.join(__dirname, 'public')));
  app.use("/public/stylesheets/style.css", express.static(__dirname + "/public/stylesheets/style.css"));
  app.use("/common/common.process.js", express.static(__dirname + "/common/common.process.js"));
  app.use("/public/lib/jsmpg.js", express.static(__dirname + "/public/lib/jsmpg.js"));
  app.use("/public/lib/binary.js", express.static(__dirname + "/public/lib/binary.js"));
  app.use("/public/js/audio_stream.js", express.static(__dirname + "/public/js/audio_stream.js"));
  app.use("/public/js/image_stream.js", express.static(__dirname + "/public/js/image_stream.js"));
  app.use("/public/js/desktopEvent_stream.js", express.static(__dirname + "/public/js/desktopEvent_stream.js"));
  app.use("/public/js/mobileEvent_stream.js", express.static(__dirname + "/public/js/mobileEvent_stream.js"));
  app.use("/public/js/connect_manager.js", express.static(__dirname + "/public/js/connect_manager.js"));
  app.use("/routes/select.js", express.static(__dirname + "/routes/select.js"));
  app.use("/util/redis.js", express.static(__dirname + "/util/redis.js"));


  app.use('/', routes);
  app.use('/select', select);
  app.use('/users', users);


// catch 404 and forward to error handler
  app.use(function (req, res, next) {
    var err = new Error('Not Found');
    err.status = 404;
    next(err);
  });

// error handlers

// development error handler
// will print stacktrace
  if (app.get('env') === 'development') {
    app.use(function (err, req, res, next) {
      res.status(err.status || 500);
      res.render('error', {
        message: err.message,
        error: err
      });
    });
  }

// production error handler
// no stacktraces leaked to user

  app.use(function (err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
      message: err.message,
      error: {}
    });
  });

  module.exports = app;
