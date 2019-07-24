
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define TEXT_MAX_SIZE 100
#define MSERV_IP "172.26.67.159"
#define MSERV_PORT 9878

typedef struct sockaddr SA;

void handle(int sock){
	char text[TEXT_MAX_SIZE];
	scanf("%[^\n]", text);
	printf("%s", text);
	write(sock, text, strlen(text));
}

int main(){
	struct sockaddr_in server;
	int sockfd_myself = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&server, sizeof(server));    
	server.sin_family = AF_INET;
	inet_pton(AF_INET, MSERV_IP, &server.sin_addr);
	server.sin_port = htons(MSERV_PORT);
	if(connect(sockfd_myself, (SA *)&server, sizeof(server)) < 0){
		fprintf(stderr, "Can't conncet to server");
		exit(0);
	}
	handle(sockfd_myself);
	close(sockfd_myself);

}