#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    int server_sockfd;
    int len;
    struct sockaddr_in my_addr; 
    struct sockaddr_in remote_addr;
    int sin_size;
    char buf[BUFSIZE];
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(8000);
    
    if ((server_sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        return 1;
    }
    
    if (bind(server_sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0) {
        perror("bind error");
        return 1;
    }
    
    sin_size = sizeof(struct sockaddr);
    printf("waiting for a packet...\n");
    
    /* recvfrom 是无连接的 */
    if ((len = recvfrom(server_sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&remote_addr, &sin_size)) < 0) {
        perror("recvform error");
        return 1;
    }
    printf("recvived packet from %s\n", inet_ntoa(remote_addr.sin_addr));
    buf[len] = '\0';
    printf("connects: %s\n", buf);
    close(server_sockfd);
    return 0;
}
