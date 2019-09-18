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
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "pg1lib.h"

const char CLIENT_USAGE[] = \
  "Usage: ./udpclient student0[0/1/2/6].cse.nd.edu 41026 [filename.txt or \"message\"]";

double time_diff(struct timeval x , struct timeval y) {
	double x_ms , y_ms , diff;
			
	x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
	y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;
						
	diff = (double)y_ms - (double)x_ms;
								
	return diff;
}

int main(int argc, char* argv[]) {

  // Get command line arguments
  char* hostname = argv[1];
  int port = atoi(argv[2]);
  char* message = argv[3];
  bool isFile = false;
  char text[BUFSIZ];
  char txt[4096] = "";
  struct hostent *server;
  struct timeval start, end;

  struct sockaddr_in saddr;

  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY;
  saddr.sin_port = htons(port);

  struct stat st;

  if (stat(message, &st) >= 0) {
    isFile = true;
	printf("File name: %s\n", message);
    FILE *fp = fopen(message, "r");
	int i = 0;
	
	while(fgets(text, 150, fp)) {
		i++;
	}
	strcat(text,"\0");
	strcat(txt, text);
  }

  // Run socket
  int sockfd = -1;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "ERROR: Socket error - %s\n", strerror(errno));
	close(sockfd);
    return EXIT_FAILURE;
  }

  server = gethostbyname(hostname);
  if (server == NULL) {
	  fprintf(stderr, "ERROR: Finding host - %s\n", strerror(errno));
	  close(sockfd);
	  return EXIT_FAILURE;
  }


  bzero((char *) &saddr, sizeof(saddr));
  saddr.sin_family = AF_INET;
  bcopy((char*) server->h_addr, (char*) &saddr.sin_addr.s_addr, server->h_length);
  saddr.sin_port = htons(port);

  socklen_t l = sizeof(saddr);

  // Get public key
  char* cpub = getPubKey();

  gettimeofday(&start, NULL);
  // Sends the public key to the Server
  int s = sendto(sockfd, (const char *) cpub, BUFSIZ, 0, (const struct sockaddr *) &saddr, sizeof(saddr));
  if (s < 0) {
    fprintf(stderr, "ERROR: Sending error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Recieve and decrypt the public key of the Server
  char spub_rec[BUFSIZ] = "";
  int t = recvfrom(sockfd, (char *) spub_rec, BUFSIZ, 0, (struct sockaddr *) &saddr, &l);
  if (t == 0) {
    fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  char* spub = decrypt(spub_rec);


  // Encrypt the message with the public key of the Server
  char* emess = "";
  if (isFile) {
    emess = encrypt(text, cpub);
	printf("Message sending:\n%s\n",text);
  }
  else {
	printf("Message sending:\n%s\n",message);
    emess = encrypt(message, spub);
  }

  sendto(sockfd, (const char *) emess, BUFSIZ, 0, (const struct sockaddr *) &saddr, sizeof(saddr));
  
  // Send the checksum to the server
  unsigned long csum = checksum(message);
  printf("Checksum Sent: %lu\n", csum);
  char * csum_s = malloc(4096);
  sprintf(csum_s, "%lu", csum);
  sendto(sockfd, (const char *) csum_s, BUFSIZ, 0, (const struct sockaddr *) &saddr, sizeof(saddr));
  
  // Receive checksum
  char* ocsum_s = malloc(4096);
  int y = recvfrom(sockfd, (char *) ocsum_s, 4096, MSG_DONTWAIT, (struct sockaddr *) &saddr, &l);
  if (y == 0) {
  	fprintf(stderr, "ERROR: Recieving error - %s\n", strerror(errno));
  	close(sockfd);
  	exit(EXIT_FAILURE);
  }
  gettimeofday(&end, NULL);
  printf("RTT: %.00lf microseconds\n", time_diff(start, end));
  printf("Recieved checksum: %s\n", csum_s);
  close(sockfd);
  return EXIT_SUCCESS;
}
