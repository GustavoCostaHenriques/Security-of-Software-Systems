// To compile:
//     gcc -g -fno-stack-protector serv-challenge.c -o serv-challenge
//


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MY_PORT		9999
#define MAXBUF		1024
#define MAXCHUNKS	32
#define SIZEM		0x100-8  /* 256 - 8 */

const char *hellomessage = "\nChallenge: Get the value of flag f14g \n\0";

const char *msg_options = "\n[1] Allocate a chunk \n"
			    "[2] Free last chunk\n"
			    "[3] Set contents of last chunk\n"
			    "[4] Print contents of first chunk\n"
			    "[5] Exit and reinitialize everything\n\0";

const char *msg_malloc = "[+] Allocation done\n\0";

const char *msg_free   = "[+] Free'd chunk\n\0";

const char *msg_requinput = "[+] Enter your payload: \0";
const char *msg_memcpy    = "[+] Contents copied into chunk\n\0";

const char *msg_nope    = "[-] You can't set the contents of the first chunk...\n\0";
const char *msg_allfree = "[-] Sorry, all chunks are free ... \n\0";
const char *msg_end     = "\n[+] Bye bye!\n\0";
const char *msg_error   = "\n[-] ERROR strange request type\n";


void processRequests(int sfd) {

  const char *f14g = "SP{nottheflag_d000000000000000000d}\n\0";

  char msg[MAXBUF];

  char *flagmessage = malloc(SIZEM);
  sprintf(flagmessage, "The flag is at %p\n", f14g);

  int chunkpos = 0;
  char **strchunks[MAXCHUNKS]; // Pointer to pointer

  strchunks[0] = malloc(SIZEM);
  *strchunks[0] = flagmessage;


  // Send hello message
  send(sfd, hellomessage, strlen(hellomessage), 0); 

  send(sfd, msg_options, strlen(msg_options), 0);

  int stop = 0;
  while(!stop) {

    if (recv(sfd, msg, MAXBUF, 0)) {

      // Valid client options:
      // [1] Allocate a chunk 
      // [2] Free last chunk
      // [3] Set contents of last chunk
      // [4] Print contents of first allocated chunk 
      // [5] Exit

      switch (msg[0]) {
        case '1':

          // If we have free()'d 0, now we use the ones starting from 1
          // Note that this will cause double free if someone free()'s 0 again
          if (chunkpos < MAXCHUNKS) {
            if (chunkpos == -1) ++chunkpos; 
            strchunks[++chunkpos] = malloc(SIZEM);

            send(sfd, msg_malloc, strlen(msg_malloc), 0); 
          }
          break;

        case '2':
          if (chunkpos > -1) {
            free(strchunks[chunkpos--]);
            send(sfd, msg_free, strlen(msg_free), 0); 
            break;
          }

        case '3':
          if (chunkpos > -1) {

            if (chunkpos == 0) {
              send(sfd, msg_nope, strlen(msg_nope), 0); 
              break;
            }

            send(sfd, msg_requinput, strlen(msg_requinput), 0); 

            if (recv(sfd, msg, 8, 0) > 0)
              memcpy(strchunks[chunkpos], (char*)msg, 8);

            send(sfd, msg_memcpy, strlen(msg_memcpy), 0); 
            break;
          }

        case '4':
          if (chunkpos > -1) { // Next line will crash on bad pointers
            char *maybeflag = *strchunks[0]; 
            send(sfd, maybeflag, strlen(f14g), 0);
            break;
          } 

          // If we didn't break in 3 or 4 it means that all chunks are free 
          send(sfd, msg_allfree, strlen(msg_allfree), 0);	
          break;

        case '5':
          stop = 1;
          send(sfd, msg_end, strlen(msg_end), 0); 
          exit(0);

        default:
          send(sfd, msg_error, strlen(msg_error), 0); 

      } /* END: switch */

    } else stop = 1;  /* error in receive */

  } /* END: while */

  close(sfd);
}

int main(int argc, char *argv[]) {
  errno = 0;
  char *p;

  pid_t parent = getpid();
  pid_t child;

  int sockfd;
  int clientfd;
  int option = 1;
  int port = MY_PORT;
  struct sockaddr_in self;
  struct sockaddr_in client_addr;
  int addrlen = sizeof(client_addr);

  if (argc == 2)
    port = strtol(argv[1], &p, 10);

  printf("[+] Server is listening at port: %d\n", port);

  /*
   * Setup server communications
   */

  /* Create TCP socket */
  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    perror("Socket");
    exit(errno);
  }
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  /* Initialize address/port structure */
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port = htons(port);
  self.sin_addr.s_addr = INADDR_ANY;

  /* Assign a port number to the socket */
  if ( bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0 ) {
    perror("Bind");
    exit(errno);
  }

  /* Make it a "listening socket" */
  if ( listen(sockfd, 20) != 0 ) {
    perror("Listen");
    exit(errno);
  }

  /*
   * Main loop: accept connection and process requests 
   *            with a separate process (fork())
   */
  while (1) {
    clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);

    printf("Client %s:%d connected!\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    child = fork();
    
    if (getpid() == parent) close(clientfd);
    else                    processRequests(clientfd);

  }

  /* Clean up (should never get here!) */
  close(sockfd);
  return 0;
}

