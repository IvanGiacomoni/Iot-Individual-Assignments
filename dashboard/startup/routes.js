const getData = require("../routes/getData");
const views = require("../routes/views");
const controlActuators = require("../routes/controlActuators");

module.exports = function (app) {
  app.use("/", getData);
  app.use("/", views);
  app.use("/", controlActuators);
};
