//Kaitlyn Jarrett
//Lab 4 server
//due 5/11/15



#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>


void connection(int connect_fd);


#define PORT 3060
#define MSG "<remcp>\n"
#define SECRET "<CS30615spr>\n"
#define MATCH "<ok>\n"
#define READY "<ready>\n"
#define ERROR "<error>\n"
#define CONFIRMED "<send>\n"

int main(int argc, char const *argv[])
{

	int listen_fd, connect_fd;
	struct sockaddr_in servaddr;
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	listen(listen_fd, 10);

	if(errno){
		fprintf(stderr, "Error in creating socket.\n");
		return EXIT_FAILURE;
	}

	while(1) {
		connect_fd = accept(listen_fd, (struct sockaddr *) NULL, NULL);
		connection(connect_fd);
	}
	int i=1;
	setsockopt(listen_fd ,SOL_SOCKET, SO_REUSEADDR, &i, sizeof(1));
	return EXIT_SUCCESS;
}

void connection(int connect_fd){
	char buff[strlen(MSG)];
	char buff2[strlen(SECRET)];
	char buff3[strlen(CONFIRMED)];
	char fileBuff[4096];
	char fileName[4096];
	int size;
	if(read(connect_fd, buff, strlen(MSG)+1)<=0||strcmp(buff,MSG)!=0)
	{
		fprintf(stderr, "Wrong message: %s. Connection will close.\n", buff);
		close(connect_fd);
		return;
	}

	printf("Client says: %s", MSG);

	if(write(connect_fd, MSG, strlen(MSG)+1)==-1){
		close(connect_fd);
		return;
	}

	if(read(connect_fd, buff2, strlen(SECRET)+1)<=0||strcmp(buff2,SECRET)!=0)
	{
		fprintf(stderr, "Wrong message: %s. Connection will close.\n", buff2);
		close(connect_fd);
		return;
	}

	printf("Client says: %s", SECRET);

	if(write(connect_fd, MATCH, strlen(MATCH)+1)==-1){
		close(connect_fd);
		return;
	}

	if((size=read(connect_fd, fileName, 4096))<=0){
		close(connect_fd);
		return;
	}
	fileName[size]='\0';

	printf("Client says: %s\n",fileName);

	if(access(fileName, F_OK&R_OK)!=0)
	{
		write(connect_fd, ERROR, strlen(ERROR)+1);
		fprintf(stderr, "%s could not be opened.\n",fileName);
		close(connect_fd);
		return;
	}

	printf("%s exists and is readable.\n",fileName);

	if(write(connect_fd, READY, strlen(READY)+1)==-1){
		close(connect_fd);
		return;
	}

	if(read(connect_fd, buff3, strlen(CONFIRMED)+1)<=0||strcmp(buff3,CONFIRMED)!=0)
	{
		fprintf(stderr, "Wrong message: %s. Connection will close.\n", buff3);
		close(connect_fd);
		return;
	}
	//what if the file already exists on the other machine?
	printf("Client says: %s",CONFIRMED);

	int fd=open(fileName, O_RDONLY);
	while((size=read(fd, fileBuff, 4096))>0){
		if(write(connect_fd, fileBuff, size)==-1){
			close(connect_fd);
			return;
		}
	}
	printf("Transfer complete.\n");
	close(connect_fd);
	return;
}
