/**
 * details:
 *     100: 客户端应当继续发送请求。这个临时响应是用来通知客户端它的部分请求已经
 *          被服务器接收，且仍未被拒绝。客户端应当继续发送请求的剩余部分，或者如
 *          果请求已经完成，忽略这个响应。服务器必须在请求完成后向客户端发送一个
 *          最终响应。
 *     101: 服务器已经理解了客户端的请求，并将通过 Upgrade 消息头通知客户端采用
 *          不同的协议来完成这个请求。在发送完这个响应最后的空行后，服务器将会切
 *          换到在 Upgrade 消息头中定义的那些协议。只有在切换新的协议更有好处的
 *          时候才应该采取类似措施。例如，切换到新的 HTTP 版本比旧版本更有优势，
 *          或者切换到一个实时且同步的协议以传送利用此类特性的资源。
 *     102: 由WebDAV(RFC 2518)扩展的状态码，代表处理将被继续执行。
 *     200: 请求已成功，请求所希望的响应头或数据体将随此响应返回。
 *     201: 请求已经被实现，而且有一个新的资源已经依据请求的需要而建立，且其 URI
 *          已经随 Location 头信息返回。假如需要的资源无法及时建立的话，应当返回
 *          "202 Accepted"。
 *     202: 服务器已接受请求，但尚未处理。正如它可能被拒绝一样，最终该请求可能会
 *          也可能不会被执行。在异步操作的场合下，没有比发送这个状态码更方便的做
 *          法了。返回 202 状态码的响应的目的是允许服务器接受其他过程的请求(例如
 *          某个每天只执行一次的基于批处理的操作)，而不必让客户端一直保持与服务
 *          器的连接直到批处理操作全部完成。在接受请求处理并返回 202 状态码的响
 *          应应当在返回的实体中包含一些指示处理当前状态的信息，以及指向处理状态
 *          监视器或状态预测的指针，以便用户能够估计操作是否已经完成。
 *     203: 服务器已成功处理了请求，但返回的实体头部元信息不是在原始服务器上有效
 *          的确定集合，而是来自本地或者第三方的拷贝。当前的信息可能是原始版本的
 *          子集或者超集。例如，包含资源的元数据可能导致原始服务器知道元信息的超
 *          级。使用此状态码不是必须的，而且只有在响应不使用此状态码便会
 *          返回 200 OK的情况下才是合适的。
 *     204: 服务器成功处理了请求，但不需要返回任何实体内容，并且希望返回更新了的
 *          元信息。响应可能通过实体头部的形式，返回新的或更新后的元信息。如果存
 *          在这些头部信息，则应当与所请求的变量相呼应。如果客户端是浏览器的话，
 *          那么用户浏览器应保留发送了该请求的页面，而不产生任何文档视图上的变
 *          化，即使按照规范新的或更新后的元信息应当被应用到用户浏览器活动视图中
 *          的文档。由于 204 响应被禁止包含任何消息体，因此它始终以消息头后的第
 *          一个空行结尾。
 *     205: 服务器成功处理了请求，且没有返回任何内容。但是与 204 响应不同，返回
 *          此状态码的响应要求请求者重置文档视图。该响应主要是被用于接受用户输入
 *          后，立即重置表单，以便用户能够轻松地开始另一次输入。与 204 响应一样，
 *          该响应也被禁止包含任何消息体，且以消息头后的第一个空行结束。
 *     206: 服务器已经成功处理了部分 GET 请求。类似于 FlashGet 或者迅雷这类的
 *          HTTP 下载工具都是使用此类响应实现断点续传或者将一个大文档分解为多个
 *          下载段同时下载。该请求必须包含 Range 头信息来指示客户端希望得到的内
 *          容范围，并且可能包含 If-Range 来作为请求条件。响应必须包含如下的头部
 *          域：Content-Range 用以指示本次响应中返回的内容的范围；如果是
 *          Content-Type 为 multipartbyteranges 的多段下载，则每一 multipart 段
 *          中都应包含 Content-Range 域用以指示本段的内容范围。假如响应中包含
 *          Content-Length，那么它的数值必须匹配它返回的内容范围的真实字节数。
 *          Date ETag 和或 Content-Location，假如同样的请求本应该返回200响应。
 *          Expires, Cache-Control，和或 Vary，假如其值可能与之前相同变量的其他
 *          响应对应的值不同的话。假如本响应请求使用了 If-Range 强缓存验证，那么
 *          本次响应不应该包含其他实体头；假如本响应的请求使用了 If-Range 弱缓存
 *          验证，那么本次响应禁止包含其他实体头；这避免了缓存的实体内容和更新了
 *          的实体头信息之间的不一致。否则，本响应就应当包含所有本应该返回
 *          200 响应中应当返回的所有实体头部域。假如 ETag 或 Last-Modified 头部
 *          不能精确匹配的话，则客户端缓存应禁止将206响应返回的内容与之前任何缓
 *          存过的内容组合在一起。任何不支持 Range 以及 Content-Range 头的缓存都
 *          禁止缓存 206 响应返回的内容。
 *     207: 由WebDAV(RFC 2518)扩展的状态码，代表之后的消息体将是一个XML消息，并
 *          且可能依照之前子请求数量的不同，包含一系列独立的响应代码。
 *     300: 被请求的资源有一系列可供选择的回馈信息，每个都有自己特定的地址和浏览
 *          器驱动的商议信息。用户或浏览器能够自行选择一个首选的地址进行重定向。
 *          除非这是一个 HEAD 请求，否则该响应应当包括一个资源特性及地址的列表的
 *          实体，以便用户或浏览器从中选择最合适的重定向地址。这个实体的格式由
 *          Content-Type 定义的格式所决定。浏览器可能根据响应的格式以及浏览器自
 *          身能力，自动作出最合适的选择。当然，RFC 2616规范并没有规定这样的自动
 *          选择该如何进行。如果服务器本身已经有了首选的回馈选择，那么在
 *          Location 中应当指明这个回馈的 URI；浏览器可能会将这个 Location 值作
 *          为自动重定向的地址。此外，除非额外指定，否则这个响应也是可缓存的。
 *     301: 被请求的资源已永久移动到新位置，并且将来任何对此资源的引用都应该使用
 *          本响应返回的若干个 URI 之一。如果可能，拥有链接编辑功能的客户端应当
 *          自动把请求的地址修改为从服务器反馈回来的地址。除非额外指定，否则这个
 *          响应也是可缓存的。新的永久性的 URI 应当在响应的 Location 域中返回。
 *          除非这是一个 HEAD 请求，否则响应的实体中应当包含指向新的 URI 的超链
 *          接及简短说明。如果这不是一个 GET 或者 HEAD 请求，因此浏览器禁止自动
 *          进行重定向，除非得到用户的确认，因为请求的条件可能因此发生变化。
 *          注意：对于某些使用 HTTP1.0 协议的浏览器，当它们发送的 POST 请求得到
 *          了一个 301 响应的话，接下来的重定向请求将会变成 GET 方式。
 *     302: 请求的资源现在临时从不同的 URI 响应请求。由于这样的重定向是临时的，
 *          客户端应当继续向原有地址发送以后的请求。只有在 Cache-Control 或
 *          Expires 中进行了指定的情况下，这个响应才是可缓存的。新的临时性的
 *          URI 应当在响应的 Location 域中返回。除非这是一个 HEAD 请求，否则响应
 *          的实体中应当包含指向新的 URI 的超链接及简短说明。如果这不是一个 GET 
 *          或者 HEAD 请求，那么浏览器禁止自动进行重定向，除非得到用户的确认，因
 *          为请求的条件可能因此发生变化。注意：虽然 RFC 1945 和 RFC 2068 规范不
 *          允许客户端在重定向时改变请求的方法，但是很多现存的浏览器将 302 响应
 *          视作为 303 响应，并且使用 GET 方式访问在 Location 中规定的 URI，而无
 *          视原先请求的方法。状态码 303 和 307 被添加了进来，用以明确服务器期待
 *          客户端进行何种反应。
 *     303: 对应当前请求的响应可以在另一个 URI 上被找到，而且客户端应当采用 GET
 *          的方式访问那个资源。这个方法的存在主要是为了允许由脚本激活的 POST 请
 *          求输出重定向到一个新的资源。这个新的 URI 不是原始资源的替代引用。同
 *          时，303 响应禁止被缓存。当然，第二个请求(重定向)可能被缓存。新的 URI
 *          应当在响应的 Location 域中返回。除非这是一个 HEAD 请求，否则响应的实
 *          体中应当包含指向新的 URI 的超链接及简短说明。注意：许多 HTTP1.1版以
 *          前的 浏览器不能正确理解 303 状态。如果需要考虑与这些浏览器之间的互
 *          动，302 状态码应该可以胜任，因为大多数的浏览器处理 302 响应时的方式
 *          恰恰就是上述规范要求客户端处理303响应时应当做的。
 *     304: 如果客户端发送了一个带条件的 GET 请求且该请求已被允许，而文档的内容
 *          (自上次访问以来或者根据请求的条件)并没有改变，则服务器应当返回这个状
 *          态码。304 响应禁止包含消息体，因此始终以消息头后的第一个空行结尾。该
 *          响应必须包含以下的头信息：Date，除非这个服务器没有时钟。假如没有时钟
 *          的服务器也遵守这些规则，那么代理服务器以及客户端可以自行将 Date 字段
 *          添加到接收到的响应头中去(正如RFC 2068中规定的一样)，缓存机制将会正常
 *          工作。ETag 和或 Content-Location，假如同样的请求本应返回 200 响应。
 *          Expires, Cache-Control，和或 Vary，假如其值可能与之前相同变量的其他
 *          响应对应的值不同的话。假如本响应请求使用了强缓存验证，那么本次响应不
 *          应该包含其他实体头；否则(例如，某个带条件的 GET 请求使用了弱缓存验
 *          证)，本次响应禁止包含其他实体头；这避免了缓存了的实体内容和更新了的
 *          实体头信息之间的不一致。假如某个 304 响应指明了当前某个实体没有缓存，
 *          那么缓存系统必须忽视这个响应，并且重复发送不包含限制条件的请求。假如
 *          接收到一个要求更新某个缓存条目的 304 响应，那么缓存系统必须更新整个
 *          条目以反映所有在响应中被更新的字段的值。
 *     305: 被请求的资源必须通过指定的代理才能被访问。Location 域中将给出指定的
 *          代理所在的 URI 信息，接收者需要重复发送一个单独的请求，通过这个代理
 *          才能访问相应资源。只有原始服务器才能建立 305 响应。注意：RFC 2068中
 *          没有明确 305 响应是为了重定向一个单独的请求，而且只能被原始服务器建
 *          立。忽视这些限制可能导致严重的安全后果。
 *     306: 在最新版的规范中，306 状态码已经不再被使用。
 *     307: 请求的资源现在临时从不同的 URI 响应请求。由于这样的重定向是临时的，
 *          客户端应当继续向原有地址发送以后的请求。只有在 Cache-Control 或
 *          Expires 中进行了指定的情况下，这个响应才是可缓存的。新的临时性的 URI
 *          应当在响应的 Location 域中返回。除非这是一个 HEAD 请求，否则响应的实
 *          体中应当包含指向新的 URI 的超链接及简短说明。因为部分浏览器不能识别
 *          307 响应，因此需要添加上述必要信息以便用户能够理解并向新的 URI 发出
 *          访问请求。如果这不是一个 GET 或者 HEAD 请求，那么浏览器禁止自动进行
 *          重定向，除非得到用户的确认，因为请求的条件可能因此发生变化。
 *     400: 1、语义有误，当前请求无法被服务器理解。除非进行修改，否则客户端不应
 *          该重复提交这个请求。
 *          2、请求参数有误。
 *     401: 当前请求需要用户验证。该响应必须包含一个适用于被请求资源的
 *          WWW-Authenticate 信息头用以询问用户信息。客户端可以重复提交一个包含
 *          恰当的 Authorization 头信息的请求。如果当前请求已经包含了
 *          Authorization 证书，那么 401 响应代表着服务器验证已经拒绝了那些证书。
 *          如果 401 响应包含了与前一个响应相同的身份验证询问，且浏览器已经至少
 *          尝试了一次验证，那么浏览器应当向用户展示响应中包含的实体信息，因为这
 *          个实体信息中可能包含了相关诊断信息(RFC 2617)。
 *     402: 该状态码是为了将来可能的需求而预留的。
 *     403: 服务器已经理解请求，但是拒绝执行它。与 401 响应不同的是，身份验证并
 *          不能提供任何帮助，而且这个请求也不应该被重复提交。如果这不是一个
 *          HEAD 请求，而且服务器希望能够讲清楚为何请求不能被执行，那么就应该在
 *          实体内描述拒绝的原因。当然服务器也可以返回一个 404 响应，假如它不希
 *          望让客户端获得任何信息。
 *     404: 请求失败，请求所希望得到的资源未被在服务器上发现。没有信息能够告诉用
 *          户这个状况到底是暂时的还是永久的。假如服务器知道情况的话，应当使用
 *          410 状态码来告知旧资源因为某些内部的配置机制问题，已经永久的不可用，
 *          而且没有任何可以跳转的地址。404 这个状态码被广泛应用于当服务器不想揭
 *          示到底为何请求被拒绝或者没有其他适合的响应可用的情况下。
 *     405: 请求行中指定的请求方法不能被用于请求相应的资源。该响应必须返回一个
 *          Allow 头信息用以表示出当前资源能够接受的请求方法的列表。鉴于 PUT，
 *          DELETE 方法会对服务器上的资源进行写操作，因而绝大部分的网页服务器都
 *          不支持或者在默认配置下不允许上述请求方法，对于此类请求均会返回 405
 *          错误。
 *     406: 请求的资源的内容特性无法满足请求头中的条件，因而无法生成响应实体。除
 *          非这是一个 HEAD 请求，否则该响应就应当返回一个包含可以让用户或者浏览
 *          器从中选择最合适的实体特性以及地址列表的实体。实体的格式由
 *          Content-Type 头中定义的媒体类型决定。浏览器可以根据格式及自身能力自
 *          行作出最佳选择。但是，规范中并没有定义任何作出此类自动选择的标准。
 *     407: 与 401 响应类似，只不过客户端必须在代理服务器上进行身份验证。代理服
 *          务器必须返回一个 Proxy-Authenticate 用以进行身份询问。客户端可以返回
 *          一个 Proxy-Authorization 信息头用以验证(RFC 2617)。
 *     408: 请求超时。客户端没有在服务器预备等待的时间内完成一个请求的发送。客户
 *          端可以随时再次提交这一请求而无需进行任何更改。
 *     409: 由于和被请求的资源的当前状态之间存在冲突，请求无法完成。这个代码只允
 *          许用在这样的情况下才能被使用：用户被认为能够解决冲突，并且会重新提交
 *          新的请求。该响应应当包含足够的信息以便用户发现冲突的源头。冲突通常发
 *          生于对 PUT 请求的处理中。例如，在采用版本检查的环境下，某次 PUT 提交
 *          的对特定资源的修改请求所附带的版本信息与之前的某个(第三方)请求向冲
 *          突，那么此时服务器就应该返回一个 409 错误，告知用户请求无法完成。此
 *          时，响应实体中很可能会包含两个冲突版本之间的差异比较，以便用户重新提
 *          交归并以后的新版本。
 *     410: 被请求的资源在服务器上已经不再可用，而且没有任何已知的转发地址。这样
 *          的状况应当被认为是永久性的。如果可能，拥有链接编辑功能的客户端应当在
 *          获得用户许可后删除所有指向这个地址的引用。如果服务器不知道或者无法确
 *          定这个状况是否是永久的，那么就应该使用 404 状态码。除非额外说明，否
 *          则这个响应是可缓存的。410 响应的目的主要是帮助网站管理员维护网站，通
 *          知用户该资源已经不再可用，并且服务器拥有者希望所有指向这个资源的远端
 *          连接也被删除。这类事件在限时、增值服务中很普遍。同样，410 响应也被用
 *          于通知客户端在当前服务器站点上，原本属于某个个人的资源已经不再可用。
 *          当然，是否需要把所有永久不可用的资源标记为"410 Gone"，以及是否需要保
 *          持此标记多长时间，完全取决于服务器拥有者。
 *     411: 服务器拒绝在没有定义 Content-Length 头的情况下接受请求。在添加了表明
 *          请求消息体长度的有效 Content-Length 头之后，客户端可以再次提交该请求。
 *     412: 服务器在验证在请求的头字段中给出先决条件时，没能满足其中的一个或多
 *          个。这个状态码允许客户端在获取资源时在请求的元信息(请求头字段数据)中
 *          设置先决条件，以此避免该请求方法被应用到其希望的内容以外的资源上。
 *     413: 服务器拒绝处理当前请求，因为该请求提交的实体数据大小超过了服务器愿意
 *          或者能够处理的范围。此种情况下，服务器可以关闭连接以免客户端继续发送
 *          此请求。如果这个状况是临时的，服务器应当返回一个 Retry-After 的响应
 *          头，以告知客户端可以在多少时间以后重新尝试。
 *     414: 请求的 URI 长度超过了服务器能够解释的长度，因此服务器拒绝对该请求提
 *          供服务。这比较少见，通常的情况包括：本应使用 POST 方法的表单提交变成
 *          了 GET 方法，导致查询字符串(Query String)过长。重定向 URI "黑洞"，
 *          例如每次重定向把旧的 URI 作为新的 URI 的一部分，导致在若干次重定向后
 *          URI 超长。客户端正在尝试利用某些服务器中存在的安全漏洞攻击服务器。这
 *          类服务器使用固定长度的缓冲读取或操作请求的 URI，当 GET 后的参数超过
 *          某个数值后，可能会产生缓冲区溢出，导致任意代码被执行。没有此类漏洞的
 *          服务器，应当返回 414 状态码。
 *     415: 对于当前请求的方法和所请求的资源，请求中提交的实体并不是服务器中所支
 *          持的格式，因此请求被拒绝。
 *     416: 如果请求中包含了 Range 请求头，并且 Range 中指定的任何数据范围都与当
 *          前资源的可用范围不重合，同时请求中又没有定义 If-Range 请求头，那么服
 *          务器就应当返回 416 状态码。假如 Range 使用的是字节范围，那么这种情况
 *          就是指请求指定的所有数据范围的首字节位置都超过了当前资源的长度。服务
 *          器也应当在返回 416 状态码的同时，包含一个 Content-Range 实体头，用以
 *          指明当前资源的长度。这个响应也被禁止使用 multipartbyteranges 作为其
 *          Content-Type。
 *     417: 在请求头 Expect 中指定的预期内容无法被服务器满足，或者这个服务器是一
 *          个代理服务器，它有明显的证据证明在当前路由的下一个节点上，Expect 的
 *          内容无法被满足。
 *     421: 从当前客户端所在的IP地址到服务器的连接数超过了服务器许可的最大范围。
 *          通常，这里的IP地址指的是从服务器上看到的客户端地址(比如用户的网关或
 *          者代理服务器地址)。在这种情况下，连接数的计算可能涉及到不止一个终端
 *          用户。
 *     422: 从当前客户端所在的IP地址到服务器的连接数超过了服务器许可的最大范围。
 *          通常，这里的IP地址指的是从服务器上看到的客户端地址(比如用户的网关或
 *          者代理服务器地址)。在这种情况下，连接数的计算可能涉及到不止一个终端
 *          用户。
 *     422: 请求格式正确，但是由于含有语义错误，无法响应(RFC 4918 WebDAV)。
 *     423: 当前资源被锁定(RFC 4918 WebDAV)。
 *     424: 由于之前的某个请求发生的错误，导致当前请求失败，
 *          例如 PROPPATCH(RFC 4918 WebDAV)
 *     425: 在WebDav Advanced Collections 草案中定义，但是未出现在
 *          《WebDAV 顺序集协议》(RFC 3658)中。
 *     426: 客户端应当切换到TLS1.0(RFC 2817)。
 *     449: 由微软扩展，代表请求应当在执行完适当的操作后进行重试。
 *     500: 服务器遇到了一个未曾预料的状况，导致了它无法完成对请求的处理。一般来
 *          说，这个问题都会在服务器的程序码出错时出现。
 *     501: 服务器不支持当前请求所需要的某个功能。当服务器无法识别请求的方法，并
 *          且无法支持其对任何资源的请求。
 *     502: 作为网关或者代理工作的服务器尝试执行请求时，从上游服务器接收到无效的
 *          响应。
 *     503: 由于临时的服务器维护或者过载，服务器当前无法处理请求。这个状况是临时
 *          的，并且将在一段时间以后恢复。如果能够预计延迟时间，那么响应中可以包
 *          含一个 Retry-After 头用以标明这个延迟时间。如果没有给出这个
 *          Retry-After 信息，那么客户端应当以处理 500 响应的方式处理它。注意：
 *          503 状态码的存在并不意味着服务器在过载的时候必须使用它。某些服务器只
 *          不过是希望拒绝客户端的连接。
 *     504: 作为网关或者代理工作的服务器尝试执行请求时，未能及时从上游服务器
 *          (URI标识出的服务器，例如HTTP、FTP、LDAP)或者辅助服务器(例如DNS)收到
 *          响应。注意：某些代理服务器在DNS查询超时时会返回 400 或者 500 错误
 *     505: 服务器不支持，或者拒绝支持在请求中使用的 HTTP 版本。这暗示着服务器不
 *          能或不愿使用与客户端相同的版本。响应中应当包含一个描述了为何版本不被
 *          支持以及服务器支持哪些协议的实体。
 *     506: 由《透明内容协商协议》(RFC 2295)扩展，代表服务器存在内部配置错误：被
 *          请求的协商变元资源被配置为在透明内容协商中使用自己，因此在一个协商处
 *          理中不是一个合适的重点。
 *     507: 服务器无法存储完成请求所必须的内容。这个状况被认为是临时
 *          的WebDAV(RFC 4918)。
 *     509: 服务器达到带宽限制。这不是一个官方的状态码，但是仍被广泛使用。
 *     510: 获取资源所需要的策略并没有没满足(RFC 2774)。
 */
