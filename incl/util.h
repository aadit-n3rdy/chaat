#ifndef CHAAT_UTIL_H
#define CHAAT_UTIL_H

#include <errno.h>

#define CHECK(x, msg) if (!(x)) {\
	perror(msg); \
	perror(#x); \
	exit(-1); \
}

#define GNUTLS_CHECK(x, msg) if ((x) < 0) { \
	perror(msg); \
	exit(-1); \
}

#endif
