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
//write en el socket primero y el contenido aparte image/jpg
    printf("Entered handler\n");
    char response[20000];
    const char s[2] = " ";
    char *token;
    token = strtok(request, s);
    token = strtok(NULL, s);
    char compare[100];
    strcpy(compare, token);
    printf("%s\n",compare);
    if(strcmp(compare,"/index.html") || strcmp(compare,"/") || strcmp(compare,"/TC2025.html")){
        printf("entered true\n");
        const char a[2] = "/";
        char *token2;
        token2 = strtok(compare, a);
        char dir[20];
        strcpy(dir, token2);
        int html_fd = open(dir,O_RDONLY);
        printf("open\n");
        unsigned long fileSize = fsize(dir);
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
    }else{
        printf("entered to else\n");
        strcpy(response, "HTTP/1.1 404 Not_Found\n");
        printf("copy to response to else\n");
    }
    printf("%s\n", response);
    return strdup(response);
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    /*
    int html_fd = open("TC2025.html",O_RDONLY);
    unsigned long fileSize = fsize("TC2025.html");
    char response[20000];
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
    char *hello = response;*/
    // Only this line has been changed. Everything is same.
    
    //char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    //char *hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " + fileSize + "\n\n" + file;
    
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
        //printf("passed if\n");
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        //printf("passed read\n");
        char throwable[30000];
        strcpy(throwable,buffer);
        char *hello = requestHandler(throwable);
        printf("%s\n",buffer);
        write(new_socket , hello , strlen(hello));
        printf("------------------Hello message sent-------------------");
        close(new_socket);
    }
    return 0;
}