#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    int server_sockfd;  //服务器端套接字
    int client_sockfd;  //客户端套接字
    int len;
    struct sockaddr_in my_addr; //服务器网络地址结构体
    struct sockaddr_in remote_addr; //客户端网络地址结构体
    int sin_size;
    char buf[BUFSIZE]; //数据传送缓冲区
    memset(&my_addr, 0, sizeof(struct sockaddr_in));//结构体初始化
    my_addr.sin_family = AF_INET;   //设置为IP通信
    my_addr.sin_addr.s_addr = INADDR_ANY;   //服务器IP地址--允许连接到所有本地地址上
    my_addr.sin_port = htons(8000); //服务器端口
    
    /* 创建服务器套接字--IPv4协议，面向连接通信，TCP协议*/
    if ((server_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("sockrt error");
        return 1;
    }
    
    /* 将套接字绑定到服务器的网络地址上*/
    if (bind(server_sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0) {
        perror("bind error");
        return 1;
    }
    
    /* 监听连接请求--监听队列长度为5*/
    listen(server_sockfd, 5);
    
    sin_size = sizeof(struct sockaddr);
    
    /* 等待客户端连接请求到达 */
    if ((client_sockfd = accept(server_sockfd, (struct sockaddr *)&remote_addr, &sin_size)) < 0) {
        perror("accept error");
        return 1;
    }
    
    printf("accept client: %s\n", inet_ntoa(remote_addr.sin_addr));
    len = send(client_sockfd, "welcome ro my server\n", 21, 0); 
    
    /* 接收客户端的数据并将起发送给客户端--recv返回接收到的字节数，send返回发送的字节数*/
    while ((len = recv(client_sockfd, buf, BUFSIZE, 0)) > 0) {
        buf[len] = '\0';
        printf("%s\n", buf);
        if (send(client_sockfd, buf, len, 0) < 0) {
            perror("write error");
            return 1;
        }
    }
    close(client_sockfd);
    close(server_sockfd);
    return 0;
}