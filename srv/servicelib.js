/*
	Protocol:
		Command:
			Client -> Server
			{
				type: "request"
				service: "Test"
				id: 1
				data: {}
			}

			Server -> Client
			{
				type: "reply"
				service: "Test"
				id: 1
				data: {}
			}

		Client -> Server
		Server -> Client
		Event:
			{
				type: "event"
				service: "Test"
				data: {}
			}
*/

servicelib = (function () {
	function Servicelib() {

		// These should be treated as read-only properties

		this.debug = false;
		this.reconnectInterval = 1000; /** The number of milliseconds to delay before attempting to reconnect. */
		this.timeoutInterval = 2000; /** The maximum time in milliseconds to wait for a connection to succeed before closing and retrying. */

		/**
		 * The current state of the connection.
		 * Can be one of: WebSocket.CONNECTING, WebSocket.OPEN, WebSocket.CLOSING, WebSocket.CLOSED
		 * Read only.
		 */
		this.readyState = WebSocket.CONNECTING;

		// Private state variables
		var timedOut = false;
		var ws;
		var last_id = 0;
		var events = {};
		var pending_requests = {};

		var trigger = function(evt, data) {
			if (events.hasOwnProperty(evt) === false) {
				return false;
			}

			events[evt].forEach(function(handler) {
				handler.call(this, data);
			}.bind(this));
		}.bind(this);

		var callCallback = function(id, data) {
			var p = pending_requests[id];
			if(p.callback !== undefined) {
				p.callback(data, p.custom_data);
			}
			clearTimeout(p.timeout_id);
			delete pending_requests[id];
		}.bind(this);

		var timeoutCallback = function(id) {
			callCallback(id, null);
		}.bind(this);

		this.on = function(name, cmd, callback) {
			var evt, handler;
			if(callback === undefined) {
				// ensure that late joiners get the correct initial state
				if(name === "connect") {
					if(this.readyState === WebSocket.OPEN)
						cmd();
				} else if(name === "disconnect") {
					if(this.readyState === WebSocket.CLOSED)
						cmd();
				}

				evt = name;
				handler = cmd;
			} else {
				evt = name + ":" + cmd;
				handler = callback;
			}

			if (events.hasOwnProperty(evt) === false) {
				events[evt] = [];
			}
			events[evt].push(handler)
		};

		this.off = function(name, cmd, callback) {
			var evt, handler;
			if(callback === undefined) {
				evt = name;
				handler = cmd;
			} else {
				evt = name + ":" + cmd;
				handler = callback;
			}
			events[evt] = events[evt].filter(function(h) {
				return h !== handler;
			});
		};

		this.request = function(name, cmd, args, callback, timeout, custom_data) {
			last_id++;

			var id = last_id;

			// start timeout timer
			var timeout_id = setTimeout(function() {
				timeoutCallback(id);
			}, timeout);

			// insert request into the request list
			pending_requests[id] = {
				"callback": callback,
				"custom_data": custom_data,
				"timeout_id": timeout_id
			};

			// send request
			ws.send(JSON.stringify({
				"type": "request",
				"service": name,
				"cmd": cmd,
				"id": id,
				"data": args
			}));
		};


		var open = function (reconnectAttempt) {
			ws = new WebSocket("ws://" + location.hostname + ":55777");

			if (this.debug) {
				console.debug('servicelib', 'attempt-connect');
			}

			var localWs = ws;
			var timeout = setTimeout(function() {
				if (this.debug) {
					console.debug('servicelib', 'connection-timeout');
				}
				timedOut = true;
				localWs.close();
				timedOut = false;
			}, this.timeoutInterval);

			ws.onopen = function(event) {
				clearTimeout(timeout);
				if (this.debug) {
					console.debug('servicelib', 'connect');
				}
				this.readyState = WebSocket.OPEN;
				trigger("connect", reconnectAttempt);
				reconnectAttempt = false;
			}.bind(this);

			ws.onclose = function(event) {
				clearTimeout(timeout);
				ws = null;
				this.readyState = WebSocket.CONNECTING;
				var e = {};
				trigger("connecting", e);
				if (!reconnectAttempt && !timedOut) {
					if (this.debug) {
						console.debug('servicelib', 'disconnect');
					}
					trigger("disconnect");
				}

				// inform all pending requests, that they failed
				for (var key in pending_requests) {
					if (pending_requests.hasOwnProperty(key)) {
						var obj = pending_requests[key];
						clearTimeout(obj.timeout_id);
					}
				}

				pending_requests = {};
				last_id = 0;

				setTimeout(function() {
					open(true);
				}, this.reconnectInterval);
			}.bind(this);

			ws.onerror = function() {
				if (this.debug) {
					console.log("socket error");
				}
				trigger("error");
			}.bind(this);

			ws.onmessage = function(pkg) {
				var msg = JSON.parse(pkg.data);
				if(msg.type === "event") {
					trigger(msg.service + ":" + msg.cmd, msg.data);
				} else if(msg.type === "reply") {
					if(pending_requests[msg.id] === undefined) {
						// message received, after the timeout already hit
						if(this.debug) {
							console.log("message received, after the timeout already hit: " + JSON.stringify(msg) );
						}
					}  else {
						if(msg.error === true) {
							trigger("error", "unknown command");
						} else {
							callCallback(msg.id, msg.data);
						}
					}
				} else {
					console.log("Unknown message received " + msg.type );
				}
			}.bind(this);
		}.bind(this);

		open(false);
	}

	return new Servicelib();
}
)();
