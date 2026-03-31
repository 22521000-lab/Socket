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

ssize_t writen(int fd, const void *ptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    nleft = n;
    while (nleft > 0) 
    {
        if ((nwritten = write(fd, ptr, nleft)) < 0) 
        {
            if (nleft == n)
                return(-1); /* error, return -1 */
            else
                break; /* error, return amount written so far */
        } 
        else if (nwritten == 0) 
        {
            break;
        }
    nleft -= nwritten;
    ptr += nwritten;
    }
    return(n - nleft); /* return >= 0 */
}

char * readfile (const char *filename, int *out_fsize)
{
    int fsize;
    FILE *f_txt = fopen(filename, "rb"); 
    if (f_txt == NULL) {
        perror("Khong the mo file");
        return NULL;
    }
    else
    {
        /*Tinh kich thuoc*/
        fseek(f_txt, 0, SEEK_END);
        fsize = ftell(f_txt);
        fseek(f_txt, 0, SEEK_SET);
    }
    char *buffer = (char*)malloc(fsize);

    // Đọc vào buffer (đảm bảo buffer đủ lớn)
        if (buffer) 
        {
            fread(buffer, 1, fsize, f_txt);
        }
        *out_fsize = fsize;
        fclose(f_txt);
        return buffer;

}
int main()
{  
    char *buffer;
    char message[1024];
    int fsize;
    int sockfd = -1;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t len;

    memset(&server_addr, 0, sizeof(server_addr));

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr =inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8888);

    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    while(1)
    {
        
        len = sizeof(client_addr);
        recvfrom(sockfd, message, sizeof(message), 0, (struct sockaddr*)&client_addr, &len);
        char *back_to_str = inet_ntoa(client_addr.sin_addr);
        printf("Dia chi IP client: %s \n", back_to_str);
        printf("message from client: %s \n", message);

        buffer = readfile("Cprograming.txt", &fsize);
        if (buffer)
        {
            int bytes_sent = 0;
            int packet_size = 1024;
            while (bytes_sent < fsize)
            {
                int byte_remaining = fsize - bytes_sent;
                int byte_write = (byte_remaining < packet_size)? byte_remaining : packet_size;
                int n = sendto(sockfd, buffer + bytes_sent, byte_write ,0, (struct sockaddr*)&client_addr, len);
                bytes_sent += byte_write;
                printf("So byte da gui: %d \n", n);
            }
            sendto(sockfd, NULL, 0, 0, (struct sockaddr*)&client_addr, len); /*gói kết thúc*/
            
            free(buffer);
            
        }
    }
    close(sockfd);
    return 0;
} 
