#ifndef __POWERSUPPLY_H__
#define __POWERSUPPLY_H__

#include "utils.h"

void powerSupply_handle(int conn_sock, powsys_t *powsys, int shmid_s, int msqid);

#endif