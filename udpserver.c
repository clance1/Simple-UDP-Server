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

const char SERVER_USAGE[] = \
  "Usage: ./udpserver 41026\n";

bool comp_csum(unsigned long csum, unsigned long ocsum) {
  if (csum == ocsum)
    return true;
  else
    return false;
}

int main(int argc, char* argv[]) {

  struct sockaddr_in saddr, caddr;
  struct stat s;

  // Get command line arguments
  int port = atoi(argv[1]);

  struct addrinfo *results;
  struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE
	};

   saddr.sin_family = AF_INET;
   saddr.sin_addr.s_addr = INADDR_ANY:
   saddr.sin_port = htons(port);

   int addr = getaddrinfo(hostname, port, &hints, &results);

  // Run socket
  int socket_fd = -1;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "ERROR: Socket error - %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Bind
  if (bind(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
    fprintf(stderr, "ERROR: Binding error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Recieve public key from client
  char cpub[BUFSIZ] = "";
  int l;
  if ((int t = recvfrom(sockfd, *cpub, BUFSIZ, MSG_WAITALL, &caddr, &l)) == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Get public key
  char spub[BIFSIZ] = getPubKey();

  // Encrypt and send the public key of the Server
  char epub[BUFSIZ] = encrypt(spub, cpub);
  if ((int s = sendto(sockfd, epub, BUFSIZ, MSG_CONFIRM, &caddr, l)) < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Receive the message
  char emess[BUFSIZ] = "";
  if ((int t = recvfrom(sockfd, *emess, BUFSIZ, MSG_WAITALL, &caddr, &l)) == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Decrypt message
  char *mess = decrypt(emess);

  // Receive checksum
  unsigned long ocsum = 0;
  if ((int t = recvfrom(sockfd, *ochecksum, BUFSIZ, MSG_WAITALL, &caddr, &l)) == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Generate new checksum and compare
  unsigned long csum = checksum(mess);
  if ()

  return EXIT_SUCCESS;
}
