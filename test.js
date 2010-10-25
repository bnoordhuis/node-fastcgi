fastcgi = require('./fastcgi');
util = require('util');

fastcgi.responder(function(req, res) {
	res.write(
		  "Status: 201 Created\n"
		+ "Content-Type: text/plain\n"
		+ "\n"
		+ "Hello, FastCGI world!\n");
	res.write("Headers: " + util.inspect(req.headers) + "\n");
});

console.log("Done.");