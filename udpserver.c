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

int main(int argc, char* argv[]) {

  struct sockaddr_in saddr, caddr;

  // Get command line arguments
  int port = atoi(argv[1]);

   saddr.sin_family = AF_INET;
   saddr.sin_addr.s_addr = INADDR_ANY;
   saddr.sin_port = htons(port);

  // Run socket
  int sockfd = -1;
  printf("Booting up server on port %d\n", port);
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "ERROR: Socket error - %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
	
  // Bind
  if (bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
    fprintf(stderr, "ERROR: Binding error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  while (1) {
  // Recieve public key from client
  char cpub[BUFSIZ] = "";
  socklen_t l = sizeof(caddr);
  printf("Waiting for public key\n");
  int t = recvfrom(sockfd, cpub, 4096, 0, (struct sockaddr *) &caddr, &l);
  if (t == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  printf("Recieved public key from client\n");

  // Get public key
  char* spub = getPubKey();

  // Encrypt and send the public key of the Server
  char* epub = encrypt(spub, cpub);
  sendto(sockfd, epub, 4096, 0, (struct sockaddr *) &caddr, l);

  // Receive the message
  char* emess = malloc(4096);
  int x = recvfrom(sockfd, emess, 4096, 0, (struct sockaddr *) &caddr, &l);
  if (x == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Decrypt message
  char *mess = decrypt(emess);
  printf("Message: %s\n", mess);

  // Receive checksum
  char* ocsum_s = malloc(4096);
  int y = recvfrom(sockfd, (char *) ocsum_s, 4096, MSG_DONTWAIT, (struct sockaddr *) &caddr, &l);
  printf("Recieved checksum: %s\n", ocsum_s);
  if (y == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  char *ptr;
  unsigned long int ocsum = strtoul(ocsum_s, &ptr, 10);

  // Generate new checksum and compare
  unsigned long csum = checksum(mess);
  printf("Generated checksum: %lu\n", ocsum);
  if (csum != ocsum) {
	  printf("Error: Wrong Checksum\n");
	  sendto(sockfd, (const char *) csum, BUFSIZ, MSG_CONFIRM, (struct sockaddr *) &caddr, sizeof(saddr));
  }	  
  else {
	  printf("Checksums match!\n");
	  char* csum_s = malloc(4096);
	  sprintf(csum_s, "%lu", csum);
	  sendto(sockfd, (const char *) csum_s, BUFSIZ, 0, (const struct sockaddr *) &caddr, sizeof(saddr));
  }
  }
  close(sockfd);
  return EXIT_SUCCESS;
}
