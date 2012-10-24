life: life.o
	gcc -o ./bin/life.exe life.o -lpthread

life.o: life.c life.h
	gcc -c life.c

clean:
	rm *.o
	rm ./bin/*.exe
