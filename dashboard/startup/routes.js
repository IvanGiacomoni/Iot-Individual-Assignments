const getData = require("../routes/getData");
const views = require("../routes/views");
const controlActuators = require("../routes/controlActuators");

var bodyParser = require('body-parser')


module.exports = function (app) {
  app.use(bodyParser.json());
  app.use(bodyParser.urlencoded({ extended: false }))
  
  app.use("/", getData);
  app.use("/", views);
  app.use("/", controlActuators);
};
