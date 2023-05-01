DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/

CFLAGS := -I$(DIRHEA) -c -Wall -std=c99
LDLIBS := -lrt
CC := gcc

all : dirs manager processor counter

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

# INCLUDE THE REMAINING RULES TO GENERATE EXECUTABLE FILES

$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

test:
	./exec/manager 4 Wh data/test.txt 

solution:
	./exec/manager 5 aux data/test_solution.txt

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~
