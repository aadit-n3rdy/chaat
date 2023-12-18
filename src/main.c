#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> // socket, bind, listen
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // conversions
#include <errno.h>

#define BUFSIZE 256

int chaat_reciever(int sock) {
	char buf[BUFSIZE];
	while (1) {
		int len = recv(sock, buf, BUFSIZE, 0);
		buf[len] = '\0';
		if (len > 0) {
			printf("%s", buf);
		} else if (len < 0) {
			printf("Error receiving msg: %s\n", strerror(errno));
			return -1;
		} else {
			printf("\n");
		}
	}
	return 0;
}

int chaat_sender(int sock, const struct sockaddr *dest_addr, socklen_t dest_len) {
	char buf[BUFSIZE];
	while (1) {
		fgets(buf, BUFSIZE, stdin);
		int slen = strlen(buf);
		buf[slen] = '\n';
		buf[slen+1] = '\0';
		if (slen > 0) {
			int len = sendto(sock, buf, strlen(buf)-1, 0, dest_addr, dest_len);
			if (len < 0) {
				printf("Sending \"%s\" failed due to error: %s\n", buf, strerror(errno));
				return -1;
			} else if (len != slen) {
				printf("Did not send entire length: %d of %d\n", len, slen);
				return -1;
			}
		}
	}
	return 0;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Simple chat application\n"
				"chaat [self port] [-r|(-s [other port])]\n"
				"-s: send\n"
				"-r: recv\n");
		return -1;
	}
	int self_port = atoi(argv[1]);
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in sa;

	sa.sin_family = AF_INET;
	if (!inet_aton("127.0.0.1", &sa.sin_addr.s_addr)) {
		printf("Invalid IP address\n");
		return -1;
	}
	sa.sin_port = htons(self_port);
	if (bind(sock, &sa, sizeof(sa))) {
		printf("ERROR: Could not bind, %s\n", strerror(errno));
		return -1;
	}

	if (strcmp(argv[2], "-r") == 0) {
		printf("Listening on port %hu...\n", ntohs(sa.sin_port));
		return chaat_reciever(sock);
	} else if (strcmp(argv[2], "-s") == 0) {
		if (argc < 4) {
			printf("Requires destination port number, exiting...\n");
			// TODO: add close calls where necessary
		}
		struct sockaddr_in dest_addr;
		int other_port = atoi(argv[3]);

		dest_addr.sin_port = htons(other_port);
		if (!inet_aton("127.0.0.1", &sa.sin_addr.s_addr)) {
			printf("Invalid dest. IP address\n");
			return -1;
		}
		printf("Sending on port %hu:\n", ntohs(sa.sin_port));
		return chaat_sender(sock, &dest_addr, sizeof(dest_addr));
	}
	return 0;
}
