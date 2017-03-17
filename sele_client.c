#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>


#define PORT 8888
#define MAX_LINE 2048

int max(int a, int b)
{
    return a> b ? a : b;
}

/* readline 函数 */
ssize_t readline(int fd, char *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;
    
    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ((rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n');
                break;
        } else if (rc == 0) {
            *ptr = 0;
            return (n -1);
        } else {
            return (-1);
        }
    }
        *ptr = 0;
        return (n);
}

/* 普通客户端消息处理函数 */
void str_cli(int sockfd)
{
    /* 发送和接受缓冲区 */
    char sendline[MAX_LINE], recvline[MAX_LINE];
    while (fgets(sendline, MAX_LINE, stdin) != NULL) {
        write(sockfd, sendline, strlen(sendline));
        
        bzero(&recvline, sizeof(recvline));
        if (readline(sockfd, recvline, MAX_LINE) == 0) {
            perror("server terminated prematurely");
            exit(1);
        }
        if( fputs(recvline, stdout) == EOF) {
            perror("fputs error");
            exit(1);
        }
        
        bzero(sendline, MAX_LINE);
    }
}

/* 采用select的客户端消息处理函数 */
void str_cli2(FILE* fp, int sockfd)
{
    int maxfd;
    fd_set rset;
    /* 发送和接受缓冲区 */
    char sendline[MAX_LINE], recvline[MAX_LINE];
    
    FD_ZERO(&rset);
    while(1) {
        /* 将文件描述符和套接字描述符添加到rset描述符集 */
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfd = max(fileno(fp), sockfd) + 1;
        select (maxfd, &rset, NULL, NULL, NULL);
        
        if (FD_ISSET(fileno(fp), &rset)) {
            if (fgets(sendline, MAX_LINE, fp) == NULL) {
                printf("read nothing...\n");
                close(sockfd);
                return ;
            }
            sendline[strlen(sendline) - 1] = '\0';
            write(sockfd, sendline, strlen(sendline));
        }
        
        if (FD_ISSET(sockfd, &rset)) {
            if (readline(sockfd, recvline, MAX_LINE) == 0) {
                perror("handleMsg: server terminated prematurely.\n");
                exit(1);
            }
            
            if (fputs(recvline, stdout) == EOF) {
                perror("fputs error");
                exit(1);
            }
        }
    }
}

int main(int argc, char *argv[]) 
{
    int sockfd;
    struct sockaddr_in servaddr;
    
    if (argc != 2) {
        perror("Usage: tcpcli <IPaddress>");
        exit(1);
    }
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0) {
        printf("inet_pton error for %s\n", argv[1]);
        exit(1);
    }
    
    /* 发送链接请求服务器 */
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        exit(1);
    }
    
    /* 调用普通交易处理函数 */
    str_cli(sockfd);
    /* 调用select技术的消息处理函数 */
    //str_cli2(stdin, sockfd);
    //    exit(0);
}
