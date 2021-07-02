#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <signal.h>
#include <stddef.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define POWER_THRESHOLD 5000
#define WARNING_THRESHOLD 4500
#define BACKLOG 10 /* Number of allowed connections */
#define BUFF_SIZE 8192
#define MAX_DEVICE 10
#define MAX_LOG_DEVICE 100
#define MAX_MESSAGE_LENGTH 200
#define MAX_MESSAGE_LENGTH_TEMP 200

// power system struct
typedef struct
{
	int powerSupply_count;
	int current_power;
	int threshold_over;
	int supply_over;
} powsys_t;

// device struct
typedef struct
{
	int pid;
	char name[50];
	int use_power[3];
	int mode;
} device_t;

/**
 * message struct
 * mtype = 1 -> logWrite_handle
 * mtype = 2 -> powSupplyInfoAccess_handle
 */
typedef struct
{
	long mtype;
	char mtext[MAX_MESSAGE_LENGTH];
} msg_t;

int tprintf(const char *fmt, ...);

#endif