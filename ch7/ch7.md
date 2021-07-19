## 套接字选项

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