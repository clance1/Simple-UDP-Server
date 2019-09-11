/* udpclient.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "pg1lib.h"

#DEFINE SERVER_PORT 41026;

CLIENT_USAGE[] = \
  "Usage: student0[0/1/2/6].cse.nd.edu 41026 [filename.txt or \"message\"]";



int main(int argc, char* argv[]) {

  struct sockaddr_in addr;

  // Get command line arguments
  char* hostname = argv[1];
  int port = atoi(argv[2]);
  char* message = argv[3];

  addr.sin_family = AF_INET;
  addr.

  // Run socket
  int socket_fd = -1;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "ERROR: Socket error - %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  // Bind to the Server
  if (bind(sockfd) < 0) {
    fprintf(stderr, "ERROR: Binding error - %s\n", strerror(errno));
    close(sockfd);
    return EXIT_FAILURE;
  }

  // Get public key
  char* cpub = getPubKey();

  // Sends the public key to the Server


  // Decrypt the public key of the Server

  // Encrypt the message with the publiv key of the Server

  // Send the checksum to the server

  // Recieve the checksum and check it
  return EXIT_SUCCESS;
}
