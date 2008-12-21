CC=avr-gcc
OBJCOPY=avr-objcopy
STRIP=avr-strip
SIZE=avr-size
AVRDUDE=avrdude

CPPFLAGS += -mmcu=atmega644p -DF_CPU=18432000UL 
CFLAGS += -std=gnu99 -Os -g -Wall -W
LDFLAGS += $(CPPFLAGS) $(CFLAGS) -nostdlib -Wl,--section-start=.text=0xE000
#LDFLAGS += $(CPPFLAGS) $(CFLAGS)  -Wl,--section-start=.text=0xE000

all: zbusloader.hex

zbusloader: zbusloader.o avr_init.o
#zbusloader: zbusloader.o
	$(CC) -o $@ $(LDFLAGS) $^
	$(SIZE) $@

clean:
	rm -f zbusloader *.o *.s *.hex *~

%.hex: %
	$(OBJCOPY) -O ihex -R .eeprom $< $@

load: zbusloader.hex
	$(AVRDUDE) -p m644p -U flash:w:$<

fuse:
	$(AVRDUDE) -p m644p -U lfuse:w:0xe7:m
	$(AVRDUDE) -p m644p -U hfuse:w:0xd0:m
	$(AVRDUDE) -p m644p -U efuse:w:0xfd:m

.PHONY:	fuse load clean
