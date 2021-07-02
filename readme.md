# ITSS Embbed Linux

## How to run
- Before running create an empty folder log to store log files
- server
```
make all
make clean
./server <PORT>
```
- client
```
gcc -o client client.c
./client <PORT>
```
