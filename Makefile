PROJECT = trove
HEADERS = $(PROJECT).h
OBJ = trove.o build.o remove.o update.o findfiles.o search.o hashtable.o
C11 = cc -std=c11
CFLAGS = -Wall -Werror

$(PROJECT):$(OBJ)
	$(C11) $(FLAGS) -o $(PROJECT) $(OBJ)

%.o:%.c $(HEADERS)
	$(C11) $(CFLAGS) -c $<

clean:
	rm -f $(PROJECT) $(OBJ)