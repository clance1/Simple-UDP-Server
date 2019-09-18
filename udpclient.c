/* udpclient.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "pg1lib.h"

const char CLIENT_USAGE[] = \
  "Usage: ./udpclient student0[0/1/2/6].cse.nd.edu 41026 [filename.txt or \"message\"]";

int main(int argc, char* argv[]) {

  // Get command line arguments
  char* hostname = argv[1];
  int port = atoi(argv[2]);
  char* message = argv[3];
  bool isFile = false;
  int fd;

  char* file_end = ".txt";

  struct addrinfo *results;
  struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE
		};

  struct sockaddr_in saddr;

  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY;
  saddr.sin_port = htons(port);


  int addr = getaddrinfo(hostname, port, &hints, &results);

  if (strstr(message, file_end) != NULL) {
    isFile = true;
	fd = open(message, O_RDONLY);
    if (fd < 0) {
      fprintf(stderr, "ERROR: Opening message - %s\n", strerror(errno));
      close(fd);
      return EXIT_FAILURE;
    }
  }

  // Run socket
  int sockfd = -1;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "ERROR: Socket error - %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  // Get public key
  char* cpub = getPubKey();

  // Sends the public key to the Server
  int s = sendto(sockfd, (const char *) cpub, BUFSIZ, MSG_CONFIRM, (const struct sockaddr *) &saddr, sizeof(saddr));
  if (s < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Recieve and decrypt the public key of the Server
  char spub_rec[BUFSIZ] = "";
  int l;
  int t = recvfrom(sockfd, (char *) spub_rec, BUFSIZ, MSG_CONFIRM, (struct sockaddr *) &saddr, &l);
  if (s == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  char* spub = decrypt(spub_rec);

  // Encrypt the message with the public key of the Server
  char* emess = "";
  if (isFile) {
    emess = encrypt(fd, cpub);
  }
  else {
    emess = encrypt(message, spub);
  }

  int r = sendto(sockfd, (const char *) emess, BUFSIZ, MSG_CONFIRM, (const struct sockaddr *) &saddr, sizeof(saddr));
  if (r < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  // Send the checksum to the server
  unsigned long csum = checksum(message);
  int x = sendto(sockfd, (const char *) csum, BUFSIZ, MSG_CONFIRM, (const struct sockaddr *) &saddr, sizeof(saddr));
  if (x < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  unsigned long scsum;
  // Recieve the checksum and check it
  int y = recvfrom(sockfd, (char *) scsum, BUFSIZ, MSG_CONFIRM, (struct sockaddr *) &saddr, &l);
  if (y == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  close(sockfd);
  return EXIT_SUCCESS;
}
