## 套接字选项

让套接字获取某些属性或者获取套接字的属性

```c
int getsockopt(int sockfd, int level, int optname, void* optval, socklen_t *optlen);

int setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t *optlen);
```

* sockfd指向一个打开的套接字fd
* level指定系统中解释选项的代码或为通用套接字代码，或为某个特定于协议的代码
  * IPPROTO_IP
  * SOL_SOCKET
  * ...
* optval是一个指向某个变量的指针，用于存放socket的选项信息
* optlen指定了optval的大小

# 通用套接字选项

通用套接字的选项和协议无关（由内核中的协议无关代码处理）

1. SO_BROADCAST

   开启或禁止进程发送广播消息的能力，在内核中进行测试，如果目的地址是广播地址且没有设置broadcast选项，则返回EACCESS错误

2. SO_DEBUG

   内核为TCP在该套接字发送和接收的所有分组保留详细跟踪信息，保存在内核的缓冲区中（*保留相应的调试信息*）

3. SO_DONTROUTE

   数据包不发往网关，而是直接发到对应的主机（？），路由守护进程（routed和gated）经常使用本选项来绕过路由表以强制将分组从特定接口送出

4. SO_ERROR

   当套接字发生错误时，将套接字上名为so_error的变量设为UNIX Exx值中的一个，称他为套接字的**待处理错误**，内核能以以下两种方式之一通知进程这个错误

   1. 如果阻塞在select上，select返回并设置读或写中一个或两个条件
   2. 如果进程使用信号驱动式IO模型，那就给进程或进程组产生一个SIGIO信号

   进程可以通过访问SO_ERROR来获取so_error的值

5. SO_KEEPALIVE

   两小时内该套接字的任一方向上都没有数据交换，TCP就对给对方发送一个probe（对照TCP的保活计时器）

   * 若对方发送了ACK，进程得不到通知，再经过无动静的2小时会再发送一个probe
   * 对方响应RST，则告诉本端对方已崩溃且重新启动，SO_ERROR被设为ECONNRESET，套接字本身被关闭
   * 对方没响应，源自Berkeley的TCP将另外发送8个探测分节，两两相隔75秒，若没有任何相应则放弃

   **如果没有对TCP探测分节的响应，则设置为ETIMEOUT，套接字被关闭**

   如果收到一个ICMP错误，则返回相应的错误，套接字也被关闭

   **一般本选项被用在服务器端，用于关闭与那些已经断电或系统崩溃的客户的连接**

6. SO_LINGER

   指定close对面向连接的协议如何操作，**可以用于指定close后要等到他的数据和FIN已被服务器主机的TCP确认后才返回而不是立即返回**

   ```c
   struct linger {
   	int l_onoff; // 0=off, nonzero=on
       int l_linger; // linger time, POSIX specifies units as seconds
   }
   ```

   * 若l_onoff=0，那么关闭本选项，l_linger的值被忽略，先前讨论的默认设置生效，即close立即返回
   * 如果l_onoff非0且l_linger为0，close后tcp将丢弃保留在缓冲区的数据，并发送一个RST给对方，避免了TIME_WAIT状态
   * 如果l_onoff非0且l_linger也非0，套接字关闭时内核将拖延一段时间，即若缓冲区还是数据，那么进程将被投入睡眠，直到
     * 所有数据都已发送完毕且被确认
     * 延迟的时间到
   * **可能遇到的问题**
     * 若linger设置时间过短（数据发送完并被确认前linger时间到）则会返回EWOULDBLOCK（该资源短暂不可用，请稍后再试）错误且缓冲区资源被丢弃

7. SO_RCVBUF和SO_SNDBUF

   是针对发送方和接收方的两个缓冲区的选项，这两个套接字允许我们改变着两个缓冲区的默认大小

   * 对于客户，SO_RCVBUF必须在调用connect之前设置（因为TCP的流量控制）
   * 对于服务器，listen前必须给监听套接字设置（同上）
   * **TCP套接字缓冲区大小至少是相应连接的MSS值的四倍（依据的是TCP快速恢复算法，即三个ACK）**
   * 客户的套接字缓冲区大小至少要是管道（TCP传输管道）的两倍，**为每一个包保留一个副本，直到受到相应的ACK**

8. SO_RCVLOWAT和SO_SNDLOWAT

   由select函数使用的低水位标记

   * rcv让select返回可读时接收缓冲区所需数据量
   * snd让select返回可写时发送缓冲区所需的可用空间

9. SO_RCVTIMEO和SO_SNDTIMEO

   允许我们给套接的接收和发送设置一个超时值，**访问他们的参数是指向timeval结构的指针**，这让我们可以用秒数和微秒数来规定超时

