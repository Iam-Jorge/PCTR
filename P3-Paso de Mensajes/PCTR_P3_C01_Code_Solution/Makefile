DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -std=c99
LDLIBS := -lrt
CC := gcc

all : dirs manager processor decoder

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

manager: $(DIROBJ)manager.o
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

processor: $(DIROBJ)processor.o
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

decoder: $(DIROBJ)decoder.o
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

test:
	./exec/manager 45.1.8.8.56.0.11.10.1 4 2 4

solution:
	./exec/manager 19.5.4.-3.8.-9.10.9.62.5.4.62.-9.62.23.5.5.-6.62.26.5.-8 10 4 7

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~
