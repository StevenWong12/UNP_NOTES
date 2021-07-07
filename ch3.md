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

   

