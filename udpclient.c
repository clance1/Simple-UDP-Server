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
  int port = stoi(argv[2]);
  char* message = argv[3];
  bool isFile = false;

  char* file_end = ".txt";

  struct addrinfo *results;
  struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE
		};

  struct sockaddr_in saddr;

  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY:
  saddr.sin_port = htons(port);


  int addr = getaddrinfo(hostname, port, &hints, &results);

  if (strstr(message, file_end) != NULL) {
    isFile = true;
	int fd = open(message, O_RDONLY);
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
  int s = sendto(sockfd, cpub, BUFSIZ, MSG_CONFIRM, &saddr, sizeof(saddr));
  if (s < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Recieve and decrypt the public key of the Server
  char spub[BUFSIZ] = "";
  int l;
  if ((int t = recvfrom(sockfd, *spub, BUFSIZ, MSG_CONFIRM, &saddr, &l)) == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  char* spub = decrypt(spub);

  // Encrypt the message with the public key of the Server
  if (isFile) {
    char emess[BUFSIZ] = encrypt(fd, cpub);
  }
  else {
    char emess[BUFSIZ] = encrypt(message, spub);
  }
  if ((int s = sendto(sockfd, emess, BUFSIZ, MSG_CONFIRM, &saddr, sizeof(saddr))) < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  // Send the checksum to the server
  unsigned long csum = checksum(message);
  if ((int s = sendto(sockfd, csum, BUFSIZ, MSG_CONFIRM, &saddr, sizeof(saddr))) < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  unsigned long scsum;
  // Recieve the checksum and check it
  if ((int t = recvfrom(sockfd, *scsum, BUFSIZ, MSG_CONFIRM, &saddr, &l)) == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  close(sockfd);
  return EXIT_SUCCESS;
}
