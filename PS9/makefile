test.exe: test.o sched.o adjstack.o savectx64.o
	gcc -o test.exe test.o sched.o adjstack.o savectx64.o

test.o: test.c
	gcc -c test.c

sched.o: sched.c sched.h
	gcc -c sched.c

adjstack.o: adjstack.c
	gcc -c adjstack.c

savectx64.o: savectx64.S
	gcc -c savectx64.S

clean:
	rm *.exe *.o *.stackdump *~