#ifndef __BIFANG_HTTP_COMMON_H
#define __BIFANG_HTTP_COMMON_H

#include <memory>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <functional>
#include <strings.h>
#include <sys/types.h>

#include "util.h"

namespace bifang
{

namespace http
{

/* Request Methods */
#define HTTP_METHOD_MAP(XX)    \
  XX(0,  DELETE)               \
  XX(1,  GET)                  \
  XX(2,  HEAD)                 \
  XX(3,  POST)                 \
  XX(4,  PUT)                  \
  /* pathological */           \
  XX(5,  CONNECT)              \
  XX(6,  OPTIONS)              \
  XX(7,  TRACE)                \
  /* WebDAV */                 \
  XX(8,  COPY)                 \
  XX(9,  LOCK)                 \
  XX(10, MKCOL)                \
  XX(11, MOVE)                 \
  XX(12, PROPFIND)             \
  XX(13, PROPPATCH)            \
  XX(14, SEARCH)               \
  XX(15, UNLOCK)               \
  XX(16, BIND)                 \
  XX(17, REBIND)               \
  XX(18, UNBIND)               \
  XX(19, ACL)                  \
  /* subversion */             \
  XX(20, REPORT)               \
  XX(21, MKACTIVITY)           \
  XX(22, CHECKOUT)             \
  XX(23, MERGE)                \
  /* upnp */                   \
  XX(24, MSEARCH)              \
  XX(25, NOTIFY)               \
  XX(26, SUBSCRIBE)            \
  XX(27, UNSUBSCRIBE)          \
  /* RFC-5789 */               \
  XX(28, PATCH)                \
  XX(29, PURGE)                \
  /* CalDAV */                 \
  XX(30, MKCALENDAR)           \
  /* RFC-2068, section 19.6.1.2 */ \
  XX(31, LINK)                 \
  XX(32, UNLINK)               \
  /* icecast */                \
  XX(33, SOURCE)

/* Status Codes */
#define HTTP_STATUS_MAP(XX)                                                 \
  XX(100, CONTINUE,                        Continue)                        \
  XX(101, SWITCHING_PROTOCOLS,             Switching Protocols)             \
  XX(102, PROCESSING,                      Processing)                      \
  XX(200, OK,                              OK)                              \
  XX(201, CREATED,                         Created)                         \
  XX(202, ACCEPTED,                        Accepted)                        \
  XX(203, NON_AUTHORITATIVE_INFORMATION,   Non-Authoritative Information)   \
  XX(204, NO_CONTENT,                      No Content)                      \
  XX(205, RESET_CONTENT,                   Reset Content)                   \
  XX(206, PARTIAL_CONTENT,                 Partial Content)                 \
  XX(207, MULTI_STATUS,                    Multi-Status)                    \
  XX(208, ALREADY_REPORTED,                Already Reported)                \
  XX(226, IM_USED,                         IM Used)                         \
  XX(300, MULTIPLE_CHOICES,                Multiple Choices)                \
  XX(301, MOVED_PERMANENTLY,               Moved Permanently)               \
  XX(302, FOUND,                           Found)                           \
  XX(303, SEE_OTHER,                       See Other)                       \
  XX(304, NOT_MODIFIED,                    Not Modified)                    \
  XX(305, USE_PROXY,                       Use Proxy)                       \
  XX(307, TEMPORARY_REDIRECT,              Temporary Redirect)              \
  XX(308, PERMANENT_REDIRECT,              Permanent Redirect)              \
  XX(400, BAD_REQUEST,                     Bad Request)                     \
  XX(401, UNAUTHORIZED,                    Unauthorized)                    \
  XX(402, PAYMENT_REQUIRED,                Payment Required)                \
  XX(403, FORBIDDEN,                       Forbidden)                       \
  XX(404, NOT_FOUND,                       Not Found)                       \
  XX(405, METHOD_NOT_ALLOWED,              Method Not Allowed)              \
  XX(406, NOT_ACCEPTABLE,                  Not Acceptable)                  \
  XX(407, PROXY_AUTHENTICATION_REQUIRED,   Proxy Authentication Required)   \
  XX(408, REQUEST_TIMEOUT,                 Request Timeout)                 \
  XX(409, CONFLICT,                        Conflict)                        \
  XX(410, GONE,                            Gone)                            \
  XX(411, LENGTH_REQUIRED,                 Length Required)                 \
  XX(412, PRECONDITION_FAILED,             Precondition Failed)             \
  XX(413, PAYLOAD_TOO_LARGE,               Payload Too Large)               \
  XX(414, URI_TOO_LONG,                    URI Too Long)                    \
  XX(415, UNSUPPORTED_MEDIA_TYPE,          Unsupported Media Type)          \
  XX(416, RANGE_NOT_SATISFIABLE,           Range Not Satisfiable)           \
  XX(417, EXPECTATION_FAILED,              Expectation Failed)              \
  XX(421, MISDIRECTED_REQUEST,             Misdirected Request)             \
  XX(422, UNPROCESSABLE_ENTITY,            Unprocessable Entity)            \
  XX(423, LOCKED,                          Locked)                          \
  XX(424, FAILED_DEPENDENCY,               Failed Dependency)               \
  XX(426, UPGRADE_REQUIRED,                Upgrade Required)                \
  XX(428, PRECONDITION_REQUIRED,           Precondition Required)           \
  XX(429, TOO_MANY_REQUESTS,               Too Many Requests)               \
  XX(431, REQUEST_HEADER_FIELDS_TOO_LARGE, Request Header Fields Too Large) \
  XX(451, UNAVAILABLE_FOR_LEGAL_REASONS,   Unavailable For Legal Reasons)   \
  XX(500, INTERNAL_SERVER_ERROR,           Internal Server Error)           \
  XX(501, NOT_IMPLEMENTED,                 Not Implemented)                 \
  XX(502, BAD_GATEWAY,                     Bad Gateway)                     \
  XX(503, SERVICE_UNAVAILABLE,             Service Unavailable)             \
  XX(504, GATEWAY_TIMEOUT,                 Gateway Timeout)                 \
  XX(505, HTTP_VERSION_NOT_SUPPORTED,      HTTP Version Not Supported)      \
  XX(506, VARIANT_ALSO_NEGOTIATES,         Variant Also Negotiates)         \
  XX(507, INSUFFICIENT_STORAGE,            Insufficient Storage)            \
  XX(508, LOOP_DETECTED,                   Loop Detected)                   \
  XX(510, NOT_EXTENDED,                    Not Extended)                    \
  XX(511, NETWORK_AUTHENTICATION_REQUIRED, Network Authentication Required)

#define Transfer_URL(str, flag) \
    m_input ? bifang::StringUtil::urlDecode(str, flag) : bifang::StringUtil::urlEncode(str, flag)

/**
 * brief: 忽略大小写比较仿函数
 */
struct CaseInsensitive
{
    /**
     * brief: 忽略大小写比较字符串
     */
    bool operator()(const std::string& lhs, const std::string& rhs) const
    {
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};

/**
 * brief: HTTP方法枚举
 */
enum class HttpMethod
{
#define XX(num, name) name = num,
    HTTP_METHOD_MAP(XX)
#undef XX
    INVALID_METHOD
};

/**
 * brief: HTTP状态枚举
 */
enum class HttpStatus
{
#define XX(code, name, desc) name = code,
    HTTP_STATUS_MAP(XX)
#undef XX
};

/**
 * brief: 将字符串方法名转成HTTP方法枚举
 * param: m HTTP方法名
 * return: HTTP方法枚举
 */
HttpMethod StringToHttpMethod(const std::string& m);

/**
 * brief: 将字符串指针转换成HTTP方法枚举
 * param: m 字符串方法枚举
 * return: HTTP方法枚举
 */
HttpMethod CharsToHttpMethod(const char* m);

/**
 * brief: 将HTTP方法枚举转换成字符串方法名
 * param: method HTTP方法枚举
 * return: 字符串方法名
 */
const std::string HttpMethodToString(const HttpMethod& method);

/**
 * brief: 将HTTP状态枚举转换成字符串状态名
 * param: status HTTP状态枚举
 * return: 字符串状态名
 */
const std::string HttpStatusToString(const HttpStatus& status);


class HttpResponse;
/**
 * brief: HTTP请求结构
 */
class HttpRequest
{
public:
    typedef std::shared_ptr<HttpRequest> ptr;
    typedef std::map<std::string, std::string, CaseInsensitive> MapType;

