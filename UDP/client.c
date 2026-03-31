#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

ssize_t readn(int fd, void *ptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    nleft = n;
    while (nleft > 0) 
    {
        if ((nread = read(fd, ptr, nleft)) < 0) 
        {
            if (nleft == n)
                return(-1); /* error, return -1 */
            else
                break; /* error, return amount read so far */
        } 
        else if (nread == 0) 
        {
            break; /* EOF */
        }
        nleft -= nread;
        ptr += nread;
    }
    return(n - nleft); /* return >= 0 */
}



int main()
{
    int n_read = 0;
    socklen_t len;
    int sockfd = -1;

    struct sockaddr_in server_address;
    char buffer[1024];
    char message[1024] = "I am client";

    memset(&server_address, 0, sizeof(server_address));
    memset(buffer, 0, sizeof(buffer));
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(8888);

    len = sizeof(server_address);
        

    sendto(sockfd, message, sizeof(message), 0, (struct sockaddr*)&server_address, len);


    FILE *received_file = fopen("client_txt.txt", "wb");
    while((n_read = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_address, &len)) != 0)
    {
    printf("So byte da nhan them: %d \n", n_read);
    fwrite(buffer, 1, n_read, received_file);
    }
    fclose(received_file);
    close(sockfd);
        
        
}