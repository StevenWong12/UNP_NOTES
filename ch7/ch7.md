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