    /**
     * brief: 构造函数
     * param: input HTTP请求方向(true表示外对内请求, false表示内对外请求)
     *        version 版本
     *        close 是否keepalive
     */
    HttpRequest(bool input = true, uint8_t version = 0x11, bool close = true);

    std::shared_ptr<HttpResponse> createResponse();

    /**
     * brief: 检查是否是长链接来初始化 m_close
     */
    void initClose();
    /**
     * brief: 初始化请求头查询参数
     */
    void initQuery(const std::string& query);
    /**
     * brief: 初始化请求体查询参数
     */
    void initContent();
    /**
     * brief: 初始化cookie参数
     */
    void initCookie();

public:
    bool isSSL()                     const { return m_ssl;        }
    HttpMethod getMethod()           const { return m_method;     }
    uint8_t getVersion()             const { return m_version;    }
    bool isClose()                   const { return m_close;      }
    bool isWebsocket()               const { return m_websocket;  }
    const std::string& getPath()     const { return m_path;       }
    const std::string& getFragment() const { return m_fragment;   }
    const std::string& getContent()  const { return m_content;    }
    std::string& getContent()              { return m_content;    }
    const MapType& getHeaderMap()    const { return m_headerMap;  }
    const MapType& getQueryMap()     const { return m_queryMap;   }
    const MapType& getContentMap()   const { return m_contentMap; }
    const MapType& getCookieMap()    const { return m_cookieMap;  }

