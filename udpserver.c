/* udpserver.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <stdbool.h>
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

  // Get command line arguments
  int port = atoi(argv[1]);

   saddr.sin_family = AF_INET;
   saddr.sin_addr.s_addr = INADDR_ANY;
   saddr.sin_port = htons(port);


  // Run socket
  int sockfd = -1;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "ERROR: Socket error - %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Bind
  if (bind(sockfd, (const struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
    fprintf(stderr, "ERROR: Binding error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Recieve public key from client
  char cpub[BUFSIZ] = "";
  int l;
  int t = recvfrom(sockfd, (char *) cpub, BUFSIZ, MSG_WAITALL, (const struct sockaddr *) &caddr, &l);
  if (t == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Get public key
  char* spub = getPubKey();

  // Encrypt and send the public key of the Server
  char* epub = encrypt(spub, cpub);
  int s = sendto(sockfd, (const char *) epub, BUFSIZ, MSG_CONFIRM, (struct sockaddr *) &caddr, l);
  if (s < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Receive the message
  char emess[BUFSIZ] = "";
  int x = recvfrom(sockfd, (const char *) emess, BUFSIZ, MSG_WAITALL, (const struct sockaddr *) &caddr, &l);
  if (x == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Decrypt message
  char *mess = decrypt(emess);

  // Receive checksum
  unsigned long ocsum = 0;
  int y = recvfrom(sockfd, (const char *) ocsum, BUFSIZ, MSG_WAITALL, (const struct sockaddr *) &caddr, &l);
  if (y == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Generate new checksum and compare
  unsigned long csum = checksum(mess);

  return EXIT_SUCCESS;
}
