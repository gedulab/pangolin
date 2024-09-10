all: pangolin

CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lpthread

SRC := pangolin.c \
		log.c \
		magic.c \
		rk.c \
		rk3588.c \
		rk3328.c \
		ipc.c

OBJ := $(patsubst %.c, %.o, $(SRC))

pangolin: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $@ $(LDFLAGS)

clean:
	rm -rf pangolin *.o *.sh
