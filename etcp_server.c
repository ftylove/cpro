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

const int MAX_LINE = 2048;
const int PORT = 6000;
const int BACKLOG = 10;
const int LISTENQ = 6666;
const int MAX_CONNECT = 20;


int main(int argc, char *argv[])
{
	/* 声明服务器地址和客户连接地址 */
	struct sockaddr_in servaddr, cliaddr;
	/* 声明服务器监听套接字和客户端连接套接字 */
	int listenfd, connfd;
	pid_t childpid;

	/* 声明缓冲区 */
	char buf[MAX_LINE];

	socklen_t clilen;

	/* 初始化监听套接字listenfd */
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 表明可接收任意IP
	servaddr.sin_port = htons(PORT);

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind error");
		exit(1);
	}

		if (listen(listenfd, LISTENQ) < 0) {
			perror("listen error");
			exit(1);
		}

		for ( ; ; ) {
			clilen = sizeof(cliaddr);
			if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0) {
				perror("accept error");
				exit(1);
			}

			if ((childpid = fork()) == 0) {
				close(listenfd);
				ssize_t n;
				char buff[MAX_LINE];
				while (( n = read(connfd, buff, MAX_LINE)) > 0) {
					write(connfd, buff, n);
				}
				exit(0);
			}
			close(connfd);
		}
		close(listenfd);
}
