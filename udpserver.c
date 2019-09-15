/* udpserver.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "pg1lib.h"

#DEFINE SERVER_PORT 41026;

const char SERVER_USAGE[] = \
  "Usage: ./udpserver 41026\n";

int main(int argc, char* argv[]) {

  struct sockaddr_in addr;
  struct stat s;

  // Get command line arguments
  int port = atoi(argv[1]);

  // Run socket
  int socket_fd = -1;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "ERROR: Socket error - %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Bind to the Client
  if (bind(sockfd) < 0) {
    fprintf(stderr, "ERROR: Binding error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }


  // Recieve public key from client
  char cpub[BUFSIZ] = "";
  if ((int t = recvfrom(sockfd, *cpub, BUFSIZ)) == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Get public key
  char spub[BIFSIZ] = getPubKey();

  // Encrypt and send the public key of the Server
  char epub[BUFSIZ] = encrypt(spub, cpub);
  if ((int s = sendto(sockfd, epub, BUFSIZ)) < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }



  return EXIT_SUCCESS;
}
