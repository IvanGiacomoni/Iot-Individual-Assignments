const express = require("express");
const app = express();

require("./startup/routes")(app);

const port = 8080;

const server = app.listen(port, () => {
  console.log("Server listening on port : ", port);
});

module.exports = server;
