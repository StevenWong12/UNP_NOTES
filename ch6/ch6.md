## I/O复用:select和poll函数

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

