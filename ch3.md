# ch 3基本套接字编程

1. 套接字函数需要一个指向*套接字地址结构的指针*作为参数  

   ```c
   struct in_addr {
       in_addr_t saddr; // a 32-bit IPv4 address
   };
   
   struct sockaddr_in {
   	uint8_t sin_len;	// len of the struct
   	sa_family_t sin_family;
       in_port_t	sin_port; // 16-bit TCP or UDP port
       
       struct in_addr sin_addr; // IPv4 address
       
       char sin_zero[8]; // unused
   };
   
   ```

   Posix 规范中只需要sin_family sin_port sin_addr三个字段

   *sockaddr_in即是套接字地址结构的指针类型*  

2. 由于void*类型在套接字函数之前定义的，所以采取了一个通用套接字地址结构

   ```c
   struct sockaddr {
       uint8_t sa_len;
       sa_family_t sa_family; // address family: AF_xxx value
       char sa_data[14]
   }
   ```

3. IPv6地址只需在4的末尾加上6

4. 从进程到内核传递套接字地址结构的函数

   1. bind()
   2. connect()
   3. sento()

5. 从内核到进程传递套接字地址结构的函数

   1. accept()
   2. recvfrom()
   3. getsockname()
   4. getpeername()

6. *地址结构大小参数是一个指针的原因：*

   1. 进程传递给内核的是一个值（value）
   2. 函数（从内核）返回给进程的是一个结果（result）
   3. 值和结果的大小不一定相同，返回值可能小于最大长度（如sockaddr_un)

7. *碎片知识: union中的数据共享同一段存储*

8. 网络字节序

   1. 网络协议必须指定一个*网络字节序*，即地址增长的方向

   2. 有函数实现他们让我们可以不必关注这些细节

      ```c
      #include <netinet/in.h>
      // n->network h->host s->short l->long
      uint16_t htons(uint16_t host16bitvalue);
      uint32_t htonl(uint32_t host32bitvalue);
      uint16_t ntohs(uint16_t net16bitvalue);
      uint32_t ntohl(uint32_t net32bitvalue);
      ```

9. *字节操纵函数*

   ```c
   #include <string.h>
   // easier to memorize than memset, set byes to 0
   // Berkeley function
   void bzero(void *dest, size_t nbytes);
   void bcopy(const void* src, void*dest, size_t nbytes);
   int bcmp(const void* ptr1, const void* ptr2, size_t nbytes);
   
   // ANSI C
   void *memset(void *dest, int c, size_t len);
   void *memcpy(void *dest, const void *src, size_t nbytes);
   int memcmp(const void* ptr1, const void* ptr2, size_t nbytes);
   
   ```

10. 地址转换函数

    在ASCII字符串和网络字节序的二进制值（直接存放在socket地质结构中）之间转换网际地址

```c
#include <arpa/inet.h>
// convert strptr to addrptr "x.x.x.x" -> x.x.x.x
int inet_aton(const char* strptr, struct in_addr *addrptr);

in_addr_t inet_addr(const char *strptr);// 被废弃，新代码用inet_aton
    
char *inet_ntoa(struct in_addr inaddr);

// 更好的方法
int inet_pton(int family, const char *strptr, void *addrptr);
// family:AF_INET/AF_INET6
const char *inet_ntop(int family, const void *addrptr, char*strptr, size_t len);
// len字段防止大小超出规定的缓存区
```



11. 对于套接字之间的IO（read和write方法）做出了封装

    ```c
    // 请求量过大时需要多次调用read（因为UNIX缓冲区有限）
    // read返回字节数为0时结束，否则将指针偏移后继续读，nleft跟踪剩余字节数
    #include "unp.h"
    ssize_t readn(int fd, void *vptr, size_t n){
        size_t nleft;
        ssize_t nread;
        char *ptr;
        ptr = vptr;
        nleft = n;
        while (nleft > 0) {
            if((nread = read(fd, ptr, nleft)) < 0){
                if (errno == EINTR)
                    nread = 0;
                else
                    return -1;
            }
            else if (nread == 0)
                	break; // EOF
            nleft -= nread;
            ptr += nread;
        }
        return (n-nleft);
    }
    
    
    // 同理，当write字节过多时不断写入缓冲区
    ssize_t writen(int fd, const void *vptr, size_t n){
    	size_t nleft;
        ssize_t nwritten;
        const char *ptr;
        
        ptr = vptr;
        nleft = n;
        while( nleft > 0 ) {
    		if((nwritten = write(fd, ptr, nleft)) <= 0){
                if(nwritten < 0 && errno == EINTR)
                    nwritten = 0;
                else
                    return -1;
            }
            nleft -= nwritten;
            ptr += nwritten;
        }
        
        return n;
    }
    
    ssize_t readline(int fd, void *vptr, size_t maxlen){
        ssize_t n, rc;
        char c, *ptr;
        ptr = vptr;
        for(n = 1; n < maxlen; n++){
        again:
            if((rc = read(fd, &c, 1)) == 1){
                *ptr++ = c;
                if(c == '\n')
                    break;
            }
            else if (rc == 0) {
                *ptr = 0;
                return n-1;
            }
            else {
                if(errno == EINTR)
                    	goto again;
                return -1
            }
        }
        *ptr = 0;
        return n;
    }
    ```

    