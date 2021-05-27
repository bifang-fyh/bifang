.PHONY:all shared bifang server client module module_clean ragel clean

#生成目标文件存放目录
$(shell test -d objs || mkdir objs)
#生成日志文件存放目录
$(shell test -d logs || mkdir logs)
#生成动态库文件存放目录
$(shell test -d libs || mkdir libs)

CPP = g++
CFLAGS = -std=c++11 -pipe -O1 -W -fPIC
#-g
LTCMALLOC = -ltcmalloc_minimal
CLIBS = -lpthread -lm -ldl -lz -lssl -lcrypto -lmysqlclient -lhiredis
DYNAMIC_PATH = libs/libbifang.so

SRC_OBJS = \
	objs/Assert.o \
	objs/thread.o \
	objs/util.o \
	objs/bencode.o \
	objs/process.o \
	objs/log.o \
	objs/address.o \
	objs/socket.o \
	objs/buffer.o \
	objs/fiber.o \
	objs/scheduler.o \
	objs/fdregister.o \
	objs/timer.o \
	objs/iocontext.o \
	objs/iomanager_epoll.o \
	objs/hook.o \
	objs/environment.o \
	objs/Iconv.o \
	objs/stream.o \
	objs/config.o \
	objs/worker.o \
	objs/uri.o \
	objs/module.o \
	objs/authorization.o \
	objs/tcpserver.o \
	objs/udpserver.o \
	objs/server.o \
	\
	objs/json_value.o \
	objs/json_reader.o \
	objs/json_writer.o \
	\
	objs/socket_stream.o \
	objs/zlib_stream.o \
	\
	objs/http_common.o \
	objs/http_server_parse.o \
	objs/http_client_parse.o \
	objs/http_parse.o \
	objs/http_session.o \
	objs/http_client.o \
	objs/servlet.o \
	objs/http_server.o \
	\
	objs/ws_session.o \
	objs/ws_client.o \
	objs/ws_servlet.o \
	objs/ws_server.o \
	\
	objs/mysql.o \
	objs/redis.o

SRC_DEPS = \
	src/version.h \
	src/bifang.h \
	src/noncopyable.h \
	src/endian_cpp.h \
	src/singleton.h \
	src/lock.h \
	src/Assert.h \
	src/thread.h \
	src/util.h \
	src/bencode.h \
	src/process.h \
	src/log.h \
	src/address.h \
	src/socket.h \
	src/buffer.h \
	src/fiber.h \
	src/scheduler.h \
	src/fdregister.h \
	src/timer.h \
	src/iocontext.h \
	src/iomanager.h \
	src/hook.h \
	src/environment.h \
	src/Iconv.h \
	src/stream.h \
	src/config.h \
	src/worker.h \
	src/uri.h \
	src/module.h \
	src/authorization.h \
	src/tcpserver.h \
	src/udpserver.h \
	src/server.h \
	\
	src/json/json.h \
	\
	src/stream/socket_stream.h \
	src/stream/zlib_stream.h \
	\
	src/http/http_common.h \
	src/http/http_server_parse.h \
	src/http/http_client_parse.h \
	src/http/http_parse.h \
	src/http/http_session.h \
	src/http/http_client.h \
	src/http/servlet.h \
	src/http/http_server.h \
	\
	src/ws/ws_session.h \
	src/ws/ws_client.h \
	src/ws/ws_servlet.h \
	src/ws/ws_server.h \
	\
	src/sql/mysql.h \
	src/sql/redis.h

SRC_INCS = \
	-I src \
	-I src/json \
	-I src/stream \
	-I src/http \
	-I src/ws \
	-I src/sql \
	-L libs \
	-L /usr/lib64/mysql \
	-L /usr/lib64 \
	-Wl,--copy-dt-needed-entries \
	-Wl,-rpath=libs

all: bifang server client

#生成动态链接库
shared:$(SRC_OBJS)
	$(CPP) $(CFLAGS) $(SRC_INCS) $(CLIBS) $(LTCMALLOC) -shared -o $(DYNAMIC_PATH) $^

#生成可执行文件
bifang:objs/bifang.o shared $(SRC_DEPS)
	$(CPP) $(CFLAGS) $(SRC_INCS) -lbifang $(LTCMALLOC) -o $@ $<

#生成服务端测试代码
server:objs/malloc_test.o shared $(SRC_DEPS)
	$(CPP) $(CFLAGS) $(SRC_INCS) -lbifang $(LTCMALLOC) -o $@ $<

#生成客户端测试代码
client:objs/fiber_test.o shared $(SRC_DEPS) 
	$(CPP) $(CFLAGS) $(SRC_INCS) -lbifang $(LTCMALLOC) -o $@ $<

#生成测试代码的.o文件
objs/%.o:test/%.cpp $(SRC_DEPS)
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $< -o $@

#生成源文件的各个.o文件
objs/%.o:src/%.cpp $(SRC_DEPS)
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $< -o $@

#生成JSON库的.o文件
objs/%.o:src/json/%.cpp $(SRC_DEPS) 
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $< -o $@

#生成STREAM库的.o文件
objs/%.o:src/stream/%.cpp $(SRC_DEPS) 
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $< -o $@

#生成HTTP库的.o文件
objs/%.o:src/http/%.cpp $(SRC_DEPS) 
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $< -o $@

#生成WEBSOCKET库的.o文件
objs/%.o:src/ws/%.cpp $(SRC_DEPS) 
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $< -o $@

#生成SQL库的.o文件
objs/%.o:src/sql/%.cpp $(SRC_DEPS) 
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $< -o $@


#编译模块文件
export SRC_DEPS
export SRC_INCS
export LTCMALLOC
module:
	$(MAKE) -f module/Makefile

module_clean:
	$(MAKE) clean -f module/Makefile

#编译ragel文件(需先安装ragel)
ragel:
	ragel -G2 src/uri.rl -o src/uri.cpp
	ragel -G2 src/http/http_server_parse.rl -o src/http/http_server_parse.cpp
	ragel -G2 src/http/http_client_parse.rl -o src/http/http_client_parse.cpp

clean:
	rm -rf *.o objs/ logs/ libs/ server client bifang core*
