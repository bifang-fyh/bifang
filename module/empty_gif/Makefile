.PHONY:modules clean

CPP = g++
CFLAGS = -std=c++11 -O1 -W -fPIC
# -g
CLIBS = -lbifang
COBJS = objs/empty_gif
CPATH = module/empty_gif
SOFILE = empty_gif.so

#生成目标文件存放目录
$(shell test -d $(COBJS) || mkdir $(COBJS))


MODULE_OBJS = \
	$(COBJS)/empty_gif_module.o \
	$(COBJS)/empty_gif_servlet.o

MODULE_DEPS = \
	$(CPATH)/empty_gif_module.h \
	$(CPATH)/empty_gif_servlet.h \
	$(CPATH)/empty_gif_config.h

modules:$(MODULE_PATH)/$(SOFILE)

$(MODULE_PATH)/$(SOFILE):$(MODULE_OBJS)
	$(CPP) $(CFLAGS) $(SRC_INCS) $(CLIBS) $(LTCMALLOC) -shared -o $@ $^

$(COBJS)/%.o:$(CPATH)/%.cpp $(SRC_DEPS) $(MODULE_DEPS)
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $(CLIBS) $(LTCMALLOC) $< -o $@

clean:
	rm -rf $(COBJS)/