    void setSSL(bool v)                    { m_ssl = v;           }
    void setMethod(HttpMethod v)           { m_method = v;        }
    void setVersion(uint8_t v)             { m_version = v;       }
    void setClose(bool v)                  { m_close = v;         }
    void setWebsocket(bool v)              { m_websocket = v;     }
    void setPath(const std::string& v)     { m_path = Transfer_URL(v, true); }
    void setQuery(const std::string& v)    { initQuery(v);        }
    void setFragment(const std::string& v) { m_fragment = Transfer_URL(v, false); }
    void setContent(const std::string& v)
    {
        m_content = v;
        initContent();
    }
    void setHeaderMap(const MapType& v)
    {
        m_headerMap.clear();
        for (auto& i : v)
            m_headerMap[i.first] = i.second;
        initClose();
        initCookie();
    }

    /**
     * brief: 设置HTTP请求的头部参数
     * param: key 关键字
     *        val 值
     */
    void setHeader(const std::string& key, const std::string& val)
    {
        m_headerMap[key] = val;
        if (strcasecmp(key.c_str(), "Connection") == 0)
        {
            initClose();
            return;
        }
        if (strcasecmp(key.c_str(), "cookie") == 0)
            initCookie();
    }
    /**
     * brief: 设置HTTP请求头的查询参数
     * param: key 关键字
     *        val 值
     */
    void setQuery(const std::string& key, const std::string& val)
    {
        std::string t_key = Transfer_URL(key, false);
        std::string t_val = Transfer_URL(val, false);
        m_queryMap[t_key] = t_val;
    }
    /**
     * brief: 设置HTTP请求的Cookie参数
     * param: key 关键字
     *        val 值
     */
    void setCookie(const std::string& key, const std::string& val)
    {
        m_cookieMap[key] = val;
    }

