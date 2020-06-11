#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>

#define PORT 8080
unsigned long fsize(char* file){
    FILE * f = fopen(file, "r");
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
    fclose(f);
    return len;
}
unsigned long imagesize(char* file){
    FILE * f = fopen(file, "rb");
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
    fclose(f);
    return len;
}
char* requestHandler(char request[30000]){
    printf("entered handler\n");
    char response[20000];
    if(strcmp(request, "") == 0){
        strcpy(response, "HTTP/1.1 404 Not_Found\n");
        printf("Empty Request\n");
        return strdup(response);
    }
    printf("Passed first if\n");
    const char s[2] = " ";
    char *token;
    printf("declaration passed\nRequest: %s", request);
    token = strtok(request, s);
    printf("Token1: %s",token);
    token = strtok(NULL, s);
    printf("Token2: %s",token);
    char compare[100];
    strcpy(compare, token);
    if(strcmp(compare,"/index.html") == 0|| strcmp(compare,"/") == 0|| strcmp(compare,"/TC2025.html") == 0){
        int html_fd;
        unsigned long fileSize;
        if(strcmp(compare,"/") == 0){
            html_fd = open("index.html",O_RDONLY);
            fileSize = fsize("index.html");
        }else{
            const char a[2] = "/";
            char *token2;
            token2 = strtok(compare, a);
            char dir[20];
            strcpy(dir, token2);
            html_fd = open(dir,O_RDONLY);
            fileSize = fsize(dir);
        }
        char file[fileSize];
        char size[15];
        sprintf(size,"%lu",fileSize);
        read(html_fd, file, fileSize);
        close(html_fd);
        char header[59] = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        strcpy(response, header);
        strcat(response, size);
        strcat(response, "\n\n");
        strcat(response,file);
        puts(response);
    }else if(strcmp(compare,"/images/background2.jpg") == 0 || strcmp(compare,"/images/background1.jpg")){
        const char a[2] = "/";
        char *token2;
        token2 = strtok(compare, a);
        char dir[40];
        strcpy(dir, token2);
        int img_fd = open(dir,O_RDONLY);
        unsigned long fileSize = imagesize(dir);
        char file[fileSize];
        char size[15];
        sprintf(size,"%lu",fileSize);
        read(img_fd, file, fileSize);
        close(img_fd);
        char header[59] = "HTTP/1.1 200 OK\nContent-Type: image/jpg\nContent-Length: ";
        strcpy(response, header);
        strcat(response, size);
        strcat(response, "\n\n");
        strcat(response,file);
        puts(response);
    }else{
        strcpy(response, "HTTP/1.1 404 Not_Found\n");
    }
    return strdup(response);
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 1000) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        char throwable[30000];
        strcpy(throwable, buffer);
        printf("Buffer\n");
        printf("%s\n",buffer);
        char *request = requestHandler(throwable);
        
        write(new_socket , request , strlen(request));
        printf("------------------Response sent-------------------");
        close(new_socket);
    }
    return 0;
}