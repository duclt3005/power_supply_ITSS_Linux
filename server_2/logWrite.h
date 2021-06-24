#ifndef __LOGWRITE_H__
#define __LOGWRITE_H__

#include "utils.h"

void logWrite_handle(device_t * devices, powsys_t *powsys, int shmid_d, int shmid_s, int msqid, FILE *log_server);

#endif