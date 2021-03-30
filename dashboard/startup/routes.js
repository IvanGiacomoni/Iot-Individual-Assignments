const getData = require("../routes/getData");
const views = require("../routes/views");

module.exports = function (app) {
  app.use("/", getData);
  app.use("/", views);
};
