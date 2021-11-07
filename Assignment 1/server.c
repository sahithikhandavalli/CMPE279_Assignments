// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
	&opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_fd, (struct sockaddr *)&address,
	sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Forking a child process.\n");

    int pid = fork();
    if(pid < 0)
    {
        printf("Error!! Child process cannot be created.");
        return -1;
    }
    else if(pid == 0)
    {
        printf("Inside child process!!\n");
        const char * nobodyUser = "nobody";
        struct passwd *pwd = getpwnam(nobodyUser);
        if (pwd == NULL) {
                    printf("ERROR in finding user with name %s\n", nobodyUser);
                    return -1;
              }
        //setuid - On success, zero is returned. On error, -1 is returned, and errno is set appropriately.
        int setuid_reponse = setuid(pwd->pw_uid);
        if(setuid_reponse < 0){
                printf("ERROR in dropping privileges and error number = %d\n", setuid_reponse);
                return -1;
              }
        printf("Child process dropped privileges\n" );
        valread = read(new_socket, buffer, 1024);
        printf("Read %d bytes: %s\n", valread, buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
        return 0;
    }
    else{
         wait(NULL);
         printf("Parent Terminating..\n" );
    }

}
