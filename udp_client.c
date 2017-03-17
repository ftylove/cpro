#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    int client_sockfd;
    int len;
    struct sockaddr_in remote_addr;
    int sin_size;
    char buf[BUFSIZE];
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    remote_addr.sin_port = htons(8000);
    
    if ((client_sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        return 1;
    }
    strcpy(buf, "This is test message");
    printf("sending: '%s'\n", buf);
    sin_size = sizeof(struct sockaddr_in);
    
    if (len = sendto(client_sockfd, buf, strlen(buf), 0, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0) {
        perror("sendto error");
        return 1;
    }
    close(client_sockfd);
    return 0;
}
