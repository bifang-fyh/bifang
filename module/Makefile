.PHONY:default clean

MODULE_PATH = libs/modules
export MODULE_PATH

#生成模块文件存放目录
$(shell test -d $(MODULE_PATH) || mkdir $(MODULE_PATH))


default:
#$(MAKE) -f module/webdav/Makefile
	$(MAKE) -f module/file_server/Makefile
	$(MAKE) -f module/empty_gif/Makefile
	$(MAKE) -f module/chat_room/Makefile
	$(MAKE) -f module/index_html/Makefile

clean:
	rm -rf $(MODULE_PATH)/
#$(MAKE) clean -f module/webdav/Makefile
	$(MAKE) clean -f module/file_server/Makefile
	$(MAKE) clean -f module/empty_gif/Makefile
	$(MAKE) clean -f module/chat_room/Makefile
	$(MAKE) clean -f module/index_html/Makefile
