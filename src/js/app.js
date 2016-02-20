/****************************************************************
 * GasBuddy for Pebble
 * Created by: Brian Jett (bdjett@me.com)
 *
 * NOT ASSOCIATED WITH GASBUDDY
 ****************************************************************/

var maxAppMessageTries = 3;
var appMessageRetryTimeout = 3000;
var appMessageTimeout = 0;
var httpTimeout = 10000;
var appMessageQueue = [];

/////////////////////////////////////////////////////////////////
// APPMESSAGE HANDLING
/////////////////////////////////////////////////////////////////

// sendAppMessage
// Sends the AppMessage queue to the Pebble
var sendAppMessage = function() {
    if (appMessageQueue.length > 0) {
        var currentAppMessage = appMessageQueue[0];
        currentAppMessage.numTries = currentAppMessage.numTries || 0;
        currentAppMessage.transactionId = currentAppMessage.transactionId || -1;

        if (currentAppMessage.numTries < maxAppMessageTries) {
            Pebble.sendAppMessage(
                currentAppMessage.message,
                function(e) {
                    appMessageQueue.shift();
                    setTimeout(function() {
                        sendAppMessage();
                    }, appMessageTimeout);
                }, function(e) {
                    console.log("Faled sending AppMessage for transactionId: " + e.data.transactionId + ". Error: " + e.data.error.message);
                    appMessageQueue[0].transactionId = e.data.transactionId;
                    appMessageQueue[0].numTries++;
                    setTimeout(function() {
                        sendAppMessage();
                    }, appMessageRetryTimeout);
                }
            );
        } else {
            console.log("Faled sending AppMessage after multiple attemps for transactionId: " + currentAppMessage.transactionId + ". Error: None. Here's the message: " + JSON.stringify(currentAppMessage.message));
        }
    }
};

/////////////////////////////////////////////////////////////////
// DATA HANDLING
/////////////////////////////////////////////////////////////////

var getCurrentLocation = function(success) {
  navigator.geolocation.getCurrentPosition(success);
};

var compareStation = function(a, b) {
  if (a.price === 0) {
    if (b.price === 0) {
      return 0;
    } else if (b.price !== 0) {
      return 1;
    }
  } else if (b.price === 0) {
    if (a.price === 0) {
      return 0;
    } else if (a.price !== 0) {
      return -1;
    }
  } else {
    return a.price - b.price;
  }
};

// getNearbyStations
// Given a latitude and longitude, requests the nearby stations and gas prices
var getCheapestStations = function(position) {
  console.log("Getting cheapest stations");
  var sendData = {
    'AppSource': 1,
    'DateDevice': '2014-12-05 13:38:48',
    'Key': 'd4e18bb76d79dbecea40e90f341c47343564884740020ea005293b9e38374905',
    'AppVersion': 2.46,
    'Source': 2,
    'Viewport': {
      'Density': 2,
      'Width': 750,
      'Height': 1334
    },
    'WebServiceVersion': 1,
    'Parameters': {
      'IsPreload': 0,
      'SearchType': 1,
      'Coordinate': {
        'Latitude': position.coords.latitude,
        'Longitude': position.coords.longitude
      }
    },
    'MemberId': '',
    'Beta': false,
    'UserLocation': {
      'Latitude': position.coords.latitude,
      'Accuracy': 10,
      'Longitude': position.coords.longitude
    },
    'AuthId': 'NRyr9kaBFvkKqlzw1PtHjA==',
    'DateEastern': '2014-12-05 13:38:48',
    'Debug': false
  };
  var req = new XMLHttpRequest();
  var requestUrl = "https://xml.gasbuddy.com/gbmaws/v2/Get_Stations_Near_Me.ashx?output=json";
  req.open('POST', requestUrl, true);
  req.timeout = httpTimeout;
  req.onload = function(e) {
    if (req.readyState == 4) {
      if (req.status == 200) {
        if (req.responseText) {
          var response = JSON.parse(req.responseText);
          var stations = response.Payload.StationCollection;
          var sts = [];
          stations.forEach(function(element, index, array) {
            var station = {};
            station.address = element.Address;
            var fuel = null;
            element.Fuels.forEach(function(element, index, array) {
              if (element.Id.toString() == localStorage.grade) {
                fuel = element;
              }
            });
            if (fuel === null) {
              station.price = 0;
            } else {
              station.price = fuel.Prices[0].Amount;
            }
            station.name = element.Name;
            sts.push(station);
          });
          sts = sts.sort(compareStation);
          sts = sts.slice(0, 20);
          sts.forEach(function(station, index, array) {
            if (station.price === 0) {
              station.price = '-';
            }
            appMessageQueue.push({'message': {
              'index': index,
              'name': station.name,
              'price': station.price.toString(),
              'address': station.address
            }});
          });
          sendAppMessage();
        }
      }
    }
  };
  req.ontimeout = function() {
    console.log("Timed out");
  };
  req.onerror = function() {
    console.log("Connection failed");
  };
  req.send(JSON.stringify(sendData));
};

