#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>

int main() {

	int testSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(testSocket < 0) {
		printf("socket failed\n");
		close(testSocket);
		return 1;
	}

	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(8080);
	sockaddr.sin_addr.s_addr = INADDR_ANY;

	int bindresult = bind(testSocket, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
	if(bindresult < 0) {
		printf("bind failed\n");
		close(testSocket);
		return 1;
	}
	printf("Port: 8080\n");

	int client;
	int listenResult = listen(testSocket, 10);
	if(listenResult < 0) {
		printf("listen error");
		close(testSocket);
		return 1;
	}
	while(1) {

		client = accept(testSocket, 0, 0);
		if(client < 0) {
			printf("client accept error");
			close(testSocket);
			return 1;
		}
	
		char buffer[512] = {0};
		recv(client, buffer, 512, 0);
		printf("%s\n", buffer);

		char* fileName = buffer + 5;
		*strchr(fileName, ' ') = 0;

		FILE* openedFile = fopen(fileName, "r");
		if(openedFile == NULL) {
			printf("404 not found");
			send(client, "HTTP/1.1 404 Not Found\r\n", 24, 0);
			close(client);
			continue;
		}

		int fileSize;
		fseek(openedFile, 0, SEEK_END);
		fileSize = ftell(openedFile);
		fseek(openedFile, 0, SEEK_SET);

		char* fileContent = (char *)malloc(fileSize + 1);
		int bytesRead = fread(fileContent, 1, fileSize, openedFile);
		fileContent[bytesRead] = '\0';

		char response[1024]; 
		snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n"
							"Content-Type: text/html\r\n"
							"Content-Length: %d\r\n"
							"\r\n"
							"%s\r\n", fileSize ,fileContent); 

		printf("sending response\n");			
		send(client, response, strlen(response), 0);

		free(fileContent);
		fclose(openedFile);
		close(client);
	}

	close(testSocket);
	printf("stopped\n");
	return 0;
}
