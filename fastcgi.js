IOWatcher = process.binding('io_watcher').IOWatcher;
bindings = require('./fastcgi.node');
fs = require('fs');

FCGI_LISTENSOCK_FILENO = 0;

function ResponderRequest() {	
};

function ResponderResponse() {	
};

exports.ResponderRequest = ResponderRequest;
exports.ResponderResponse = ResponderResponse;

exports.isFastCGI = function() {
	return fs.fstatSync(FCGI_LISTENSOCK_FILENO).isSocket();
};

exports.authorizer = function(callback) {
	throw new Error('Not yet implemented.');
};

exports.filter = function(callback) {
	throw new Error('Not yet implemented.');
};

exports.responder = function(callback) {
	watcher = new IOWatcher();
	watcher.callback = function() {
		// TODO catch exception and emit as error event
		clientfd = bindings.accept(FCGI_LISTENSOCK_FILENO);

		// TODO set up FCGI environment
		request = new ResponderRequest();
		response = new ResponderResponse();
		callback(request, response);
	};
	watcher.set(FCGI_LISTENSOCK_FILENO, true, false); // read=true, write=false
	watcher.start();
};
