all:
	g++ -Wall -ansi -g -I/home/bnoordhuis/opt/nodejs/include/node/ -lfcgi -fPIC -shared -o fastcgi.node fastcgi.cc
