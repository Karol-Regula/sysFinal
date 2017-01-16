compile: pipe_networking.c pipe_networking.h client.c server.c fnake.c
	gcc client.c pipe_networking.c -o client; gcc server.c pipe_networking.c -o server; gcc fnake.c -o fnake;

clean:
	rm server
	rm client
	rm *~
