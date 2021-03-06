#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

const int MAX_LINE = 2048;
const int PORT = 6001;
const int BACKLOG = 10;
const int LISTENQ = 6666;
const int MAX_CONNECT = 20;

void *recv_message(void *fd)
{
    int sockfd = *(int *)fd;
    while(1) {
        char buf[MAX_LINE];
        memset(buf, 0, MAX_LINE);
        int n;
        
        if ((n = recv(sockfd, buf, MAX_LINE, 0)) == -1) {
            perror("recv error.\n");
            exit(1);
        }
        buf[n] = '\0';
        /* 若收到exit，代表退出通信 */
        if (strcmp(buf, "exit") == 0) {
            printf("Server is closed.\n");
            close(sockfd);
            exit(0);
        }
        
        printf("\nServer: %s\n", buf);
    }
}

int main(int argc, char *argv[])
{
    int sockfd;
    pthread_t recv_tid, send_tid;
    struct sockaddr_in servaddr;
    
    /* 判断是否位合法输入 */
    if (argc != 2) {
        perror("Usage: tcpcli <IPaddress>");
        exit(1);
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0) {
        printf("inet_pton error for %s\n", argv[1]);
        exit(1);
    }
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0) {
        printf("Inet_pton error for %s \n", argv[1]);
        exit(1);
    }
    
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        exit(1);
    }
    
    /* 创建子程序处理该客户链接接收消息 */
    if (pthread_create(&recv_tid, NULL, recv_message, &sockfd) == -1) {
        perror("pthread create error.\n");
        exit(1);
    }
    
    /* 处理客户端发送消息 */
    char msg[MAX_LINE];
    memset(msg, 0, MAX_LINE);
    while(fgets(msg, MAX_LINE, stdin) != NULL) {
        if(strcmp(msg, "exit\n") == 0) {
            printf("byebye.\n");
            memset(msg, 0, MAX_LINE);
            strcpy(msg, "byebye.");
            send(sockfd, msg, strlen(msg), 0);
            close(sockfd);
            exit(0);
        }
        
        if (send(sockfd, msg, strlen(msg), 0) == -1) {
            perror("send error,\n");
            exit(1);
        }
    }
}
