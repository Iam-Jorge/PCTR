DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall
LDFLAGS := -lpthread -lrt
CC := gcc

all : dirs manager barbero cliente

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

manager: $(DIROBJ)manager.o $(DIROBJ)semaforoI.o $(DIROBJ)memoriaI.o
	$(CC) $(LDFLAGS) -o $(DIREXE)$@ $^

barbero: $(DIROBJ)barbero.o $(DIROBJ)semaforoI.o $(DIROBJ)memoriaI.o
	$(CC) $(LDFLAGS) -o $(DIREXE)$@ $^

cliente: $(DIROBJ)cliente.o $(DIROBJ)semaforoI.o $(DIROBJ)memoriaI.o
	$(CC) $(LDFLAGS) -o $(DIREXE)$@ $^

$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~ 
