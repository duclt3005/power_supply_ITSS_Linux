#ifndef __CONNECTMNG_H__
#define __CONNECTMNG_H__

#include "utils.h"
#include "powerSupply.h"

void connectMng_handle(int server_port, powsys_t *powsys, int shmid_s, int msqid);

#endif