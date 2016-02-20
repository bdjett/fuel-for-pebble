function currentLocation(success) {
  navigator.geolocation.getCurrentPosition(success);
}

module.exports.currentLocation = currentLocation;
