CC=gcc
CFLAGS=-lreadline -I. -Wall 
DEPS = jobcontroller.c myclock.c myremindme.c myls.c mypinfo.c mycd.c mypwd.h myecho.h execute.h display.h input.h 
OBJ = jobcontroller.o myclock.o myremindme.o myls.o mypinfo.o mycd.o mypwd.o myecho.o execute.o display.o input.o shell.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

shell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ core
