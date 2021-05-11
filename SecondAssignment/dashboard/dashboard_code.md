# Dahsboard code

## Libraries used

- aws-sdk
- aws-iot-device-sdk
- axios
- dotenv
- express
- body-parser
- nodemon

## Code organization

The dashboard code has been organized in several directories.

### root directory

- **index.js**: it contains the code needed to launch the server
- **package.json**: it contains all dependencies for the server

### config

- **default.json**: it contains all configuration parameters

### helper

- **sortData.js**: it contains the quicksort algorithm code

### routes

- **views.js**: it contains all routes needed to manage the views on the frontend side
- **getData.js**: it contains all routes needed to manage the temperature and gas/smoke data retrieved from the cloud on AWS
- **controlActuators.js**: it contains all routes needed to manage the actuators on the frontend side

### startup

- **routes.js**: it contains the code needed to use the routes available on the server

### views

- **dashboard.html**: it contains the code for the dashboard view