10. SO_REUSEADDR和SO_REUSEPORT

    1. SO_REUSEADDR启动一个监听服务器并绑定知名端口，即使以前建立的将该端口用作本地端口的连接仍存在（**可以防止因为某个地址已建立连接而失败**）
       * 比如父进程退出重启后可以防止失败
       * 比如进程直接被kill后重启进程可以防止bind失败
    2. 允许在同一个端口上启动同一个服务器的多个实例，每个实例捆绑一个不同的本地IP（**IP别名技术**），托管多个http服务器的网点（site）
    
    *补充知识：IP多宿与地址别名*
    
    * 现在的主机具备把多个IP地址赋予单个给定物理接口的能力，除第一个IP地址外的每一个IP地址都成为一个**别名**或**逻辑接口**，通常别名地址和主机地址共享一个子网地址
    
    3. 允许单个进程捆绑到同一端口多个套接字上
    
    **总结：（1）TCP服务器中，bind前设置SO_REUSEADDR; (2) 编写一个可在同一时刻在同一主机上运行多次的多播程序时，设置SO_REUSEADR**
    
    潜在的威胁：可能盗用了知名端口2049（NFS，网络文件系统），但2049不是保留的。后来的端口+地址更具体所以会发送发更具体的一方

# IPv4套接字选项

1. IP_HDRINCL选项

   本选项开启时，我们构造完整的IP首部

2. IP_OPTIONS选项

   允许我们在IPv4首部中设置IP选项（options域）

3. IP_RECVDSTADDR选项

   收到的UDP数据包目的IP由recvmsg函数作为辅助数据返回

4. IP_RECVIF选项

   UDP数据包接收接口索引由recvmsg函数作为辅助数据返回

5. IP_TOS选项

   允许为服务、类型设置字段

6. IP_TTL选项

   设置或获取分组上的TTL

# IPv6套接字选项

1. IPV6_CHECKSUM选项（影响除ICMPv6外所有分组，因为ICMP会验证并存储）

   指定用户数据中校验和所处位置的字节偏移，若改制为非负，那么内核将：

   （1）给所有外出分组计算并存储校验和

   （2）验证外来分组的校验和，丢弃校验和无效的分组

2. IPV6_DONTFRAG选项

   禁止为UDP套接字或原始套接字自动插入分片首部

3. IPV6_NEXTHOP选项

   为数据包的下一条地址指定为一个套接字地址结构

4. IPV6_PATHMTU

   只能获取，返回值为由路由MTU发现功能确定的当前MTU	

5. ...

# TCP套接字选项

1. TCP_MAXSEG选项

   允许我们获取或设置TCP连接的最大分节大小（MSS），是我们TCP可以发送给对端的最大数据量，通常由对端使用SYN分节通告的MSS

2. TCP_NODELAY选项

   禁止TCP的Nagle算法

   * Nagle算法：减少广域网上分组的数目。**要求TCP连接上最多只能有一个未被确认的小分组，在该分组的确认到达之前不能发送其他的小分组**

   1. 比如telnet，Rlogin等程序由于是每次发送一个字符（小分组），如果开启Nagle算法可能会导致回显的效率不够高
   2. 假设某个客户向服务器发送400字节请求，请求由一个4字节的请求类型和后跟的396字节请求数据构成，服务器先调用4字节的write，再执行396字节的write，**而且服务器的进程难以收到首先收到的4字节数据进行操作，所以服务器的TCP将拖延该4字节数据的ACK**，以下为解决：
      * 使用writev而不是两次write
      * 把4个字节和396个字节复制到单个缓冲区中，然后对缓冲区调用一次write
      * 设置TCP_NO_DELAY(不可取，有损于网络)





# SCTP控制（待完成）

...

# fcntl函数

fcntl(file control)函数可执行各种描述符控制操作，提供了网络编程相关的如下特性：

1. 非阻塞IO。通过使用F_SETFL命令设置O_NONBLOCK文件状态标志将套接字设置为非阻塞型
2. 信号驱动式IO。通过使用F_SETFL设置O_ASYNC状态，当套接字一旦发生变化，内核就产生一个SIGIO信号
3. F_SETOWN允许我们指定用于接收SIGIO和SIGURG信号的套接字属主（进程ID或进程组ID）

# 习题总结

1. 单位是bytes，看程序7.1.c
2. connect后没有变化（本地的应该保持一致？）
3. 服务器直接转成listen状态，套接字不会经历TIME_WAIT状态（且丢弃保留在缓冲区中的数据）
4. ？（为什么会绑定到别的端口）
   * **第一个客户调用setsockopt bind connect，如果第二个客户在第一个客户调用bind和connect之间调用bind则会返回EADDRINUSE错误，然而当第一个客户已连接到对端（connect完成）第二个客户的bind就正常工作，处理的方法就是返回EADDRINUSE时尝试多次调用bind而不是出现错误立即abort**
5. 不需要指定SO_REUSEADDR(why?之前写的测试程序也不需要)
6. 看不出来区别诶..?
7. 保留发送回来的ICMP报文？允许底层的协议debug？

11. 根据RFC1122，ACK延迟应该小于0.5s，而且在完全大小分节流上至少每隔一个分节就有一个ACK
12. 父进程大部分时间阻塞在accept上，子进程则阻塞在read上，**keepalive对监听套接字不起作用**，子进程read返回ETIMEDOUT
13. 阻塞在fgets上，select可以探测到sockfd的ETIMEOUT错误
14. select上耗时最多

​                     