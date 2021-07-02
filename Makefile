CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: server

server: server.o connectMng.o elePowerCtrl.o logWrite.o powerSupply.o powSupplyInfoAccess.o utils.o
	${CC} server.o connectMng.o elePowerCtrl.o logWrite.o powerSupply.o powSupplyInfoAccess.o utils.o -o server

server.o: server.c
	${CC} ${CFLAGS} server.c

elePowerCtrl.o: elePowerCtrl.c
	${CC} ${CFLAGS} elePowerCtrl.c

connectMng.o: connectMng.c
	${CC} ${CFLAGS} connectMng.c

logWrite.o: logWrite.c
	${CC} ${CFLAGS} logWrite.c

powerSupply.o: powerSupply.c
	${CC} ${CFLAGS} powerSupply.c

powSupplyInfoAccess.o: powSupplyInfoAccess.c
	${CC} ${CFLAGS} powSupplyInfoAccess.c

utils.o: utils.c
	${CC} ${CFLAGS} utils.c

clean:
	rm -f *.o *~

