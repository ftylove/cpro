#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAXLINE 1024

void handle(int connfd);

int main(int argc, char *argv[])
{
    char * servInetAddr = "127.0.0.1";
    int servPort = 6888;
    char buf[MAXLINE];
    int connfd;
    struct sockaddr_in servaddr;
    
    if (argc == 2) {
        servInetAddr = argv[1];
    }
    
    if (argc == 3) {
        servInetAddr = argv[1];
        servPort = atoi(argv[2]);
    }
    
    if (argc > 3) {
        printf("Usage: echoclient <IPaddress> <Port>\n");
        return -1;
    }
    
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(servPort);
    inet_pton(AF_INET, servInetAddr, &servaddr.sin_addr);
    
    if (connect(connfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
        return -1;
    }
    printf("welcome to echoclient\n");
    handle(connfd);
    close(connfd);
    printf("exit\n");
    exit(0);
}

void handle(int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];
    int n;
    for ( ; ; ) {
        if (fgets(sendline, MAXLINE, stdin) == NULL) {
            break;
        }
        /* 也可以不用标准库的缓冲流，直接使用系统函数无缓存操作 */
	/*
        if (read(STDIN_FILENO, sendline, MAXLINE) == 0) {
            break;
        }
        */
        
        n = write(sockfd, sendline, strlen(sendline));
        n = read(sockfd, recvline, MAXLINE);
        if (n == 0) {
            printf("echoclinet: server terminated prematurely\n");
            break;
        }
        write(STDOUT_FILENO, recvline, n);
    }
}
