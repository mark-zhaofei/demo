#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 1024
char *client_path = "client.socketB";
char *server_path = "server.socket";


static void handle_recv_msg(int sockfd, char *buf)
{
printf("client recv msg is:%s\n", buf);
sleep(5);
write(sockfd, buf, strlen(buf) +1);
}

static void handle_connection(int sockfd)
{
char sendline[MAXLINE],recvline[MAXLINE];
int maxfdp,stdineof;
fd_set readfds;
int n;
struct timeval tv;
int retval = 0;

while (1) {

FD_ZERO(&readfds);
FD_SET(sockfd,&readfds);
maxfdp = sockfd;

tv.tv_sec = 5;
tv.tv_usec = 0;

retval = select(maxfdp+1,&readfds,NULL,NULL,&tv);

if (retval == -1) {
return ;
}

if (retval == 0) {
printf("client timeout.\n");
continue;
}

if (FD_ISSET(sockfd, &readfds)) {
n = read(sockfd,recvline,MAXLINE);
if (n <= 0) {
fprintf(stderr,"client: server is closed.\n");
close(sockfd);
FD_CLR(sockfd,&readfds);
return;
}

handle_recv_msg(sockfd, recvline);
}
}
}

int main(int argc,char *argv[])
{
int sockfd;
int len;
struct  sockaddr_un cliun, serun;

if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
	perror("client socket error");
	exit(1);
}

// 一般显式调用bind函数，以便服务器区分不同客户端
  memset(&cliun, 0, sizeof(cliun));
	cliun.sun_family = AF_UNIX;
	strcpy(cliun.sun_path, client_path);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(cliun.sun_path);
	unlink(cliun.sun_path);

	if (bind(sockfd, (struct sockaddr *)&cliun, len) < 0) {
			perror("bind error");
			exit(1);
	}

	memset(&serun, 0, sizeof(serun));
	serun.sun_family = AF_UNIX;
	strcpy(serun.sun_path, server_path);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(serun.sun_path);
	if (connect(sockfd, (struct sockaddr *)&serun, len) < 0){
		perror("connect error");
		return -1;
	}



printf("client send to server .\n");
write(sockfd, "MDM_POWER_OFF", 32);

handle_connection(sockfd);

return 0;
}
