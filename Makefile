# Makefile for zmqGenerator

OBJ=neventArray.o zmqGenerator.o nexus2event.o posix-timers.o md5.o
NXINSTALL=/afs/psi.ch/project/sinq/sl6-64

ROBJ=zmqReader.o

all: zmqGenerator zmqReader

.c.o:
	$(CC) -c -g -I$(NXINSTALL)/include $*.c

zmqGenerator: $(OBJ)
	$(CC) -g -o zmqGenerator -L$(NXINSTALL)/lib $(OBJ) -lNeXus -lhdf5 -lsz -lrt -lzmq -lsodium

zmqReader: $(ROBJ)
	$(CC) -g -o zmqReader $(ROBJ) -L$(NXINSTALL)/lib -lzmq -lsodium

clean:
	- rm *.o
	- rm zmqGenerator




