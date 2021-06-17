#include "utils.h"
#include "connectMng.h"
#include "elePowerCtrl.h"
#include "powerSupply.h"
#include "logWrite.h"

int server_port;
pid_t connectMng, powerSupply, elePowerCtrl, powSupplyInfoAccess, logWrite;
int listen_sock, conn_sock;
char recv_data[BUFF_SIZE];
int bytes_sent, bytes_received;
struct sockaddr_in server;
struct sockaddr_in client;
unsigned int sin_size;
char use_mode[][10] = {"off", "normal", "limited"};
key_t key_s = 8888, key_d = 1234, key_m = 5678; //system info, device storage, message queue
int shmid_s, shmid_d, msqid;					//system info, device storage, message queue
FILE *log_server;

powsys_t *powsys;
device_t *devices;

void sigHandleSIGINT();

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
		exit(1);
	}
	server_port = atoi(argv[1]);
	printf("SERVER start, PID is %d.\n", getpid());

	///////////////////////////////////////////
	// Create shared memory for power system //
	///////////////////////////////////////////
	if ((shmid_s = shmget(key_s, sizeof(powsys_t), 0644 | IPC_CREAT)) < 0)
	{
		tprintf("shmget() failed\n");
		exit(1);
	}
	if ((powsys = (powsys_t *)shmat(shmid_s, (void *)0, 0)) == (void *)-1)
	{
		tprintf("shmat() failed\n");
		exit(1);
	}
	powsys->powerSupply_count = 0;
	powsys->current_power = 0;
	powsys->threshold_over = 0;
	powsys->supply_over = 0;
	powsys->reset = 0;

	/////////////////////////////////////////////
	// Create shared memory for devices storage//
	/////////////////////////////////////////////
	if ((shmid_d = shmget(key_d, sizeof(device_t) * MAX_DEVICE, 0644 | IPC_CREAT)) < 0)
	{
		tprintf("shmget() failed\n");
		exit(1);
	}
	if ((devices = (device_t *)shmat(shmid_d, (void *)0, 0)) == (void *)-1)
	{
		tprintf("shmat() failed\n");
		exit(1);
	}

	// Init data for shared memory
	int i;
	for (i = 0; i < MAX_DEVICE; i++)
	{
		devices[i].pid = 0;
		strcpy(devices[i].name, "");
		devices[i].use_power[0] = 0;
		devices[i].use_power[1] = 0;
		devices[i].use_power[2] = 0;
		devices[i].mode = 0;
	}

	//////////////////////////////////
	// Create message queue for IPC //
	//////////////////////////////////
	if ((msqid = msgget(key_m, 0644 | IPC_CREAT)) < 0)
	{
		tprintf("msgget() failed\n");
		exit(1);
	}

	///////////////////
	// Handle Ctrl-C //
	///////////////////
	signal(SIGINT, sigHandleSIGINT);

	///////////////////////////////////
	// start child process in SERVER //
	///////////////////////////////////
	if ((connectMng = fork()) == 0)
	{
		connectMng_handle(
			msqid,
			shmid_s,
			shmid_d,
			listen_sock,
			conn_sock,
			server,
			client,
			server_port,
			sin_size,
			bytes_sent,
			bytes_received,
			powerSupply,
			powsys
		);
	}
	else if ((elePowerCtrl = fork()) == 0)
	{
		elePowerCtrl_handle(
			msqid,
			shmid_s,
			shmid_d,
			devices,
			powsys
		);
	}
	else if ((powSupplyInfoAccess = fork()) == 0)
	{
		powSupplyInfoAccess_handle(
			msqid,
			shmid_s,
			shmid_d,
			use_mode,
			devices,
			powsys
		);
	}
	else if ((logWrite = fork()) == 0)
	{
		logWrite_handle(
			msqid,
			shmid_s,
			shmid_d,
			devices,
			powsys,
			log_server
		);
	}
	else
	{
		tprintf("SERVER forked new process connectMng ------------------ pid: %d.\n", connectMng);
		tprintf("SERVER forked new process elePowerCtrl ---------------- pid: %d.\n", elePowerCtrl);
		tprintf("SERVER forked new process powSupplyInfoAccess --------- pid: %d.\n", powSupplyInfoAccess);
		tprintf("SERVER forked new process logWrite -------------------- pid: %d.\n\n", logWrite);
		waitpid(connectMng, NULL, 0);
		waitpid(elePowerCtrl, NULL, 0);
		waitpid(powSupplyInfoAccess, NULL, 0);
		waitpid(logWrite, NULL, 0);
		tprintf("SERVER exited\n\n");
	}

	return 0;
}

void sigHandleSIGINT()
{
	msgctl(msqid, IPC_RMID, NULL);
	shmctl(shmid_s, IPC_RMID, NULL);
	shmctl(shmid_d, IPC_RMID, NULL);
	fclose(log_server);
	kill(0, SIGKILL);
	exit(0);
}