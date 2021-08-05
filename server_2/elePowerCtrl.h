#ifndef __ELEPOWERCTRL_H__
#define __ELEPOWERCTRL_H__

#include "utils.h"

void elePowerCtrl_handle(
	int msqid,
	int shmid_s,
	int shmid_d,
	device_t *devices,
	powsys_t *powsys
);

#endif