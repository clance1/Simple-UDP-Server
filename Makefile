all:
	gcc -lcrypto -lz -Wall udpclient.c -o udpclient
	gcc -lcrypto -lz -Wall udpserver.c -o udpserver
