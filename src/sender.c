#include "sender.h"

#include "config.h"
#include "util.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

int chaat_sender(int sock, const struct sockaddr_in *dest_addr) {
	int ret;

	gnutls_session_t tls_sesh;
	gnutls_certificate_credentials_t creds; 
	gnutls_priority_t prty_cache;
	gnutls_datum_t datum;

	GNUTLS_CHECK(gnutls_certificate_allocate_credentials(&creds),
			"Couldn't alloc space for certs");
	GNUTLS_CHECK(gnutls_certificate_set_x509_trust_file(creds, TLS_CAFILE,
				GNUTLS_X509_FMT_PEM), "Could not set trust file");

	GNUTLS_CHECK(gnutls_init(&tls_sesh, GNUTLS_CLIENT), 
			"Could not init GnuTLS client");
	GNUTLS_CHECK(gnutls_credentials_set(tls_sesh, GNUTLS_CRD_CERTIFICATE, creds),
			"Could not set credentials");
	gnutls_session_set_verify_cert(tls_sesh, NULL, 0);
	GNUTLS_CHECK(gnutls_set_default_priority(tls_sesh), "Couldn't set prty");

	ret = connect(sock, (const struct sockaddr*)dest_addr, sizeof(*dest_addr));
	CHECK(ret >= 0, "Couldn't connect: ");

	gnutls_transport_set_int(tls_sesh, sock);
	gnutls_handshake_set_timeout(tls_sesh, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);
	do {
		ret = gnutls_handshake(tls_sesh);
	} while (ret > 0 && ret != GNUTLS_E_SUCCESS);
	if (ret < 0) {
		perror("Handshake failed!!");
		int status;
		int type;
		switch (ret) {
			case GNUTLS_E_CERTIFICATE_VERIFICATION_ERROR:
				status = gnutls_session_get_verify_cert_status(tls_sesh);
				type = gnutls_certificate_type_get(tls_sesh);
				GNUTLS_CHECK(gnutls_certificate_verification_status_print(status,
							type, &datum, 0), "CVF failed");
				fprintf(stderr, "Error: %s\n", datum.data);
				gnutls_free(datum.data);
				return -1;
				break;
			default:
				fprintf(stderr, "No idea what the problem was: %s\n", 
						gnutls_strerror(ret));
		}
	}

	char buf[BUFSIZE];
	while (1) {
		fgets(buf, BUFSIZE-1, stdin);
		int slen = strlen(buf);
		if (slen > 0) {
			buf[slen] = '\n';
			slen++;
			buf[slen] = '\0';
			int len = 0;
			do {
				ret = gnutls_record_send(tls_sesh, buf, strlen(buf)-1);
				len += (ret != GNUTLS_E_AGAIN && ret != GNUTLS_E_INTERRUPTED) ?
					ret : 0;
			} while (ret == GNUTLS_E_AGAIN || ret == GNUTLS_E_INTERRUPTED);
			CHECK(len >= 0, "Could not send msg");
			CHECK(len == slen-1, "Incomplete msg sent");
		}
	}
	return 0;
}
