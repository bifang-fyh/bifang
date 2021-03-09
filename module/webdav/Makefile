.PHONY:modules clean

CPP = g++
CFLAGS = -std=c++11 -O1 -W -fPIC
# -g
CLIBS = -lbifang
COBJS = objs/webdav
CPATH = module/webdav
SOFILE = webdav.so

#生成目标文件存放目录
$(shell test -d $(COBJS) || mkdir $(COBJS))


MODULE_OBJS = \
	$(COBJS)/webdav_module.o \
	$(COBJS)/webdav_servlet.o \
	$(COBJS)/webdav_action.o \
	$(COBJS)/webdav_lock.o

MODULE_DEPS = \
	$(CPATH)/webdav_module.h \
	$(CPATH)/webdav_servlet.h \
	$(CPATH)/webdav_action.h \
	$(CPATH)/webdav_lock.h \
	$(CPATH)/webdav_config.h

modules:$(MODULE_PATH)/$(SOFILE)

$(MODULE_PATH)/$(SOFILE):$(MODULE_OBJS)
	$(CPP) $(CFLAGS) $(SRC_INCS) $(CLIBS) -shared -o $@ $^

$(COBJS)/%.o:$(CPATH)/%.cpp $(SRC_DEPS) $(MODULE_DEPS)
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $(CLIBS) $< -o $@

clean:
	rm -rf $(COBJS)/
