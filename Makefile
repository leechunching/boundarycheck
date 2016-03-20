FFPATH=../ffmpeg
CC=gcc

CFLAGS=-I./ -I${FFPATH}/libavcodec -I${FFPATH}/libavdevice -I${FFPATH}/libavformat -I${FFPATH}/libavutil -I${FFPATH}/libavfilter -I${FFPATH}/libswscale -I/usr/include/SDL -I${FFPATH} -Wall -g


LDFLAGS=${FFPATH}/cmdutils.o  ${FFPATH}/libavdevice/libavdevice.a ${FFPATH}/libavformat/libavformat.a ${FFPATH}/libavcodec/libavcodec.a ${FFPATH}/libswscale/libswscale.a ${FFPATH}/libavutil/libavutil.a  ${FFPATH}/libavdevice/libavdevice.a ${FFPATH}/libavfilter/libavfilter.a -lSDL -lz -lbz2 -lasound

EXLIB= -L/usr/lib -lX11 -lm -lpthread

OBJS=ffplay.o boundaryCheck.o

#OBJS=ffplay.o ffparser.o filedb.o xdraw.o comp.o
#${FFPATH}/cmdutils.o 

all: ${OBJS} 
	${CC} -o ffplay  ${OBJS} ${LDFLAGS} ${EXLIB}
.c.o: *.c *.h
	$(CC) ${CFLAGS} -c $< -o $@

clean:
	rm *.o ffplay
