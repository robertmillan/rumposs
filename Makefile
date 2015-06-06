CFLAGS += -D__RCSID\(x\)='' -Wall -Werror -fpic -D_GNU_SOURCE -O2 -g -I.
LDFLAGS += -lpthread -lrt -shared -fpic

# Generic RUMP audio support
LDFLAGS+= -lrumpvfs -lrumpdev -lrumpdev_audio

# PCI audio
LDFLAGS+= -lrumpdev_pci -lrumpdev_audio_ac97 -lrumpdev_pci_auich

all: rumposs.so

wrap.o: wrap.c open.c

ossaudio.o: ossaudio.c
	$(CC) $(CFLAGS) -Wno-unused-variable -c $^ -o $@

rumposs.so: wrap.o ossaudio.o
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -f *.so *.o *~
