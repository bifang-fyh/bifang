.PHONY:modules clean

CPP = g++
CFLAGS = -std=c++11 -O1 -W -fPIC
# -g
CLIBS = -lbifang
COBJS = objs/file_server
CPATH = module/file_server
SOFILE = file_server.so

#生成目标文件存放目录
$(shell test -d $(COBJS) || mkdir $(COBJS))


MODULE_OBJS = \
	$(COBJS)/file_server_module.o \
	$(COBJS)/file_server_servlet.o \
	$(COBJS)/file_server_autoindex.o

MODULE_DEPS = \
	$(CPATH)/file_server_module.h \
	$(CPATH)/file_server_servlet.h \
	$(CPATH)/file_server_autoindex.h \
	$(CPATH)/file_server_config.h

modules:$(MODULE_PATH)/$(SOFILE)

$(MODULE_PATH)/$(SOFILE):$(MODULE_OBJS)
	$(CPP) $(CFLAGS) $(SRC_INCS) $(CLIBS) -shared -o $@ $^

$(COBJS)/%.o:$(CPATH)/%.cpp $(SRC_DEPS) $(MODULE_DEPS)
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $(CLIBS) $< -o $@

clean:
	rm -rf $(COBJS)/
