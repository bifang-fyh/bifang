## bifang - 一个基于协程的服务器框架


### 源代码目录结构

* configs/ 配置文件目录

* create/ 应用模板自动生成脚本目录

* html/ html文件目录(与对应的网络服务配合使用)

* module/ 应用模块文件目录

* src/ 源代码目录

* test/ 测试文件目录


### 编译使用方法

1.系统的依赖有zlib、openssl、boost、tcmalloc、mysqlclient、hiredis等，编译环境必须支持c++11，可尝试编译然后查看报错信息逐个安装依赖。由于MySQL的C库不同版本有所区别，所以出错需要自行排查兼容性原因，或者直接屏蔽MySQL相关代码
（屏蔽MySQL和Redis模块方法：屏蔽Makefile里面链接相关动态库的代码，然后将src/version.h文件里的MYSQL_ENABLE和REDIS_ENABLE宏屏蔽）

2.进入源代码根目录，执行make命令，Makefile将源文件链接生成为libs/libbifang.so文件，并自动连接生成可执行文件bifang，测试文件server和client

3.执行make module编译应用模块，应用模块的动态库将生成于libs/modules/目录下

4.可以使用make ragel编译几个和http解析相关的ragel文件(需先安装ragel，默认已经编译好了)

5.输入./bifang执行程序


### 日志模块

1.支持流式和格式化的日志风格，支持日志格式自定义，可通过配置文件进行配置

2.目前支持文件名、文件行号、程序运行时间、线程id、协程id、日志时间、线程名称、日志级别等配置

3.使用之前需在当前代码文件调用NameLogger(name)，RootLogger()或者SystemLogger()，以定义一个全局的日志器

4.流式日志使用：log_debug << "bifang log";

5.格式化日志使用：log_fmt_debug("%s", "bifang log");

6.更多使用方法详见src/log.cpp、src/log.h、configs/log.json


### 协程相关

1.协程封装：该协程基于ucontext_t来实现用户态的线程，比线程更轻量级

2.协程调度器：用于管理协程的调度，内部为一个线程池，协程可以在多个线程中自由切换。是一个N-M的协程调度模型，N个线程，M个协程，重复利用每一个线程

3.IO协程调度模块：继承于协程调度器，内部封装了epoll，支持定时器功能，支持任意fd读写事件的添加，删除，取消功能。支持一次性定时器，循环定时器，条件定时器等

4.hook封装：参考腾讯的libco代码，通过hook系统层面上的部分API，使协程调度器接管系统api的非阻塞的过程，可以把复杂的异步调用，封装成同步操作，降低业务逻辑的编写复杂度。hook的开启控制是线程粒度的，可以自由选择(在实现对应的hook函数的情况下任何fd均可被接管)


### Socket相关

封装了Socket类，提供所有socket API功能，统一封装了地址类，将IPv4和IPv6地址在一定程度上统一起来，并提供对应的域名、IP解析等功能。


### TcpServer模块

基于Socket类，封装了一个通用的TcpServer类，可以快速绑定一个或多个地址、启动服务、监听端口、接收连接、处理socket连接等功能。后续的http、websocket均继承自该类


### HTTP模块

1.使用Ragel来编写HTTP/1.1和uri解析的代码

2.uri正则表达式可参照(RFC 3986：http://www.ietf.org/rfc/rfc3986.txt)，里面有详细的说明，官方的正则与ragel的语法稍有不同，需参照ragel官方使用文档来进行适当修改

3.http请求和响应解析参照了Mongrel2进行实现的

4.Servlet模块：仿照java的servlet，实现了一套Servlet接口，并支持uri的精准匹配，模糊匹配等功能

5.websocket模块：基于http实现了简单的websocket模块，可以和HTTP，servlet一起配合提供网络服务


### 配置模块

使用JSON格式文件作为配置文件(JSON不支持注释，所以不能对配置文件注释，可以自行保存一份备份文件后再去大幅度修改配置信息)，支持变更通知功能，支持STL容器(vector、forward_list、list、set、unordered_set、map、unordered_map)，支持自定义类型的支持(需要自行实现序列化和反序列化方法)


### 应用模块生成脚本

进入create/目录，执行命令./create.sh 模块名，即可在module目录下生成对应名称的应用模块，并且自动添加编译信息进Makefile里面，可在生成之后进去module目录尝试编译以验证是否生成成功


### 应用模块目录

目前实现了6个简单的应用模块，全部是基于前面的http、websocket和servlet来实现的，需要根据系统环境作出修改的一些信息均置于xxx_config.h文件里了，方便修改

1.index_html模块：实现了一个简单的html页面显示，html文件置于html/index_html目录中，示例文件是nginx的默认主页，之前是为了和nginx进行压测性能对比，可自行放置文件在此目录，访问格式：http://ip:port/index_html/文件名，不输入文件名的话默认显示index.html文件

2.empty_gif模块：参考nginx的empty_gif模块实现，浏览器在访问网页时会请求一个ico文件来作为收藏夹的小图标，如果不想实现这个功能又不想报错的话可以使用此模块，作用是当浏览器请求ico文件时会返回给浏览器一个空白的gif图，以此来骗浏览器

3.chat_room模块：基于websocket实现的一个简单的聊天室，使用自定义的消息格式，html文件置于html/chat_room目录中，访问格式：http://ip:port/chat_room/，记得要自行修改html/chat_room/index.html里的websocket地址，不然websocket连接不上后台，自然也就无法正常使用

4.file_server模块：参考nginx的autoindex模块实现，将本地目录映射到网页上，访问格式：http://ip:port/file_server/xxx，单击为直接打开文件(如果文件类型不支持网页直接打开则会转为下载)，双击为下载文件。支持range请求，网页登录验证，大文件传输速度控制等等功能，MP4、MKV文件可直接在线播放(H5自带的功能，浏览器在播放视频文件时为了使用户可以自由拖拽进度条，会使用range来分段请求大的视频文件，每一次拖拽则请求对应范围的文件信息，服务器得支持这功能，不然播放视频将无法拖拽进度条)

5.webdav模块：参考nginx的webdav模块以及官方的文档实现，不同客户端对于webdav所需实现功能的要求差别很大，从目前的经验来看COPY请求是可以不实现的，客户端几乎都是自己根据需要拷贝的目录信息去递归拷贝文件和目录的，不需要服务端去实现这功能，尽管这是标准所要求添加的。目前只测试的两个客户端，winscp是允许服务器不实现LOCK，UNLOCK，PROPPATCH请求的，但是Windows的映射网络驱动器是需要我们实现这几个的，不实现的话将无法写入信息到文件中。同样也是支持range请求，登录验证，使用方法和正常映射Windows网络驱动器没区别，映射方法可以自行百度搜索，Windows网络驱动器初始是不支持http访问的，得修改系统配置，详见百度

6.doxygen模块：doxygen是一种开源跨平台的，以类似JavaDoc风格描述的文档系统。doxygen可以从一套归档源文件开始，生成HTML格式的在线类浏览器，或离线的LATEX、RTF参考手册。这里使用doxygen生成本框架的html说明文档，访问格式：http://ip:port/doxygen/。由于生成的文档比较庞大，故需要使用者自行生成，doxygen标准配置文件就是源代码根目录下的doxyconfig文件（需要自行改写html生成目录，并将doxygen应用模块的ROOT_PATH宏改写为该目录）



作者：bifang

QQ：1125498083

邮箱：1125498083@qq.com

博客地址：https://blog.csdn.net/weixin_43798887

github：https://github.com/bifang-fyh/bifang