    /**
     * brief: 删除HTTP请求的头部参数
     * param: key 关键字
     */
    void delHeader(const std::string& key)
    {
        m_headerMap.erase(key);
    }
    /**
     * brief: 删除HTTP请求头的查询参数
     * param: key 关键字
     */
    void delQuery(const std::string& key)
    {
        std::string t_key = Transfer_URL(key, false);
        m_queryMap.erase(t_key);
    }
    /**
     * brief: 删除HTTP请求的Cookie参数
     * param: key 关键字
     */
    void delCookie(const std::string& key)
    {
        m_cookieMap.erase(key);
    }

    /**
     * brief: 获取HTTP请求的头部参数
     * param: key 关键字
     *        def 默认值
     * return: 如果存在则返回对应值, 否则返回默认值
     */
    std::string getHeader(const std::string& key, const std::string& def = "") const;
    /**
     * brief: 获取HTTP请求的请求参数(可能在query里也可能在content里)
     * param: key 关键字
     *        def 默认值
     * return: 如果存在则返回对应值, 否则返回默认值
     */
    std::string getParam(const std::string& key, const std::string& def = "") const;
    /**
     * brief: 获取HTTP请求的Cookie参数
     * param: key 关键字
     * param: def 默认值
     * return: 如果存在则返回对应值, 否则返回默认值
     */
    std::string getCookie(const std::string& key, const std::string& def = "") const;

