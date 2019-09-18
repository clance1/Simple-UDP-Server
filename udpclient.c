/* udpclient.c */

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

const char CLIENT_USAGE[] = \
  "Usage: ./udpclient student0[0/1/2/6].cse.nd.edu 41026 [filename.txt or \"message\"]";

int main(int argc, char* argv[]) {

  struct sockaddr_in addr;
  struct stat s;

  // Get command line arguments
  char* hostname = argv[1];
  int port = atoi(argv[2]);
  char* message = argv[3];
  bool isFile = false;

  char* file_end = ".txt";

  if (strstr(message, file_end) != NULL) {
    isFile = true;
    if ((int fd = open(message, O_RDONLY) < 0) {
      fprintf(stderr, "ERROR: Opening message - %s\n", strerror(errno));
      close(fd);
      return EXIT_FAILURE;
    }
  }

  addr.sin_family = AF_INET;
  addr.

  // Run socket
  int socket_fd = -1;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "ERROR: Socket error - %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  // Get public key
  char cpub[BIFSIZ] = getPubKey();

  // Sends the public key to the Server
  if ((int s = sendto(sockfd, cpub, BUFSIZ)) < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Recieve and decrypt the public key of the Server
  char spub[BUFSIZ] = "";
  if ((int t = recvfrom(sockfd, *spub, BUFSIZ)) == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  char spub[BUFSIZ] = decrypt(spub);

  // Encrypt the message with the public key of the Server
  if (isFile) {
    char emess[BUFSIZ] = encrypt(fd, cpub);
  }
  else {
    char emess[BUFSIZ] = encrypt(message, spub);
  }
  if ((int s = sendto(sockfd, emess, BUFSIZ)) < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  // Send the checksum to the server
  unsigned long csum = checksum(message);
  if ((int s = sendto(sockfd, csum, BUFSIZ)) < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  // Recieve the checksum and check it
  return EXIT_SUCCESS;
}
