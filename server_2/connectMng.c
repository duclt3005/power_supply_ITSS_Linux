#include "connectMng.h"

void connectMng_handle(int server_port, powsys_t *powsys, int shmid_s, int msqid)
{
	///////////////////////
	// Connect to client //
	///////////////////////
	//Step 1: Construct a TCP socket to listen connection request
	int listen_sock, conn_sock;
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		tprintf("socket() failed\n");
		exit(1);
	}

	//////////////////////////////
	// Connect to shared memory //
	//////////////////////////////
	if ((powsys = (powsys_t *)shmat(shmid_s, (void *)0, 0)) == (void *)-1)
	{
		tprintf("shmat() failed\n");
		exit(1);
	}

	//Step 2: Bind address to socket
	struct sockaddr_in server;
	struct sockaddr_in client;
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(server_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listen_sock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		tprintf("bind() failed\n");
		exit(1);
	}

	//Step 3: Listen request from client
	if (listen(listen_sock, BACKLOG) == -1)
	{
		tprintf("listen() failed\n");
		exit(1);
	}

	//Step 4: Communicate with client
	while (1)
	{
		//accept request
		socklen_t sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)) == -1)
		{
			tprintf("accept() failed\n");
			continue;
		}

		// if 11-th device connect to SERVER
		if (powsys->powerSupply_count == MAX_DEVICE)
		{
			char re = '9';
			int bytes_sent;
			if ((bytes_sent = send(conn_sock, &re, 1, 0)) <= 0)
				tprintf("send() failed\n");
			close(conn_sock);
			break;
		}

		// create new process powerSupply
		pid_t powerSupply;
		if ((powerSupply = fork()) < 0)
		{
			tprintf("powerSupply fork() failed\n");
			continue;
		}

		if (powerSupply == 0)
		{
			//in child
			close(listen_sock);
			powerSupply_handle(conn_sock, powsys, shmid_s, msqid);
			close(conn_sock);
		}
		else
		{
			//in parent
			close(conn_sock);
			powsys->powerSupply_count++;
			tprintf("A device connected, connectMng forked new process powerSupply --- pid: %d.\n", powerSupply);
		}
	} //end communication
	close(listen_sock);
} //end function connectMng_handle