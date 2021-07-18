##  I/O复用:select和poll函数

再ch5可以看到,被阻塞于fgets的客户端无视了服务器发过来的FIN信号(处于CLOSE_WAIT状态),**所以我们需要一种预先告知内核的能力**,使得内核一旦发现进程指定的一个或多个I/O条件就绪:

	1. 输入已准备好被读取
	2. fd已经能够承载更多输出

它便通知进程,这称为I/O复用(multiplexing)



**I/O复用被使用在以下场合**

1. 客户处理多个描述符(通常是交互式输入和网络套接字,即stdin和socket)
2. 一个客户处理多个套接字
3. 一个服务器既要处理TCP又要处理UDP
4. 一个服务器要处理多个服务或多个协议



# UNIX下5种I/O模型

1. 阻塞式IO

   即等待数据的到来,在此之前阻塞

2. 非阻塞式IO

   不断对内核进行询问(轮询,polling),内核中没有数据准备好时向进程返回一个错误,这样会消耗大量CPU时间

3. IO复用

   调用**select**和**poll**,阻塞在这两个系统调用的某一个之上而不是IO系统调用上

   即使用select或poll来请求内核的数据而不是recvfrom

   **相较于阻塞式IO,需要调用两个系统调用,但它的优势在于可以等待多个描述符就绪**

4. 信号驱动式IO(SIGIO)

   利用sigaction安装一个信号处理函数,当内核产生SIGIO后通知进程调用recvfrom来获取数据

5. 异步IO(POSIX的aio_系列函数)

   通过系统调用告诉内核启动某个操作,当操作完成(包括将数据复制到用户区)后通过aio_系列函数通知进程

   *与SIGIO不同的是异步IO告诉进程何时完成,而SIGIO告诉进程何时开始*

# select函数

允许进程指示内核等待多个事件中的任何一个发生,并只在有一个或多个事件发生或经历一段指定的时间后才唤醒他

例: 可以通过select告诉内核在下列情况发生才返回

 	1. 集合{1, 4, 5}中的任何fd准备好读;
 	2. 集合{2, 7}中的任何fd准备好写;
 	3. 集合{1, 4}中的任意fd有异常条件待处理
 	4. 经历了10.2s

```c
#include <sys/select.h>
#include <sys/time.h>

int select(int maxfdpl, fd_set *readset, fd_set *writeset, fd_set *exceptset, const struct timeval *timeout)  
```



1. 对于timeout参数:

   * 设置为空指针:仅在有一个描述符准备好IO时才返回

   * 设置一个(非0)值:在有一个描述符准备好IO时返回,但不超过timeout指向的秒数和微秒数

   * 设置为0,根本不等待,在检查描述符后立即返回,这称为轮询(polling)





**从可移植性考虑,在捕获信号时,必须做好select返回EINTR错误的准备**

2. 对于其他的参数

   select使用描述符集(file descriptor),每个set都是一个整数数组,其中**每一位**对应一个描述符, 举例来说:

   * 假设使用32位整数,数组的第一个元素对应于描述符0~31
   * 第二个元素对应于32~63
   * 以此类推

   *实现的细节都在fd_set和四个宏中*

   ```c
   void FD_ZERO(fd_set *fdset); // clear all bits in fdset
   // 在使用fdset之前一定要将其FD_ZERO(将其初始化)
   void FD_SET(int fd, fd_set *fdset); // turn on the bit for fd in fdset
   void FD_CLR(int fd, fd_set *fdset); // turn off the bit for fd in fdset
   void FD_ISSET(int fd, fd_set *fdset); // test whether the bit for fd is in fdset;
   ```

3. 描述符就绪条件

   1. 满足以下四个条件中的任何一个时,一个套接字准备好读
      * 接收缓存区数据字节数 >= 接收缓冲区低水位标记的当前大小(SO_RCVLOWAT)
      * 连接的读半部关闭(接收了FIN),这样的socket不阻塞并直接返回0
      * 是一个监听套接字(调用了listen)且已经完成的连接数不为0
      * 套接字上有错误待处理,不阻塞并返回-1, errno设置为具体的错误条件, 可以通过指定SO_ERROR选项调用getsockopt获取并清除
   2. 满足下列四个条件之一时,一个套接字准备好写
      * 套接字发送缓冲区可用空间字节数大于等于套接字发送缓冲区低水位标记的当前大小,且该套接字已连接(或不需要连接)
      * 该连接的写半部关闭,对这样的套接字写将产生SIGPIPE信号
      * 使用非阻塞式connect的套接字已建立连接
      * 其上有一个套接字错误待处理

