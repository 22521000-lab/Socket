#include <sys/un.h>
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define SOCKET_PATH "/tmp/unix_socket.sock"

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
    int sockfd = -1;

    struct sockaddr_un server_address;
    char buffer[1024];

    memset(&server_address, 0, sizeof(server_address));
    memset(buffer, 0, sizeof(buffer));
    
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket error");
        return 1;
    }
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path) - 1);

    if (connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) == 0)
    {
        ssize_t n;
        FILE *received_file = fopen("client_txt.txt", "wb");
        while((n = read(sockfd, buffer, sizeof(buffer) - 1)) > 0)
        {
            fwrite(buffer, 1, n, received_file);
            printf("Đã nhận thêm %d byte...\n", n);
            n_read += n;
        }
        fclose(received_file);
        printf("So byte da nhan: %d \n", n_read);
        close(sockfd); 
    }
   

} 
