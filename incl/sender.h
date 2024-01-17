#ifndef CHAAT_SENDER_H
#define CHAAT_SENDER_H

#include <netinet/in.h>

int chaat_sender(int sock, const struct sockaddr_in *dest_addr);

#endif
