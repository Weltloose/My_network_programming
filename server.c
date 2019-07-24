
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define MSERV_PORT 9878
#define MSERV_IP "172.26.67.159"
#define TEXT_MAX_SIZE 100

typedef struct sockaddr SA;

void sigchld_handler(int signo){
	int chld_pidt;
	while((chld_pidt = waitpid(-1, NULL, WNOHANG)) > 0){
		printf("%d child process terminated.\n", chld_pidt);
	}
}

void Signal(int signo, void (*func)(int)){
	struct sigaction newsig;
	newsig.sa_handler = func;
	sigemptyset(&newsig.sa_mask);
	newsig.sa_flags = 0;
	sigaction(signo, &newsig, NULL);
}

void handle(int socket){
	char temp[TEXT_MAX_SIZE];
	int read_n;
again:
	while((read_n = read(socket, temp, TEXT_MAX_SIZE)) > 0){
		printf("%s", temp);
	}
	if(read_n < 0) goto again;
	if(read_n == 0) return;
}



int main(){
	int sockfd_listen = socket(AF_INET, SOCK_STREAM, 0);
	int sockfd_connect;
	struct sockaddr_in client, server;
	bzero(&server, sizeof(server));
	inet_pton(AF_INET, MSERV_IP, &server.sin_addr);
	server.sin_port = htons(MSERV_PORT);
	server.sin_family = AF_INET;
	if(bind(sockfd_listen, (const SA *)&server, sizeof(server)) < 0){
		fprintf(stderr, "Can't bind.\n");
		exit(0);
	}
	if(listen(sockfd_listen, 10) < 0){
		fprintf(stderr, "Can't listen.\n");
		exit(0);
	}
	Signal(SIGCHLD, sigchld_handler);

	socklen_t client_size = sizeof(client);
	while(1){
		sockfd_connect = accept(sockfd_listen, (SA *)&client, &client_size);
		pid_t child = fork();
		if(child == 0){
			close(sockfd_listen);
			handle(sockfd_connect);
			close(sockfd_connect);
			exit(0);
		}
		close(sockfd_connect);
	}
	close(sockfd_listen);
}