*注意:当某个套接字上有错误时,select将其标记为既可读又可写*



4. **缓冲区大小问题（select和stdio混用的问题）**

   * stdio流函数在用户空间和内核都有缓冲，系统IO只在内核有缓冲
   * stdio流的io函数缓冲机制：面对文件使用全缓冲，面对设备使用行缓冲（屏幕、键盘行缓冲）
   * 而select函数只对内核缓冲区判断读写就绪（用于缓冲区对select是透明的）
   * stdio库函数是利用用户区来进行缓存，用户区缓存满了才将数据流入内核区
   * 系统调用的缓存区仅在内核区
   * **stdio函数在用户输出缓冲区有数据的时候不会调用内核中的数据，所以我们的范例程序（6.5.c）才会在get版本不断输出**   
   
   
   
5. shutdown函数（**用于解决批量输入输出与输入文件大小不等的问题**）

   *在使用文件进行批量的读写时，当读到EOF时如果不进行处理，则会直接返回到main中，进而结束读写，但是这时仍有数据在路上（发送或接收途中），故直接进行close是不恰当的*

   * close把fd引用计数减1，仅在fd计数为0时才关闭套接字
   * close终止读写两个方向的数据传送，shutdown可以仅仅关闭一方（*比如写完后通知对方可以继续写，但关闭了自身的写端*）

   ```c
   int shutdown(int sockfd, int howto);
   /*
   	SHUT_RD 关闭读的一端，socket接收缓冲区现有数据被丢弃，进程不能对这样的socket进行读（新接收的数据也会被丢弃）
   	SHUT_WR 关闭写的一端，被称为半关闭（half-close），留在发送缓冲中的数据会被发送掉，并后跟TCP正常连接终止序列
   	SHUT_RDWR 与调用上述两个各一次等效
   	
   */
   ```

*服务器程序可以用select来处理任意个客户的单进程程序，而不是为每个客户派生一个子程序*



**当有客户到达时，我们在client数组中的第一个可用项中记录其已连接套接字的描述符，并加入到fdset中**

# pselect 函数

是POSIX定义的一个函数

```c
int pselect(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, const struct timespec *timeout, const sigset_t *sigmask);
```

与select的不同：

1. 使用了timespec而不是timeval（**timespec第二个成员指定纳秒数，timeval第二个成员指定微秒数**）
2. 增加了第六个参数：**一个指向信号掩码的指针**，该参数允许程序禁止递交某些信号



# poll函数

poll提供的功能与select类似，不过在处理流设备时能够提供额外的信息

```c
int poll(struct pollfd *fdarray, unsigned long nfds, int timeout);

struct pollfd {
	int fd; // fd to check
    short events; // events of interest on fd
    short revents; // events that occurred on fd
};
```

1. pollfd中使用了events和revents分别代表测试的条件以及描述符的状态，一个为调用值，另一个为返回结果，从而避免了使用值-结果参数
2. 以下为events以及revents的一些常值

| 常值       | 可以作为event？ | 可以作为revent？ | 说明                     |
| ---------- | --------------- | ---------------- | ------------------------ |
| POLLIN     | √               | √                | 普通或优先带数据可读     |
| POLLRDNORM | √               | √                | 普通数据可读             |
| POLLRDBAND | √               | √                | 优先级带数据可读         |
| POLLPRI    | √               | √                | 高优先级数据可读         |
| POLLOUT    | √               | √                | 普通或优先带数据可写     |
| POLLWRNORM | √               | √                | 普通数据可写             |
| POLLWRBAND | √               | √                | 优先级带数据可写         |
| POLLER     |                 | √                | 发生错误                 |
| POLLHUP    |                 | √                | 发生挂起                 |
| POLLNVAL   |                 | √                | 描述符不是一个打开的文件 |

3. timeout指定poll返回前等待多长时间，单位是毫秒
   * =INFTIM 时永远等待
   * =0时立即返回，不阻塞进程



# 习题总结

1. fdset是一个结构体，结构体中包含这个数组，而c允许结构跨等号赋值
2. 若select告诉我们一个socket可写，那么缓冲区就有8192可用空间，但如果我们以8193长度对阻塞式socket调用write时，write会阻塞以等待一个字节的可用空间，**对阻塞式套接字的写操作将会一直阻塞到所有数据都能被内核接受为止**
3. 没有对两个fd进行轮询，忽略了两个fd均就绪的情况
4. 8192
5. 持续发送数据，tcp确认后扔掉他们
6. **shutdown会发送FIN，而close只有在调用时描述符引用计数为1时才发送FIN**
7. 服务器会直接退出，但在6-26处理了这种情况