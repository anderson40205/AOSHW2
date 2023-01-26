all:client server 
client:client.c
	gcc client.c -o client -lpthread
server:server.c 
	gcc server.c -o server -lpthread
clean:
	rm *.o client server