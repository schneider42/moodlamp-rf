CC=avr-gcc
OBJCOPY=avr-objcopy
STRIP=avr-strip
SIZE=avr-size
AVRDUDE=avrdude

CPPFLAGS += -mmcu=atmega16 -DF_CPU=16000000UL 
CFLAGS += -std=gnu99 -Os -g -Wall -W
LDFLAGS +=  $(CPPFLAGS) $(CFLAGS) -nostdlib -Wl,--section-start=.text=0x3C00

all: funkloader.hex

funkloader: funkloader.o rfm12_trans.o rfm12_wait_read.o \
	funkloader_tx_reply.o avr_init.o
	$(CC) -o $@ $(LDFLAGS) $^
	$(SIZE) $@

clean:
	rm -f funkloader *.o *.s *.hex *~

%.hex: %
	$(OBJCOPY) -O ihex -R .eeprom $< $@

load: funkloader.hex
	$(AVRDUDE) -p m16 -U flash:w:$<

fuse:
#	$(AVRDUDE) -p m16 -U lfuse:w:0xbf:m
	$(AVRDUDE) -p m16 -U lfuse:w:0x3f:m
	$(AVRDUDE) -p m16 -U hfuse:w:0xc0:m

funkloader.o: funkloader.c pinconfig.h
funkloader_tx_reply.o: funkloader_tx_reply.S pinconfig.h
rfm12_trans.o: rfm12_trans.S pinconfig.h
rfm12_wait_read.o: rfm12_wait_read.S pinconfig.h

.PHONY:	fuse load clean
