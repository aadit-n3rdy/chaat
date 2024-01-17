#ifndef CHAAT_SESSION_H

#include <gnutls/gnutls.h>

#define CHAAT_SESSION_CLIENT 0
#define CHAAT_SESSION_SERVER 1

struct chaat_session {
	char type;
	gnutls_session_t gts;
};

typedef struct chaat_session chaat_session_t;

int chaat_session_init(chaat_session_t *sesh, char type); 
int chaat_session_end(chaat_session_t *sesh);

#endif
