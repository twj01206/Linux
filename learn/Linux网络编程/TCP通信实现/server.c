// TCP 通信的服务器端

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main()
{
    // 1.创建socket(用于监听的套接字)
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        exit(-1);
    }
    // 2.绑定
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET, "10.201.0.0", saddr.sin_addr.s_addr); // IP地址的转换 主机字节序转换为网络字节序
    saddr.sin_addr.s_addr = INADDR_ANY;                      // 0.0.0.0  INADDR_ANY=0
    saddr.sin_port = htons(9999);
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    if (ret == -1)
    {
        perror(bind);
        exit(-1);
    }

    // 3.监听
    ret = listen(lfd, 8);
    if (ret == -1)
    {
        perror("listen");
        exit(-1);
    }

    // 4.接受客户端连接
    struct sockaddr_in clientaddr;
    // socklen_t len =sizeof(clientaddr);
    int len = sizeof(clientaddr);
    int cfd = accept(lfd, (struct sockaddr *)&clientaddr, &len);
    if (cfd == -1)
    {
        perror("accpet");
        exit(-1);
    }

    // 输出客户端信息  (获取过来的网络xx转换为本地xx)
    char clientIP[16];
    inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
    unsigned short clientPort = ntohs(clientaddr.sin_port);
    printf("client ip is %s,port is %d\n", clientIP, clientPort);

    // 5.通信
    // 先获取客户端数据
    while (1)
    {
        char recvBuf[1024] = {0};
        int num = read(cfd, recvBuf, sizeof(recvBuf));
        if (num == -1)
        {
            perror("read");
            exit(-1);
        }
        else if (num > 0)
        {
            printf("recv client data : %s\n", recvBuf);
        }
        else if (num == 0)
        {
            // 表示客户端断开连接  类似管道 客户端关闭 为0
            printf("client closed...");
            break;;  
        }

        char *data = "hello,i am sercer";
        // 给客户端发生数据
        write(cfd, data, strlen(data));
    }

    // 关闭文件描述符
    close(cfd);
    close(lfd);
    return 0;
}
/*套接字函数
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int socket(int domain,int type,int protocol);
   --功能：创建一个套接字
   --参数：
        - domain 协议族
            AF_INET : ipv4
            AF_INET6 : ipv6
            AF_UNIX , AF_LOCAL:本地套接字通信(进程间通信)
        -type：通信过程中使用的协议类型
            SOCK_STREAM : 流式协议
            SOCK_DGRAM  : 报式协议
        -protocol ：具体的一个协议。一般写0
            -SOCK_STREAM : 流式协议默认使用TCU
            -SOCK_DGRAM  : 报式协议默认使用UDP
        -返回值：
            -成功：返回文件描述符，操作的就是内核缓冲区
            -失败：-1

int bind(int sockfd,const struct sockaddr *addr,socklen_t addr len);//socket命名字
   --功能：绑定，将fd和本地的IP + 端口进行绑定
   --参数：

*/