CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: server_2

server_2: server_2.o connectMng.o elePowerCtrl.o logWrite.o powerSupply.o utils.o
	${CC} server_2.o connectMng.o elePowerCtrl.o logWrite.o powerSupply.o utils.o -o server_2

server_2.o: server_2.c
	${CC} ${CFLAGS} server_2.c

elePowerCtrl.o: elePowerCtrl.c
	${CC} ${CFLAGS} elePowerCtrl.c

connectMng.o: connectMng.c
	${CC} ${CFLAGS} connectMng.c

logWrite.o: logWrite.c
	${CC} ${CFLAGS} logWrite.c

powerSupply.o: powerSupply.c
	${CC} ${CFLAGS} powerSupply.c

utils.o: utils.c
	${CC} ${CFLAGS} utils.c

clean:
	rm -f *.o *~

