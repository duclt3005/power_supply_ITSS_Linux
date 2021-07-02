#ifndef __POWERSUPPLYINFO_H__
#define __POWERSUPPLYINFO_H__

#include "utils.h"

void powSupplyInfoAccess_handle(device_t *devices, powsys_t *powsys, int shmid_d, int shmid_s, int msqid);

#endif