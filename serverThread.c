#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>

#define BUF_SIZE 512
#define NO_OF_CLIENTS 10

int clientfd[NO_OF_CLIENTS];

void* socketChat(void *arg){
	char buffer[BUF_SIZE];
	int retval;
	int clientfd = *((int*)arg);
 	int targetclient=-1;
	
	while(1){
		printf("iteration");	
		bzero(buffer, BUF_SIZE);
		retval = read(clientfd, buffer, BUF_SIZE);
	
		if(retval < 0){
			perror("Reading Error");
			break;	
		}
		if(strncmp(buffer, "exit", 4) == 0 || strlen(buffer) < 1){
			printf("Client %d: %s\n",clientfd, buffer);
			break;
		}

		
		else if(strncmp(buffer,"connect",7)==0){
			targetclient=buffer[8]-'0';
			printf("Target is set to %d",targetclient);
			fflush(stdin);
			
		}
		else if(strncmp(buffer,"show",4)==0){
			printf("People online are");
			fflush(stdin);

			
		}
		else if(strncmp(buffer,"disconnect",10)==0){
			targetclient=-1;
			printf("Disconnected from target");
			fflush(stdin);
	
		}
		else{
			if(targetclient==-1){
				targetclient=clientfd;
			}
		printf("I am writing no matter what");
		fflush(stdin);
		retval = write(targetclient, buffer, strlen(buffer));
		if(retval < 0){
			perror("Writing Error");
			break;
		}	
		}

	
	}
	// Linked list entry delete
	printf("\n[+] Client %d disconnected from server\n", clientfd);
}


int main(int argc, char *argv[]){

	if(argc < 2){
		printf("SYNTAX: %s [portno]", argv[0]);
		exit(1);
	}
	
	int sockfd, clientfd, portno, retval;
	struct sockaddr_in server_addr, client_addr;
	socklen_t clientlen;
	portno = atoi(argv[1]);
	
// 1) Socket function
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("Socket creation Error");
		exit(1);
	}
	bzero((char *)&server_addr, sizeof(server_addr)); // fills with zero
		
	// set the values in structure
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // any local address
	server_addr.sin_port = htons(portno);
	
// 2) Bind function
	if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		perror("Binding Error");
		exit(1);
	}
		
// 3) Listen function
	if(listen(sockfd, NO_OF_CLIENTS) == 0)
		printf("Listening on port: %d", portno);
	else
		printf("Error in listening");
		
	pthread_t pid[NO_OF_CLIENTS];
	int i = 0;
	while(1){
	// 4) Accept function
		clientlen = sizeof(client_addr);
		clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &clientlen);
		if(clientfd < 0){
			perror("Accepting Error");
			exit(1);
		}
		else{
			printf("\n[+] Client %d connected to server\n", clientfd);
		}
	// 5) Threading
	
		pthread_create(&pid[i],NULL,&socketChat,&clientfd);
		i++;	
	}
	
	close(sockfd);
	
	return 0;	
}
