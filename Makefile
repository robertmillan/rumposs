CFLAGS += -Wall -Werror -fpic -D_GNU_SOURCE -O2 -g -I.
LDFLAGS += -lpthread -lrt -shared -fpic

# Generic RUMP audio support
LDFLAGS+= -Wl,--no-as-needed -lrumpvfs -Wl,--no-as-needed -lrumpdev -Wl,--no-as-needed -lrumpdev_audio

# PCI audio
LDFLAGS+= -Wl,--no-as-needed -lrumpdev_pci
LDFLAGS+= -Wl,--no-as-needed -lrumpdev_audio_ac97 -Wl,--no-as-needed -lrumpdev_pci_auich
LDFLAGS+= -Wl,--no-as-needed -lrumpdev_pci_hdaudio -Wl,--no-as-needed -lrumpdev_hdaudio_hdafg

all: rumposs.so

wrap.o: wrap.c open.c

ossaudio.o: ossaudio.c
	$(CC) $(CFLAGS) -Wno-unused-variable -c $^ -o $@

rumposs.so: wrap.o ossaudio.o
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -f *.so *.o *~
