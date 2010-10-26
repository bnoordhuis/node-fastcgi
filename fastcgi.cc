#include <fcgi_stdio.h>

#include <node.h>
#include <v8.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

namespace {

using namespace v8;

Handle<Value> Accept(const Arguments& args) {
	HandleScope scope;

	assert(args[0]->IsInt32());

	const int listenfd = args[0]->Int32Value();
	sockaddr_in sin;
	socklen_t len;
	len = sizeof(sin);

	const int clientfd = accept(listenfd, (sockaddr *) &sin, &len);
	if (clientfd == -1) {
		return ThrowException(node::ErrnoException(errno, "accept", "", 0));
	}
	return scope.Close(Integer::New(clientfd));
}

Handle<Value> Dup2(const Arguments& args) {
	HandleScope scope;

	assert(args[0]->IsInt32());
	assert(args[1]->IsInt32());

	const int oldfd = args[0]->Int32Value();
	const int newfd = args[1]->Int32Value();

	if (dup2(oldfd, newfd) == -1) {
		return ThrowException(node::ErrnoException(errno, "dup2", "", 0));
	}
	return scope.Close(Integer::New(newfd));
}

void RegisterModule(Handle<Object> target) {
	HandleScope scope;

	target->Set(String::NewSymbol("accept"), FunctionTemplate::New(Accept)->GetFunction());
	target->Set(String::NewSymbol("dup2"), FunctionTemplate::New(Dup2)->GetFunction());
}

} // anonymous namespace

NODE_MODULE(fastcgi, RegisterModule);
