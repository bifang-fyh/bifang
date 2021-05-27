.PHONY:modules clean

CPP = g++
CFLAGS = -std=c++11 -O1 -W -fPIC
# -g
CLIBS = -lbifang
COBJS = objs/chat_room
CPATH = module/chat_room
SOFILE = chat_room.so

#生成目标文件存放目录
$(shell test -d $(COBJS) || mkdir $(COBJS))


MODULE_OBJS = \
	$(COBJS)/chat_room_module.o \
	$(COBJS)/chat_room_protocol.o \
	$(COBJS)/chat_http_servlet.o \
	$(COBJS)/chat_ws_servlet.o

MODULE_DEPS = \
	$(CPATH)/chat_room_module.h \
	$(CPATH)/chat_room_protocol.h \
	$(CPATH)/chat_http_servlet.h \
	$(CPATH)/chat_ws_servlet.h \
	$(CPATH)/chat_room_config.h

modules:$(MODULE_PATH)/$(SOFILE)

$(MODULE_PATH)/$(SOFILE):$(MODULE_OBJS)
	$(CPP) $(CFLAGS) $(SRC_INCS) $(CLIBS) $(LTCMALLOC) -shared -o $@ $^

$(COBJS)/%.o:$(CPATH)/%.cpp $(SRC_DEPS) $(MODULE_DEPS)
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $(CLIBS) $(LTCMALLOC) $< -o $@

clean:
	rm -rf $(COBJS)/
