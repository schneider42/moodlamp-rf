all: moodsound

moodsound:
	gcc -o moodsound `pkg-config --cflags --libs gstreamer-0.10` moodsound.c

clean:
	rm -f moodsound
