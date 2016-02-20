var maxAppMessageTries = 3;
var appMessageRetryTimeout = 3000;
var appMessageTimeout = 0;

function sendMessage(message) {
	Pebble.sendAppMessage(message, function(e) {
		// success
		console.log('success');
	}, function(e) {
		// error
		console.log('error');
	});
}

module.exports.sendMessage = sendMessage;
