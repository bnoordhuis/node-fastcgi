ifndef NODE_PATH
	NODE_PATH=/home/bnoordhuis/opt/nodejs
endif

all:
	g++ -Wall -ansi -g -I$(NODE_PATH)/include/node/ -fPIC -shared -o fastcgi.node fastcgi.cc
