function request(method, url, data, success, error) {
	var req = new XMLHttpRequest();
  req.open(method, url, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if (req.status == 200) {
        if (req.responseText) {
          var response = JSON.parse(req.responseText);
					if (success) success(response);
				}
      } else {
				if (error) error();
			}
    } else {
			if (error) error();
		}
  };
  req.ontimeout = function() {
    console.log("Timed out");
		if (error) error();
  };
  req.onerror = function() {
    console.log("Connection failed");
		if (error) error();
  };
  req.send(JSON.stringify(data));
}

module.exports.request = request;
