#include "config.h"
#include "util.h"
#include "sender.h"
#include "receiver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> // socket, bind, listen
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // conversions
#include <errno.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Simple chat application\n"
				"chaat [self port] [-r|(-s [other port])]\n"
				"-s: send\n"
				"-r: recv\n");
		return -1;
	}
	int ret = 0;
	int self_port = atoi(argv[1]);
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	int option = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(option));
	struct sockaddr_in sa;

	sa.sin_family = AF_INET;
	sa.sin_port = htons(self_port);
	sa.sin_addr.s_addr = INADDR_ANY;
	ret = bind(sock, (struct sockaddr*)&sa, sizeof(sa));
	CHECK(ret == 0, "Could not bind socket");

	if (strcmp(argv[2], "-r") == 0) {
		printf("Listening on port %hu...\n", ntohs(sa.sin_port));
		return chaat_receiver(sock);
	} else if (strcmp(argv[2], "-s") == 0) {
		CHECK(argc >= 4, "Dest port required");
		// TODO: add close calls where necessary

		int other_port = atoi(argv[3]);
		struct sockaddr_in dest_addr;

		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(other_port);
		ret = inet_aton("127.0.0.1", &dest_addr.sin_addr);
		CHECK(ret != 0, "Invalid dest IP");

		printf("Sending on port %hu:\n", ntohs(sa.sin_port));
		return chaat_sender(sock, &dest_addr);
	}
	return 0;
}
