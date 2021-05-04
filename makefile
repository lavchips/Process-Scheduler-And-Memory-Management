##Adapted from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC = gcc
CFLAGS = -lm -Wall -Wextra -std=gnu99 -I.
OBJ = scheduler.o scheduling.o statistics_and_linkedlist.o memory_management.o
EXE = scheduler

##Create .o files from .c files. Searches for .c files with same .o names given in OBJ
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

##Create executable linked file from object files.
$(EXE): $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

##Other dependencies
scheduler.o: scheduler.c

scheduling.o: scheduling.h scheduling.c

statistics_and_linkedlist.o: statistics_and_linkedlist.h statistics_and_linkedlist.c

memory_management.o: memory_management.h memory_management.c



##Performs clean (i.e. delete object files) and deletes executable
clean:
	rm -f $(EXE) $(OBJ)

