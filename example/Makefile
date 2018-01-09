
CC=../gcc-linaro-arm-linux-gnueabihf-4.7-2012.11-20121123_linux/bin/arm-linux-gnueabihf-gcc
CC=../gcc-linaro-4.9-2015.02-3-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-gcc

all:
	$(CC) VideoTest.c \
			-L/usr/local/lib/media-codec/ \
			-lcedar_vencoder -lcedar_common -lcedar_base\
			-I/home/mouse9911/media-codec/vencoder/include/ \
			-Wl,-rpath=/usr/local/lib/media-codec/ -o VideoTest

#			-mfloat-abi=soft

clean:
	rm a.out

