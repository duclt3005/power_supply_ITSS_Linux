#ifndef __POWERSUPPLY_H__
#define __POWERSUPPLY_H__

#include "utils.h"

void powerSupply_handle(int conn_sock, powsys_t *powsys, int shmid_s, int msqid);

void powSupplyInfoAccess_handle(device_t *devices, powsys_t *powsys, int shmid_d, int shmid_s, int msqid);

#endif