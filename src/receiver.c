#include "config.h"
#include "util.h"

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

int chaat_receiver(int sock) {
	int ret;

	gnutls_session_t sesh;
	gnutls_certificate_credentials_t creds;
	gnutls_datum_t datum;

	GNUTLS_CHECK(gnutls_certificate_allocate_credentials(&creds), 
			"Couldn't init creds");
	GNUTLS_CHECK(gnutls_certificate_set_x509_key_file(creds, 
			TLS_CERTFILE, TLS_PRIVATE_KEY, GNUTLS_X509_FMT_PEM),
			"Could not set key and cert");
	char buf[BUFSIZE];
	if (listen(sock, 0)) {
		printf("Error listening: %s\n", strerror(errno));
		return -1;
	}

	GNUTLS_CHECK(gnutls_init(&sesh, GNUTLS_SERVER), "Could not init session");
	GNUTLS_CHECK(gnutls_credentials_set(sesh, GNUTLS_CRD_CERTIFICATE, creds),
			"Could not set creds");
	GNUTLS_CHECK(gnutls_set_default_priority(sesh), "Couldn't set prty");

	gnutls_certificate_server_set_request(sesh, GNUTLS_CERT_IGNORE);
	gnutls_handshake_set_timeout(sesh, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);

	printf("Accepting connection...\n");
	struct sockaddr_in dest_addr;
	socklen_t da_len = sizeof(dest_addr);
	int recv_fd = accept(sock, (struct sockaddr*)&dest_addr, &da_len);
	if (recv_fd < 0) {
		printf("Error accepting: %s\n", strerror(errno));
		return -1;
	}
	printf("Accepted connection from %s:%hu\n", inet_ntoa(dest_addr.sin_addr),
			ntohs(dest_addr.sin_port));

	gnutls_transport_set_int(sesh, recv_fd);
	
	do {
		ret = gnutls_handshake(sesh);
	} while (ret > 0 && ret != GNUTLS_E_SUCCESS);
	if (ret < 0) {
		perror("Handshake failed\n");
		close(recv_fd);
		perror(gnutls_strerror(ret));
		exit(-1);
	}
	printf("Handshake SUCCESS!!");

	while (1) {
		do {
			ret = gnutls_record_recv(sesh, buf, BUFSIZE-1);
		} while (ret == GNUTLS_E_AGAIN || ret == GNUTLS_E_INTERRUPTED);
		if (ret == 0) {
			printf("Connection aborted...");
			exit(0);
		} else if (ret < 0) {
			printf("ERROR");
			perror(gnutls_strerror(ret));
			exit(-1);
		}
		int len = ret;
		buf[len] = '\0';
		if (len > 0) {
			printf("%s", buf);
		} else if (len < 0) {
			printf("Error receiving msg: %s\n", strerror(errno));
			return -1;
		}	
	}
	return 0;
}

