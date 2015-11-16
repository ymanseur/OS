fifotest.exe: fifotest.o fifo.o sem.o tas64.s 
	gcc -o fifotest.exe fifotest.o fifo.o sem.o tas64.s

testandset.o: testandset.c
	gcc -c testandset.c

fifotest.o: fifotest.c
	gcc -c fifotest.c

fifo.o: fifo.c fifo.h
	gcc -c fifo.c

sem.o: sem.c sem.h
	gcc -c sem.c

clean:
	rm -f *.exe *.o *.stackdump *~

backup:
	test -d backups || mkdir backups
	cp *.c backups
	cp *.h backups
	cp *.s backups
	cp Makefile backups