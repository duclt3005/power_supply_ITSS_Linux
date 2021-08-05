#ifndef __POWERSUPPLY_H__
#define __POWERSUPPLY_H__

#include "utils.h"

void powerSupply_handle(
	int msqid,
	int shmid_s,
	int shmid_d,
	int conn_sock,
	int bytes_received,
	powsys_t *powsys
);

void powSupplyInfoAccess_handle(
	int msqid,
	int shmid_s,
	int shmid_d,
	char use_mode[][10],
	device_t *devices,
	powsys_t *powsys
);

#endif