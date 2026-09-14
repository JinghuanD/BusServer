/* Jinghusn Ding */
/* Edmonton Bus Client 
    Mar 28 2026*/
    
#include <stdio.h>
#include <unistd.h>	
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>

#define MAXLINE 1024

int main(int argc, char *argv[]){

    socklen_t len;
    struct sockaddr_in servaddr;
    char recvMsg[MAXLINE];
    
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <port> <ask|reserve|quit>\n", argv[0]);
        return -1;
    }

    //Ask/Reserve/Quit
    if (strcmp(argv[3], "ask") != 0 && strcmp(argv[3], "reserve") != 0&& strcmp(argv[3], "quit") != 0) {
        fprintf(stderr, "Command must be 'ask' or 'reserve' or 'quit'\n");
        return -1;
    }

    //Create the client socket
    int cli_fd = socket(AF_INET, SOCK_STREAM, 0);
    len = sizeof(servaddr);

    //Initialize the socket from the command line
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons((uint16_t)atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid IP address: %s\n", argv[1]);
        return -1;
    }

    int ret = connect(cli_fd, (struct sockaddr*) &servaddr,len);
    if(ret < 0){

        perror("Server Port Connect Error!\n");
        return -1;
    }
    else{

        if (send(cli_fd, argv[3], strlen(argv[3]), 0) < 0) {
        perror("Send Error");
        close(cli_fd);
        return -1;
    }

    // Clear the receive buffer before get new messages
    memset(recvMsg, 0, sizeof(recvMsg));
    ret = recv(cli_fd, recvMsg, sizeof(recvMsg) - 1, 0);
    if (ret < 0) {
        perror("Receive Error!");
        close(cli_fd);
        return -1;
    }
    recvMsg[ret] = '\0';
    printf("%s\n", recvMsg);   
    } 

    //Close the client socket
    close(cli_fd);
    return 0;

}