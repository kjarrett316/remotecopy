//Kaitlyn Jarrett
//Lab 4 client
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
#include <libgen.h>


#define PORT 3060
#define MSG "<remcp>\n"
#define SECRET "<CS30615spr>\n"
#define MATCH "<ok>\n"
#define READY "<ready>\n"
#define ERROR "<error>\n"
#define CONFIRMED "<send>\n"

int main(int argc, char *argv[])
{
	if(argc!=2){
		fprintf(stderr,"Usage: ./remcp HOST:FILE");
		return EXIT_FAILURE;
	}
	char *server_ip=strtok(argv[1],":");
	char *filepath=strtok(NULL,":");

	int sock_fd;
	struct sockaddr_in servaddr;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	inet_aton(server_ip,&servaddr.sin_addr);

	connect(sock_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	if(errno){
		fprintf(stderr, "Error in creating socket.\n");
		return EXIT_FAILURE;
	}

	char buff[strlen(MSG)];
	char buff2[strlen(SECRET)];
	char buff3[strlen(ERROR)];
	char fileBuff[4096];
	int size;

	if(write(sock_fd,MSG,strlen(MSG)+1)==-1){
		close(sock_fd);
		return EXIT_FAILURE;
	}

	if(read(sock_fd, buff, strlen(MSG)+1)<=0||strcmp(buff,MSG)!=0)
	{
		printf("Wrong message: %s. Connection will close.\n", buff);
		close(sock_fd);
		return EXIT_FAILURE;
	}

	printf("Server says: %s",MSG);

	if(write(sock_fd, SECRET, strlen(SECRET)+1)==-1){
		close(sock_fd);
		return EXIT_FAILURE;
	}

	if(read(sock_fd, buff2, strlen(MATCH)+1)<=0||strcmp(buff2,MATCH)!=0)
	{
		printf("Wrong message: %s. Connection will close.\n", buff2);
		close(sock_fd);
		return EXIT_FAILURE;
	}
	printf("Server says: %s",MATCH);

	if(write(sock_fd, filepath, strlen(filepath)+1)==-1){
		close(sock_fd);
		return EXIT_FAILURE;
	}

	if(read(sock_fd, buff3, strlen(READY)+1)<=0||strcmp(buff3,READY)!=0)
	{
		printf("Wrong message: %s. Connection will close.\n", buff2);
		close(sock_fd);
		return EXIT_FAILURE;
	}

	printf("Server says: %s",READY);

	if(write(sock_fd, CONFIRMED, strlen(CONFIRMED)+1)==-1){
		close(sock_fd);
		return EXIT_FAILURE;
	}

	int fd;
	char *fileName=basename(filepath);
	if((fd=open(fileName, O_WRONLY|O_CREAT|O_EXCL, 0600))<=0){
		printf("File, %s, already exists in this directory.\n",fileName);
		close(sock_fd);
		return EXIT_FAILURE;
	}

	printf("Ready to recieve file.\n");
	while((size=read(sock_fd, fileBuff, 4096))>0){
		if(write(fd, fileBuff, size)==-1)
		{
			close(sock_fd);
			return EXIT_FAILURE;
		}
	}
	printf("File recieved.\n");
	close(sock_fd);
	return EXIT_SUCCESS;
}
