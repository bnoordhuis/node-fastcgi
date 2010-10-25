#include <fcgi_stdio.h>
#include <node.h>
#include <v8.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

namespace {

using namespace v8;

// watches the accept() socket
ev_io incoming_connection_watcher;

void incoming_connection_callback(struct ev_loop *loop, ev_io *watcher, int revents) {
	// TODO accept() connection
}

bool is_socket(int fd) {
	struct stat s;
	return fstat(fd, &s) != -1 && S_ISSOCK(s.st_mode);
}

bool is_fastcgi() {
	// FIXME tighter check
	return is_socket(STDIN_FILENO);
}

Handle<Value> IsFastCGI(const Arguments& args) {
	return is_fastcgi() ? True() : False();
}

Handle<Value> Write(const Arguments& args) {
	assert(args[0]->IsString());
	String::Utf8Value s(args[0]->ToString());
	fputs(*s, stdout);
	fflush(stdout);
	return Undefined();
}

Handle<Value> Responder(const Arguments& args) {
	HandleScope scope;

	if (!args[0]->IsFunction()) {
		Local<Value> ex = Exception::Error(String::New("Argument must be a callback."));
		return ThrowException(ex);
	}

	Function* callback = Function::Cast(*args[0]);
	while (FCGI_Accept() == 0) {
		TryCatch tc;

		Local<Object> req = Object::New();
		Local<Object> res = Object::New();

		// FIXME optimize
		Local<Array> requestHeaders = Array::New(0);
		for (int i = 0; environ[i]; ++i) {
			requestHeaders->Set(i, String::New(environ[i]));
		}

		req->Set(String::NewSymbol("headers"), requestHeaders);
		res->Set(String::NewSymbol("write"), FunctionTemplate::New(Write)->GetFunction());

		Handle<Value> argv[2] = { req, res };
		Local<Value> rv = callback->Call(Context::GetCurrent()->Global(), 2, argv);

		if (tc.HasCaught()) {
			// tc.ReThrow()? or not throw at all?
			return tc.Exception();
		}
	}

	return Undefined();
}

void RegisterModule(Handle<Object> target) {
	target->Set(String::NewSymbol("isFastCGI"), FunctionTemplate::New(IsFastCGI)->GetFunction());
	target->Set(String::NewSymbol("responder"), FunctionTemplate::New(Responder)->GetFunction());

	if (true || is_fastcgi()) {
		// fd 0 is a socket; strange things happen when a nodejs application tries to read from it so move it
		const int fd = dup2(0, open("/", O_RDONLY));

		// put socket in non-blocking mode
		const int flags = fcntl(fd, F_GETFL);
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);

		// add it to the watch list
		ev_init(&incoming_connection_watcher, incoming_connection_callback);
		ev_io_set(&incoming_connection_watcher, fd, EV_READ | EV_WRITE);	// accept() socket so EV_WRITE is probably enough (but add EV_ERROR?)
		ev_io_start(EV_DEFAULT_UC, &incoming_connection_watcher);
	}
}

} // anonymous namespace

NODE_MODULE(fastcgi, RegisterModule);
