#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include "cJSON.h"
#include "data.h"

#define MYPORT  80
#define QUEUE   20
#define BUFFER_SIZE 1024


data_t sys;


//---------------------------------------------------------------------
void getImg(int newsock,const char* file)
{
	int len,sumlen;
	FILE *fp = fopen(file, "rb");
	if (fp == NULL) {
		printf("failed to open %s\n",file); 
		fclose(fp);
		return;
	}
	fseek(fp, 0, SEEK_END);
	long fileLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char buf[4096] = { 0 };
	memset(buf, '\0', 4096);
	strcat(buf, "HTTP/1.0 200 OK\r\n");
	strcat(buf, "Content-Type: ");
	strcat(buf, getHeadType(file));
	strcat(buf, ";charset=UTF-8\r\n");

	char contentLength[256] = { 0 };
	sprintf(contentLength, "Content-Length: %ld\r\n\r\n", fileLen);
	strcat(buf, contentLength);
	send(newsock, buf, strlen(buf), 0);
	printf("%s\n", buf);
	sumlen = 0;
	while (true)
	{
		memset(buf, '\0', 4096);
		len = fread(buf,1, 4096, fp);
		send(newsock, buf, len, 0);

		sumlen += len;
		if (sumlen >= fileLen - 1) break;
		fseek(fp, sumlen, SEEK_SET);				
	}
	fclose(fp);
	close(newsock);
}
//-------------------------------------------------------------------------------
const char* getHeadType(const char* filepath)
{
	const char* extname = NULL;
	const char* content = "text/plain";
	if ((extname = strrchr(filepath, '.')) != NULL)
	{
		extname++;
		if (strcmp(extname, "html") == 0 || strcmp(extname, "htm") == 0) content = "text/html";
		else if (strcmp(extname, "css") == 0) content = "text/css";
		else if (strcmp(extname, "gif") == 0) content = "image/gif";
		else if (strcmp(extname, "jpeg") == 0 || strcmp(extname, "jpg") == 0) content = "image/jpeg";
		else if (strcmp(extname, "png") == 0) content = "image/png";
		else if (strcmp(extname, "ico") == 0) content = "image/x-icon";
	}
	return content;
}
//-------------------------------------------------------------------------------
void request(int newsock, char* buf)
{
	char txt[100] = { 0 };
	if (strncmp(buf, "GET ", 4) == 0) {
		char* p = buf + 4;
		memset(txt, '\0', 100);
		strcpy(txt, "./img");
		for (int i = 5; *p != ' '; i++) {
			txt[i] = *p++;
		}
		if (strcmp(txt, "./img/") == 0) strcpy(txt, "./img/index.html");
		printf("%s\n", txt);
		getImg(newsock, txt);
	}
}
//------------------------------------------------------------------------------
int main(void)
{
	int fd;
//	if (wiringPiSetup() < 0)return 1;
//	if ((fd = serialOpen("/dev/ttyAMA0", 115200)) < 0)
	/*{
		printf("error\n");
		return 1;
	}*/
	///定义sockfd
	int server_sockfd = socket(AF_INET, SOCK_STREAM, 0); //IPPROTO_TCP
	if (server_sockfd < 0) {
		printf("socket error\n");
		return 1;
	}

	///定义sockaddr_in
	struct sockaddr_in server_sockaddr;
//	memset(&server_sockaddr, 0, sizeof(server_sockaddr));
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(MYPORT);
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	///bind，成功返回0，出错返回-1
	if (bind(server_sockfd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) == -1)
	{
		perror("bind");
		printf("bind error\n");
		return 1;
	}
	listen(server_sockfd, 100);
	printf("lisening..\n");


	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);

	///成功返回非负描述字，出错返回-1
//	int newsock = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);

	char buf[1024] = { 0 }, txt[50] = { 0 };
	char* p = buf;
	int newsock;
	while (1) 
	{
		newsock = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
		if (newsock < 0) {
			printf("newsock errror\n");
			return 1;
		}
		if ((recv(newsock, buf, 1024, 0)) > 0)
		{
			printf("%s\n", buf);
			request(newsock, buf);
		}
		
		

	}
	


//	getImg(newsock, "/img2.png");

	close(server_sockfd);

	//serialClose(fd);
	printf("OK\n");
	return 0;
}
//-------------------------------------------------------------------------------------
