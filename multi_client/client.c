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

enum{
    REQUEST = 0,
    LISTEN = 1
};
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

    struct sockaddr_in server_address;
    char buffer[1024];

    memset(&server_address, 0, sizeof(server_address));
    memset(buffer, 0, sizeof(buffer));
    
    
    while(1)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
        server_address.sin_port = htons(8888);
        int connect_server = 0;
        printf("Go 1 de connect server\n");
        scanf("%d",&connect_server);
        if (connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) == 0 && connect_server == 1)
        {
            int option;
            printf("Go 0 de lay listfile go 1 de lay file \n");
            scanf("%d",&option);
            if (option == 0)
            {
                char request[] = "LIST";
                writen(sockfd, request, strlen(request));
                ssize_t n;
                while((n = read(sockfd, buffer, sizeof(buffer) - 1)) > 0)
                {
                    printf("Da nhan %d byte...\n", n);
                    n_read += n;
                }
                buffer[n_read] = '\0';
                printf("Danh sach file: %s \n", buffer);
                memset(buffer, 0, sizeof(buffer));
            }
            else
            {
                char request[1024];
                char namefile[50];
                char pathfile[100];
                printf("Nhap ten file \n");
                scanf("%s",namefile);

                snprintf(request, sizeof(request), "GET %s", namefile);
                snprintf(pathfile, sizeof(pathfile), "./client_download/%s",namefile);
                writen(sockfd, request, strlen(request));
                ssize_t n;
                FILE *received_file = fopen(pathfile, "wb");
                while((n = read(sockfd, buffer, sizeof(buffer) - 1)) > 0)
                {
                    fwrite(buffer, 1, n, received_file);
                    printf("Đã nhận %d byte...\n", n);
                    n_read += n;
                }
                fclose(received_file);
                memset(buffer, 0, sizeof(buffer)); 
            }   
            close(sockfd);
        }
    }
    
} 
