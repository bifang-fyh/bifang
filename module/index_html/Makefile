.PHONY:modules clean

CPP = g++
CFLAGS = -std=c++11 -O1 -W -fPIC
# -g
CLIBS = -lbifang
COBJS = objs/index_html
CPATH = module/index_html
SOFILE = index_html.so

#生成目标文件存放目录
$(shell test -d $(COBJS) || mkdir $(COBJS))


MODULE_OBJS = \
	$(COBJS)/index_html_servlet.o \
	$(COBJS)/index_html_module.o

MODULE_DEPS = \
	$(CPATH)/index_html_servlet.h \
	$(CPATH)/index_html_module.h \
	$(CPATH)/index_html_config.h

modules:$(MODULE_PATH)/$(SOFILE)

$(MODULE_PATH)/$(SOFILE):$(MODULE_OBJS)
	$(CPP) $(CFLAGS) $(SRC_INCS) $(CLIBS) $(LTCMALLOC) -shared -o $@ $^

$(COBJS)/%.o:$(CPATH)/%.cpp $(SRC_DEPS) $(MODULE_DEPS)
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $(CLIBS) $(LTCMALLOC) $< -o $@

clean:
	rm -rf $(COBJS)/ $(modules)
