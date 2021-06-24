#ifndef __ELEPOWERCTRL_H__
#define __ELEPOWERCTRL_H__

#include "utils.h"

void elePowerCtrl_handle(device_t *devices, powsys_t *powsys, int shmid_d, int shmid_s, int msqid);

#endif