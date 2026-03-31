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
#include<dirent.h>
#include<poll.h>

#define MAX_CLIENT 1024


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
char* get_filelist(const char *path)
{
    struct dirent *entry;
    DIR *dp = opendir(path);
    char *list = (char*)malloc(1024); 
    memset(list, 0, 1024);

    while ((entry = readdir(dp)))
    {
        strcat(list, entry->d_name);
        strcat(list, "\n");
    }
    closedir(dp);
    return list;
}

char* readfile (const char *filename, int *out_fsize)
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
    int i;
    char *list;
    char *buffer;
    int fsize;
    int socklisten_fd = -1;
    int sockconnect_fd = -1;

    struct pollfd file_descriptions[MAX_CLIENT];
    memset(file_descriptions, 0, sizeof(file_descriptions));
    int number_fd = 1;
    



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


    file_descriptions[0].fd = socklisten_fd;
    file_descriptions[0].events = POLLIN;
    for(i = 1; i < MAX_CLIENT; i++)
    {
        file_descriptions[i].fd = -1;
    }
    while(1)
    {
        int ret = poll(file_descriptions, number_fd, -1);
        if (ret < 0) { perror("poll error"); break; }

        if (file_descriptions[0].revents & POLLIN)
        {
            len = sizeof(client_addr);
            if ((sockconnect_fd = accept(socklisten_fd, (struct sockaddr*)&client_addr, &len)) >= 0)
            {
                char *back_to_str = inet_ntoa(client_addr.sin_addr);
                printf("Dia chi IP client: %s \n", back_to_str);
                printf("Port IP client: %d \n", ntohs(client_addr.sin_port));
            }
            for (i = 1; i < MAX_CLIENT; i++)
            {
                if(file_descriptions[i].fd < 0)
                {
                    file_descriptions[i].fd = sockconnect_fd;
                    file_descriptions[i].events = POLLIN;
                    if (i >= number_fd) number_fd += 1;
                    break;
                }
            }
        }
        
        for (i = 1; i <number_fd; i++)
        {
            if (file_descriptions[i].fd < 0) break;
            if (file_descriptions[i].revents & POLLIN)
            {
                char request[1024];
                ssize_t n = read(file_descriptions[i].fd, request, sizeof(request) - 1);
                if (n > 0)
                {
                    request[n] = '\0';
                    printf("Da nhan request %s tu client\n", request);

                    if (strcmp(request, "LIST") == 0)
                    {
                        printf("gui danh sach file \n");
                        list = get_filelist("./server_listfile");
                        writen(file_descriptions[i].fd, list, strlen(list));    
                        free(list);
                    }
                    else
                    {
                        if (strncmp(request,"GET ",4) == 0)
                        {
                            printf("gui file \n");
                            char pathfile[1024];
                            snprintf(pathfile, sizeof(pathfile), "./server_listfile/%s", request + 4);
                            buffer = readfile(pathfile, &fsize);
                            if (buffer)
                            {
                                int n = writen(file_descriptions[i].fd, buffer, fsize);
                                printf("So byte da gui: %d \n", n);
                                free(buffer);
                            } 
                        }
                    }
                    close(file_descriptions[i].fd);
                    file_descriptions[i].fd = -1;
                }       
            }
        }
        
        
        
        
    } 
    close(socklisten_fd);
    return 0;
}