    /**
     * brief: 获取HTTP请求的头部参数
     * param: key 关键字
     *        def 默认值
     * return: 如果存在且转换成功返回对应的值, 否则返回def
     */
    template<class T>
    T getHeaderAs(const std::string& key, const T& def = T())
    {
        return getParamValue(m_headerMap, key, def);
    }
    /**
     * brief: 获取HTTP请求的请求参数(可能在query里也可能在content里)
     * param: key 关键字
     *        def 默认值
     * return: 如果存在且转换成功返回对应的值, 否则返回def
     */
    template<class T>
    T getParamAs(const std::string& key, const T& def = T())
    {
        std::string t_key = Transfer_URL(key, false);
        auto ret = getParamValue(m_queryMap, t_key, def);
        if (ret != def)
            return ret;
        return getParamValue(m_contentMap, key, def);
    }
    /**
     * brief: 获取HTTP请求的Cookie参数
     * param: key 关键字
     *        def 默认值
     * return: 如果存在且转换成功返回对应的值, 否则返回def
     */
    template<class T>
    T getCookieAs(const std::string& key, const T& def = T())
    {
        return getParamValue(m_cookieMap, key, def);
    }

    std::string toString();

private:
    // HTTP请求方向(true表示外对内, false表示内对外)
    bool m_input;
    // 是否是ssl传输
    bool m_ssl;
    // HTTP方法
    HttpMethod m_method;
    // HTTP版本
    uint8_t m_version;
    // 是否自动关闭
    bool m_close;
    // 是否为websocket
    bool m_websocket;
    // 请求路径
    std::string m_path;
    // 请求fragment
    std::string m_fragment;
    // 请求消息内容
    std::string m_content;

