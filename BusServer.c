/* Jinghusn Ding */
/* Edmonton Bus Server 
    Mar 28 2026*/

#include <stdio.h>
#include <unistd.h>	
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAXHOSTNAME	256
#define MAXLINE 1024
#define MAXCONNECT 256
#define MAXBUS 20
//#define DEBUG

char * getIPAddress()
{
	char myname[ MAXHOSTNAME + 1 ];
	static char IPinASCII[ MAXHOSTNAME ];	/* Oversized */
	struct hostent * hp ;

	memset( myname, 0, MAXHOSTNAME + 1 );	/* Init */
	memset( IPinASCII, 0, MAXHOSTNAME );

	gethostname( myname, MAXHOSTNAME );
#ifdef DEBUG
	printf( "hostname is %s\n", myname );
#endif /* DEBUG */

	hp = gethostbyname( myname );
	if( hp == NULL )
	{
		perror( "gethostbyname" );
		return( "IP not found" );
	}

	inet_ntop( hp->h_addrtype, hp->h_addr_list[ 0 ], IPinASCII,
		MAXHOSTNAME ) ;

#ifdef DEBUG
	printf( "canonical hostname is %s at IP %s\n", hp->h_name, IPinASCII );
#endif /* DEBUG */

	return( IPinASCII );
}


/* Paul Lu */
/*
 * getPortNumber - given a valid file descriptor/socket, return the port number
 */
int getPortNumber( int socketNum )
{
	struct sockaddr_in addr;
	int rval;
	socklen_t addrLen;

	addrLen = (socklen_t)sizeof( addr );

	/* Use getsockname() to get the details about the socket */
	rval = getsockname( socketNum, (struct sockaddr*)&addr, &addrLen );
	if( rval != 0 )
		perror("getsockname() failed in getPortNumber()");

	/* Note cast and the use of ntohs() */
	return( (int) ntohs( addr.sin_port ) );
} /* getPortNumber */


int main( int argc, char * argv[] )
{
	int listenfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;
    char sendMsg[MAXLINE];
    char recvMsg[MAXLINE];
    int bus_ava_num = MAXBUS;
    
//Create the listen socket

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if(listenfd < 0){

        printf("Listening Port Set Error!\n");
        return -1;
    }

 //Bind the socket   

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(0);

    int ret = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(ret < 0){

        perror("Port Bind Error!\n");
        return -1;
    }

//Begin listening
    ret = listen(listenfd, MAXCONNECT);
    if(ret < 0){

        perror("Port Listen Error!\n");
        return -1;
    }
    printf("EDM is running on %s, listening on port %d\n", getIPAddress(),getPortNumber(listenfd));
   
//The main loop, allow one client to connect each time
    while(1){
        len = sizeof(cliaddr);
        connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&len);
        if(connfd < 0){

            perror("Client Connect Error!\n");
            continue;//Wait for the next client if this one is failed.
        }
        else{
            //Clear the receive buffer
            memset(recvMsg, 0, sizeof(recvMsg));
            ret = recv(connfd, recvMsg, sizeof(recvMsg) - 1, 0);
            if(ret <= 0){

                perror("Port Receive Error!\n");
                close(connfd);
                continue;
            }
            else{
                //Deal with the received message
                recvMsg[ret] = '\0';
                printf("Received command: %s\n", recvMsg); 
                if(strcmp(recvMsg,"quit") == 0){

                    snprintf(sendMsg, sizeof(sendMsg), "Client Exit.");
                    if(!send(connfd, sendMsg, strlen(sendMsg), 0)){

                        perror("Port Respond Quit Error!\n");

                    }
                }
                else if(strcmp(recvMsg,"ask") == 0){
                    snprintf(sendMsg, sizeof(sendMsg), "Shuttle Bus has %d seats available", bus_ava_num);
                    if(!send(connfd, sendMsg, strlen(sendMsg), 0)){

                        perror("Port Respond Ask Error!\n");
                    }
                }
                else if(strcmp(recvMsg,"reserve") == 0){

                    if(bus_ava_num > 0){
                        bus_ava_num --;
                        snprintf(sendMsg, sizeof(sendMsg), "Seat Reserved. Shuttle Bus now has %d seats available", bus_ava_num);
                        if(!send(connfd, sendMsg, strlen(sendMsg), 0)){

                        perror("Port Respond Reserve Error!\n");
                    }

                    }
                    else{

                        if(send(connfd, "Failed Reservation", 20,0) < 0){

                            perror("Port Respond Reserve Fail Error!\n");

                        }
                        
                    }
                    
                }

            }
            close(connfd);
        }
        
    }
    //Close the listen socket to shut down the server
    close(listenfd);
    return 0;
}
