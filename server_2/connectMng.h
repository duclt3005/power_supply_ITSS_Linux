#ifndef __CONNECTMNG_H__
#define __CONNECTMNG_H__

#include "utils.h"
#include "powerSupply.h"

void connectMng_handle(
	int msqid,
	int shmid_s,
	int shmid_d,
	int listen_sock,
	int conn_sock,
	struct sockaddr_in server,
	struct sockaddr_in client,
	int server_port,
	int sin_size,
	int bytes_sent,
	int bytes_received,
	pid_t powerSupply,
	powsys_t *powsys
);

#endif