// getNearbyStations
// Given a latitude and longitude, requests the nearby stations and gas prices
var getNearbyStations = function(position) {
  console.log("Getting nearby stations");
  var sendData = {
    'AppSource': 1,
    'DateDevice': '2014-12-05 13:38:48',
    'Key': 'd4e18bb76d79dbecea40e90f341c47343564884740020ea005293b9e38374905',
    'AppVersion': 2.46,
    'Source': 2,
    'Viewport': {
      'Density': 2,
      'Width': 750,
      'Height': 1334
    },
    'WebServiceVersion': 1,
    'Parameters': {
      'IsPreload': 0,
      'SearchType': 1,
      'Coordinate': {
        'Latitude': position.coords.latitude,
        'Longitude': position.coords.longitude
      }
    },
    'MemberId': '',
    'Beta': false,
    'UserLocation': {
      'Latitude': position.coords.latitude,
      'Accuracy': 10,
      'Longitude': position.coords.longitude
    },
    'AuthId': 'NRyr9kaBFvkKqlzw1PtHjA==',
    'DateEastern': '2014-12-05 13:38:48',
    'Debug': false
  };
  var req = new XMLHttpRequest();
  var requestUrl = "https://xml.gasbuddy.com/gbmaws/v2/Get_Stations_Near_Me.ashx?output=json";
  req.open('POST', requestUrl, true);
  req.timeout = httpTimeout;
  req.onload = function(e) {
    if (req.readyState == 4) {
      if (req.status == 200) {
        if (req.responseText) {
          var response = JSON.parse(req.responseText);
          var stations = response.Payload.StationCollection;
          stations = stations.slice(0, 20);
          stations.forEach(function(element, index, array) {
            console.log(JSON.stringify(element));
            var station = {};
            station.address = element.Address;
            var fuel = null;
            element.Fuels.forEach(function(element, index, array) {
              if (element.Id.toString() == localStorage.grade) {
                fuel = element;
              }
            });
            if (fuel === null) {
              station.price = '-';
            } else {
              station.price = fuel.Prices[0].Amount;
            }
            station.name = element.Name;
            if (station.price === 0) {
              station.price = '-';
            }
            appMessageQueue.push({'message': {
              'index': index,
              'name': station.name,
              'price': station.price.toString(),
              'address': station.address
            }});
          });
          sendAppMessage();
        }
      }
    }
  };
  req.ontimeout = function() {
    console.log("Timed out");
  };
  req.onerror = function() {
    console.log("Connection failed");
  };
  req.send(JSON.stringify(sendData));
};

/////////////////////////////////////////////////////////////////
// PEBBLE EVENT LISTENERS
/////////////////////////////////////////////////////////////////

Pebble.addEventListener('ready', function(e) {
  if (!localStorage.grade) {
    localStorage.grade = '1';
  }
});

// RECEIVED APP MESSAGE
Pebble.addEventListener("appmessage", function(e) {
  console.log("Received message: " + JSON.stringify(e));
  if (e.payload.getStations) {
    // GET WAIT TIMES
    if (e.payload.getStations == "location") {
      getCurrentLocation(getNearbyStations);
    } else {
      getCurrentLocation(getCheapestStations);
    }
  }
});

Pebble.addEventListener("showConfiguration", function(e) {
  var grade = localStorage.grade;
  Pebble.openURL("http://logicalpixels.com/fuel.html#" + encodeURIComponent(JSON.stringify({'grade': grade})));
});

// CLOSED CONFIG WINDOW
Pebble.addEventListener("webviewclosed", function(e) {
  var configuration = JSON.parse(decodeURIComponent(e.response));
  localStorage.grade = configuration.grade.toString();
});
