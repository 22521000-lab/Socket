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
    int fsize;
    int socklisten_fd = -1;
    int sockconnect_fd = -1;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t len;

    memset(&server_addr, 0, sizeof(server_addr));

    socklisten_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(socklisten_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr =inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8888);

    bind(socklisten_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(socklisten_fd, 10);
    while(1)
    {
        
        len = sizeof(client_addr);
        if ((sockconnect_fd = accept(socklisten_fd, (struct sockaddr*)&client_addr, &len)) >= 0)
        {
            char *back_to_str = inet_ntoa(client_addr.sin_addr);
            printf("Dia chi IP client: %s \n", back_to_str);
        }
        buffer = readfile("Cprograming.txt", &fsize);
        if (buffer)
        {
            int n = writen(sockconnect_fd, buffer, fsize);
            printf("So byte da gui: %d \n", n);
            free(buffer);
            close(sockconnect_fd);
        }
    }
    free(buffer);
    close(socklisten_fd);
    return 0;
} 
