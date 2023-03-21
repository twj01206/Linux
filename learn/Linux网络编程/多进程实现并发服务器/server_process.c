#include<stdio.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
int main(){

    //1.创建socket
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }
    
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(9999);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    //绑定
    int ret = bind(lfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(ret == -1){
        perror("bind");
        exit(0);
    }

    //监听
    ret = listen(lfd,128);
    if(ret == -1){
        perror("listen");
        exit(-1);
    }

    //不断循环等待客户端的连接
    while(1){
        struct sockaddr_in clientaddr;
        int len = sizeof(clientaddr);
        //接受连接
        int cfd = accpet(lfd,(struct sockaddr*)&clientaddr,len);
        if(cfd == -1){
            perror("accpet");
            exit(-1);
        }

        //每一个连接进来，创建一个子进程跟客户端通信
        pid_t pid=fork();
        if(pid == 0){
            //子进程
            //获取客户端信息
            char clientIP[16];
            inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,clientIP,sizeof(clientIP));
            unsigned short clientPort = ntohs(clientaddr.sin_port);
            
            printf("client ip is : %s,port is %d\n",clientIP,clientPort);

            //处里 接受客户端发来的数据
            char recBuf[1024]={0};
            while (1)
            {
                int len = read(cfd,&recBuf,sizeof(recBuf));

                if(len == -1){
                    perror("read");
                    exit(-1);
                }else if(len > 0){
                    printf("recv client data : %s\n",recBuf);
                }else {
                    printf("client closed...");
                }
                
                write(cfd,recBuf,strlen(recBuf));
            }
            close(cfd);
            exit(0);//退出
        }
    }
    close(lfd);
    return 0;
}