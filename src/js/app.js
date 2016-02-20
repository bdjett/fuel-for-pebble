/****************************************************************
 * GasBuddy for Pebble
 * Created by: Brian Jett (bdjett@me.com)
 *
 * NOT ASSOCIATED WITH GASBUDDY
 ****************************************************************/

var appMessage = require('app-message');
var xhr = require('xhr');
var location = require('location');
var Clay = require('clay');
var clayConfig = require('config.json');
var clay = new Clay(clayConfig);

/////////////////////////////////////////////////////////////////
// DATA HANDLING
/////////////////////////////////////////////////////////////////

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
  var data = {
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
  var url = 'https://xml.gasbuddy.com/gbmaws/v2/Get_Stations_Near_Me.ashx?output=json';
  xhr.request('POST', url, data, function(response) {
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
      appMessage.sendMessage({
        'index': index,
        'name': station.name,
        'price': station.price.toString(),
        'address': station.address
      });
    });
  });
};

// getNearbyStations
// Given a latitude and longitude, requests the nearby stations and gas prices
var getNearbyStations = function(position) {
  console.log("Getting nearby stations");
  var data = {
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
  var url = "https://xml.gasbuddy.com/gbmaws/v2/Get_Stations_Near_Me.ashx?output=json";
  xhr.request('POST', url, data, function(response) {
    var stations = response.Payload.StationCollection;
    stations = stations.slice(0, 20);
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
        station.price = '-';
      } else {
        station.price = fuel.Prices[0].Amount;
      }
      station.name = element.Name;
      if (station.price === 0) {
        station.price = '-';
      }
      appMessage.send({
        'index': index,
        'name': station.name,
        'price': station.price.toString(),
        'address': station.address
      });
    });
  });
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
      location.currentLocation(getNearbyStations);
    } else {
      location.currentLocation(getCheapestStations);
    }
  }
});

Pebble.addEventListener("showConfiguration", function(e) {
  Pebble.openURL(clay.generateUrl());
});

// CLOSED CONFIG WINDOW
Pebble.addEventListener("webviewclosed", function(e) {
  var configuration = clay.getSettings(e.response);
  localStorage.grade = configuration.grade.toString();
});
