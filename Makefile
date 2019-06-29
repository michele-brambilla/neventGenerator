# Makefile for zmqGenerator

OBJ=neventArray.o zmqGenerator.o nexus2event.o 
NXINSTALL=
HDF5_LIBRARY_PATH = /opt/stow/hdf5-1.8/lib
ROBJ=zmqReader.o

INC = neventArray.h nexus2event.h posix_timers.h md5.h config.h utils.h
CC = gcc-mp-6

all: zmqGenerator zmqReader

.c.o:
	$(CC) -c -ggdb -I$(NXINSTALL)/include $*.c

zmqGenerator: $(OBJ)
	$(CC) -ggdb -o zmqGenerator -L$(NXINSTALL)/lib $(OBJ) -lNeXus -lzmq -lsodium -lmxml -Wl,-rpath $(HDF5_LIBRARY_PATH)/libhdf5-shared.1.8.18.dylib

zmqReader: $(ROBJ)
	$(CC) -ggdb -o zmqReader $(ROBJ) -L$(NXINSTALL)/lib -lzmq -lsodium

clean:
	- rm *.o
	- rm zmqGenerator zmqReader




# DO NOT DELETE

zmqReader.o: /usr/include/stdio.h /usr/include/features.h
zmqReader.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
zmqReader.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-64.h
zmqReader.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
zmqReader.o: /usr/include/libio.h /usr/include/_G_config.h
zmqReader.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
zmqReader.o: /usr/include/bits/sys_errlist.h /usr/include/string.h
zmqReader.o: /usr/include/xlocale.h /usr/include/stdint.h
zmqReader.o: /usr/include/bits/wchar.h /usr/include/stdlib.h
zmqReader.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
zmqReader.o: /usr/include/endian.h /usr/include/bits/endian.h
zmqReader.o: /usr/include/bits/byteswap.h /usr/include/sys/types.h
zmqReader.o: /usr/include/time.h /usr/include/sys/select.h
zmqReader.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
zmqReader.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
zmqReader.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
zmqReader.o: neventArray.h config.h
