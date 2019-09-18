all:
	gcc -std=gnu99 -lcrypto -lz -Wall udpclient.c -o udpclient
	gcc -std=gnu99 -lcrypto -lz -Wall udpserver.c -o udpserver