    // 请求头部参数MAP
    MapType m_headerMap;
    // 请求头部查询参数MAP
    MapType m_queryMap;
    // 请求体查询参数MAP(m_headerMap含有(application/x-www-form-urlencoded)时才有效)
    MapType m_contentMap;
    // 请求cookie参数MAP
    MapType m_cookieMap;
};

/**
 * brief: HTTP响应结构体
 */
class HttpResponse
{
public:
    typedef std::shared_ptr<HttpResponse> ptr;
    typedef std::map<std::string, std::string, CaseInsensitive> MapType;
    typedef std::multimap<std::string, std::string, CaseInsensitive> M_MapType;

    /**
     * brief: 构造函数
     * param: input HTTP请求方向(true表示外对内响应, false表示内对外响应)
     *        version 版本
     *        close 是否自动关闭
     */
    HttpResponse(bool input = false, uint8_t version = 0x11, bool close = true);

    /**
     * brief: 检查是否是长链接来初始化 m_close
     */
    void initClose();

public:
    HttpStatus getStatus()           const { return m_status;     }
    uint8_t getVersion()             const { return m_version;    }
    bool isClose()                   const { return m_close;      }
    bool isWebsocket()               const { return m_websocket;  }
    const std::string& getContent()  const { return m_content;    }
    std::string& getContent()              { return m_content;    }
    const std::string& getReason()   const { return m_reason;     }
    const MapType& getHeaderMap()    const { return m_headerMap;  }
    const std::string& getFilename() const { return m_filename;   }
    int64_t getStart()               const { return m_start;      }
    int64_t getLength()              const { return m_length;     }
    size_t getRate()                 const { return m_rate;       }
    size_t getRateAfter()            const { return m_rate_after; }
    const std::vector<std::string>& getCookies() const { return m_cookies; }

