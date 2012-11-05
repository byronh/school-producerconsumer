all: producerconsumer

producerconsumer: producerconsumer.c
	gcc -o producerconsumer producerconsumer.c -pthread

clean:
	rm -rf producerconsumer
