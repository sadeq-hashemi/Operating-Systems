all: shm_server shm_client exec
shm_server: shm_server.c
	gcc shm_server.c -o shm_server -Wall -Werror -lrt -lpthread 
shm_client: shm_client.c
	gcc shm_client.c -o shm_client -Wall -Werror -lrt -lpthread 
exec: exec.c
	gcc exec.c -o exec -Wall -Werror -lrt -lpthread
clean: 
	shm_server
	shm_client
	exec
