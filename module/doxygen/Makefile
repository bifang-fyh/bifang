.PHONY:modules clean

CPP = g++
CFLAGS = -std=c++11 -O1 -W -fPIC
# -g
CLIBS = -lbifang
COBJS = objs/doxygen
CPATH = module/doxygen
SOFILE = doxygen.so

#生成目标文件存放目录
$(shell test -d $(COBJS) || mkdir $(COBJS))


MODULE_OBJS = \
	$(COBJS)/doxygen_module.o \
	$(COBJS)/doxygen_servlet.o

MODULE_DEPS = \
	$(CPATH)/doxygen_module.h \
	$(CPATH)/doxygen_servlet.h \
	$(CPATH)/doxygen_config.h

modules:$(MODULE_PATH)/$(SOFILE)

$(MODULE_PATH)/$(SOFILE):$(MODULE_OBJS)
	$(CPP) $(CFLAGS) $(SRC_INCS) $(CLIBS) -shared -o $@ $^

$(COBJS)/%.o:$(CPATH)/%.cpp $(SRC_DEPS) $(MODULE_DEPS)
	$(CPP) -c $(CFLAGS) $(SRC_INCS) $(CLIBS) $< -o $@

clean:
	rm -rf $(COBJS)/
