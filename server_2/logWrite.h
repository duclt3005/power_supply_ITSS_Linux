#ifndef __LOGWRITE_H__
#define __LOGWRITE_H__

#include "utils.h"

void logWrite_handle(
	int msqid,
	int shmid_s,
	int shmid_d,
	device_t *devices,
	powsys_t *powsys,
	FILE *log_server
);

#endif