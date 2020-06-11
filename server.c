#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
/*
Based on the code from:
https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
https://kriversia.com/2017/08/simple-web-server-en-c
*/
#define PORT 8080
//Find the size of a specified file
unsigned long fsize(char* file){
    FILE * f = fopen(file, "r");
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
    fclose(f);
    return len;
}
//Find the size of a specified image
unsigned long imagesize(char* file){
    FILE * f = fopen(file, "rb");
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
    fclose(f);
    return len;
}
//Handles the requests
char* requestHandler(char request[30000]){
    char response[20000];
    //Checks if the request is empty
    if(strcmp(request, "") == 0){
        strcpy(response, "HTTP/1.1 404 Not_Found\n");
        printf("Empty Request\n");
        //Return not found response
        return strdup(response);
    }
    //Obtains the file the requests wants
    const char s[2] = " ";
    char *token;
    token = strtok(request, s);
    token = strtok(NULL, s);
    char compare[100];
    strcpy(compare, token);
    //If the file exists in this project it returns a response with the file
    if(strcmp(compare,"/index.html") == 0|| strcmp(compare,"/") == 0|| strcmp(compare,"/TC2025.html") == 0){
        int html_fd;
        unsigned long fileSize;
        //Sets the file in index.html if the request only contains "/"
        if(strcmp(compare,"/") == 0){
            html_fd = open("index.html",O_RDONLY);
            fileSize = fsize("index.html");
        }else{
            // Get the file without "/" example: "input: /index.html / actual: index.html"
            const char a[2] = "/";
            char *token2;
            token2 = strtok(compare, a);
            char dir[20];
            strcpy(dir, token2);
            //Opens the file and finds its size
            html_fd = open(dir,O_RDONLY);
            fileSize = fsize(dir);
        }
        //Transforms the size from unsigned long to char and creates a char of the size of the file
        char file[fileSize];
        char size[15];
        sprintf(size,"%lu",fileSize);
        //reads and closes the file
        read(html_fd, file, fileSize);
        close(html_fd);
        //Creates the header of the response and concatenates its file size and its content.
        char header[59] = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
        strcpy(response, header);
        strcat(response, size);
        strcat(response, "\n\n");
        strcat(response,file);
        puts(response);
    }else{//If the file does not exist, it returns the "not found" response
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
    
    //Opening port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    //Binds socket adress with its file descriptor
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    //Sets the max number of queued connections to 1000
    if (listen(server_fd, 1000) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        //Asks another socket to recieve the request
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        char buffer[30000] = {0};
        //Loads the request into a char
        valread = read( new_socket , buffer, 30000);
        printf("Request:\n");
        printf("%s\n",buffer);
        //Handles the request and generates the response
        char *response = requestHandler(buffer);
        //Sends the response
        write(new_socket , response , strlen(response));
        printf("------------------Response sent-------------------");
        //Closes the socket to accept a new request
        close(new_socket);
    }
    return 0;
}
//----------------------------------------TO BE TESTED--------------------------------------------------------
/*
else if(strcmp(compare,"/images/background2.jpg") == 0 || strcmp(compare,"/images/background1.jpg")){
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
    }*/