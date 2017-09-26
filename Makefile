#created by ljk<ljk@xdja.com>
ifndef CC
CC = gcc
endif
CFLAGS += -g -O0 -Wall -fPIC
CFLAGS += -I./
LDFLAGS += -L./
LIBS += -lc

.PHONY : all clean

SRCS += event_epoll.c test.c

OBJS = $(SRCS:.c=.o)

TARGET = test

all : ${TARGET}

${TARGET} : ${OBJS}
	${CC} -o $@ $^ ${LDFLAGS} ${LIBS}
	@echo "$@"

%.o : %.c %.h
	${CC} ${CFLAGS} -o $@ -c $<

clean :
	rm -rf ${OBJS} ${TARGET}
