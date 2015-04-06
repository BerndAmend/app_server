
var counter = 0;

setInterval(1000, function() {
	// service.notify(name, obj);
	// service.notify(clientId, name, obj);
	service.notify("test_event", {"counter": counter++);
});

service.clientDisconnected = function(clientID) {
}

service.onPing = function(clientID, args) {
	return {"from_me_for_you": "pong" };
};