    void setStatus(HttpStatus v)         { m_status = v;    }
    void setVersion(uint8_t v)           { m_version = v;   }
    void setClose(bool v)                { m_close = v;     }
    void setWebsocket(bool v)            { m_websocket = v; }
    void setReason(const std::string& v) { m_reason = v;    }
    void setGzip(bool v1, uint64_t v2, int v3)
    {
        m_gzip = v1;
        m_gzip_min_length = v2;
        m_gzip_comp_level = v3;
    }
    void setRate(size_t v1, size_t v2 = 0)
    {
        m_rate = v1;
        m_rate_after = v2;
    }
    void setHeaderMap(const M_MapType& v)
    {
        m_headerMap.clear();
        m_cookies.clear();
        for (auto& i : v)
        {
            if (!strcasecmp(i.first.c_str(), "Set-Cookie"))
            {
                m_cookies.push_back(i.second);
                continue;
            }
            m_headerMap[i.first] = i.second;
        }
        initClose();
    }

    /**
     * brief: 设置响应头部参数
     * param: key 关键字
     *        val 值
     */
    void setHeader(const std::string& key, const std::string& val)
    {
        if (strcasecmp(key.c_str(), "Set-Cookie") == 0)
        {
            m_cookies.push_back(val);
            return;
        }
        m_headerMap[key] = val;
        if (strcasecmp(key.c_str(), "Connection") == 0)
            initClose();
    }
    /**
     * brief: 获取响应头部参数
     * param: key 关键字
     *        def 默认值
     * return: 如果存在返回对应值, 否则返回def
     */
    std::string getHeader(const std::string& key, const std::string& def = "") const
    {
        auto it = m_headerMap.find(key);
        return it == m_headerMap.end() ? def : it->second;
    }
    /**
     * brief: 删除响应头部参数
     * param: key 关键字
     */
    void delHeader(const std::string& key)
    {
        m_headerMap.erase(key);
    }
    /**
     * brief: 获取响应的头部参数
     * param: key 关键字
     *        def 默认值
     * return: 如果存在且转换成功返回对应的值, 否则返回def
     */
    template<class T>
    T getHeaderAs(const std::string& key, const T& def = T())
    {
        return getParamValue(m_headerMap, key, def);
    }

    void setCookie(const std::string& key, const std::string& val,
             time_t expired = 0, const std::string& domain = "",
             const std::string& path = "", bool secure = false);

    /**
     * brief: 添加响应头Content-Type参数
     * param: filename 文件名(完整路径)
     * details: 当文件无后缀名时, content-type设置为application/octet-stream, 提示浏览器下载该文件(无后缀名一般无法预览)
     *          当文件后缀名不在查找列表中时, content-type设置为text/plain, 让浏览器自行判断文件可否预览
     *          其余情况按查找列表数值填写进去即可
     */
    void setConetentType(const std::string& filename);

    /**
     * brief: 添加响应头Last-Modified参数, 并检查其与请求头的If-Modified-Since是否相同
     * param: filename 文件名(完整路径, 若为空则使用当前时间作为最后修改时间)
     *        request http请求(可以为空)
     *        def 默认值(不为空的话不检测文件名, 直接填此值进去)
     * return: true  - 无需重新发送报文体(客户端有缓存)
     *         false - 需发送报文体
     */
    bool setLastModified(const std::string& filename, HttpRequest::ptr request,
             const std::string& def = "");

    /**
     * brief: 添加响应头ETag参数
     * param: filename 文件名(完整路径)
     *        request http请求(不能为空)
     * return: true  - 无需重新发送报文体(客户端有缓存)
     *         false - 需发送报文体
     */
    bool setETag(const std::string& filename, HttpRequest::ptr request);

    /**
     * brief: 设置响应头Location
     * param: path 重定向地址
     */
    void setLocation(HttpRequest::ptr request, const std::string& path);

    /**
     * brief: 正常设置报文体内容
     * param: content 报文体内容
     */
    void setContent(const std::string& content)
    {
        m_content = content;
    }
    /**
     * brief: 对大文件进行一系列处理以实现分段发送数据(小文件原样发送, 分段之后的数据不再进行gzip压缩)
     * param: filename 文件名(完整路径)
     *        request http请求(不能为空)
     */
    void setContent(const std::string& filename, HttpRequest::ptr request);

    std::string toString();

private:
    void set_403_content();
    void set_404_content();

private:
    // HTTP响应方向(true表示外对内, false表示内对外)
    bool m_input;
    // 响应状态
    HttpStatus m_status;
    // 版本
    uint8_t m_version;
    // 是否自动关闭
    bool m_close;
    // 是否为websocket
    bool m_websocket;
    // gzip参数
    bool m_gzip;
    uint64_t m_gzip_min_length;
    int m_gzip_comp_level;
    // 大文件相关
    std::string m_filename;
    int64_t m_start;
    int64_t m_length;
    // 限速相关(单位: kb/s)
    size_t m_rate;
    size_t m_rate_after;
    // 响应消息内容
    std::string m_content;
    // 响应原因
    std::string m_reason;
    // 响应头部MAP
    MapType m_headerMap;
    // 响应Cookies列表
    std::vector<std::string> m_cookies;
};


/**
 * brief: 流式输出HttpRequest
 * param: os 输出流
 *        req HTTP请求智能指针
 * return: 输出流
 */
std::ostream& operator<<(std::ostream& os, const HttpRequest::ptr& req);

/**
 * brief: 流式输出HttpResponse
 * param: os 输出流
 * param: rsp HTTP响应智能指针
 * return: 输出流
 */
std::ostream& operator<<(std::ostream& os, const HttpResponse::ptr& rsp);

typedef void (*element_cb)(void* data, const char* at, size_t length);
typedef void (*field_cb)(void* data, const char* field, size_t flen, const char* value, size_t vlen);

}

}

#endif /*__BIFANG_HTTP_COMMON_H*/
