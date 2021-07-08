# ch4 基本套接字编程  

1. socket()
```c
 int socket(int family, int type, int protocol)
 ```
    为了执行网络I/O 一个进程要做的第一件事就是调用socket函数  
    socket标识了通信的协议类型（tcp/udp, ipv4/ipv6, *字节流协议*)
    1. 并非所有的协议组合有效
    2. tcp是一个字节流协议，仅支持SOCK_STREAM协议
        Linux支持SOCK_PACKET用于对数据链路访问
2. PF_代表协议族，AF_代表地址族
    *曾尝试将他们合并但许多现存代码将崩溃*
3. connect()
```c
int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen)
``` 
    1. 调用connect前不必bind，内核会确定自身的ip以及临时端口
    2. 若是TCP套接字，调用connect会激发三次握手
    3. connect会返回各种错误
        1. ETIMEDOUT 发送SYN后无回应
        2. ECONNREFUSED 硬错误，对应端口上没有进程在监听
        3. ENETUNREACH EHOSTUNREACH 软错误 icmp
4. bind()
```c
int bind(int sockfd, const struct sockaddr* myaddr, socklen_t addrlen)
```
    将一个地址赋予一个socket
    1. 对于服务器来说，一般需要绑定一个众所周知端口
    2. tcp客户通常不把IP地址捆绑到它的套接字上，内核会为其选定一个ip地址*通配地址(INADDR_ANY = 0)* 
    *3. bind返回的一个常见错误是EADDRINUSE(address already in use)*

5. listen()
```c
int listen(int sockfd, int backlog)
```
    1. socket创建了一个套接字后被假设为一个主动套接字(调用connect的客户)，listen将未连接的套接字转换成一个被动套接字，指示内核应接受指向该套接字的连接请求
        *使套接字从CLOSED转换成LISTEN状态*
    2. 内核为LISTEN套接字维护两个队列
        1. 未完成连接队列(incomplete connection queue)，客户到达服务器，服务器等待完成相应的TCP三路握手，socket处于SYN_RCVD，每一次握手（3次之前）会在未完成队列中保留该项
        2. 已完成连接队列(completed connection queue)，已完成三路握手的客户对应其中一项，这些套接字处于ESTABLISHED状态
    3. *backlog值在大多数例中都设置为5是因为这是4.2BSD支持的最大值*
    4. *TCP中的RST（reset the connection 拒绝非法的数据请求）标志*
        1. 一个客户SYN到达时，若队列满则忽略，不发送RST，理由是：
            a. 客户进程会立即处理这个错误而不是重传
            b. 无法区分是“端口无服务器在监听”还是“有服务器监听，不过队列满了”
6. accept()
```c
int accept(int sockfd, struct sockaddr *cliaddr, socklen_t *adddrlen)
```
    1. accept由TCP服务器调用，若已完成连接队列为空则进程投入睡眠
    2. 返回值是由内核自动生成的一个客户套接字(client_socket)
    3. *一个服务器通常仅仅创建一个监听套接字（accpet的第一个参数sockfd）
