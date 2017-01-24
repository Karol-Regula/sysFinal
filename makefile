compile: pipe_networking.c pipe_networking.h client.c server.c fnake.c fnake.h
	gcc client.c pipe_networking.c fnake.c -o client; gcc server.c pipe_networking.c -o server

clean:
	rm server
	rm client
	rm fnake
	rm *.txt
	rm *.csv
	rm *~
