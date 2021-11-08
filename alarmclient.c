#include "stems.h"

void getargs_pc(char* hostname, int* port, char* filename, char* name, float* threshold)
{
	FILE* fp;

	fp = fopen("config-pc.txt", "r"); //config-pc���� �Ӱ谪�� �о�´�.
	if (fp == NULL)
		unix_error("config-pc.txt file does not open.\n");


	fscanf(fp, "%s", hostname);
	fscanf(fp, "%d", port);
	fscanf(fp, "%s", filename);
	fscanf(fp, "%s", name);
	fscanf(fp, "%f", threshold);
	fclose(fp);
}


void parseData(char* data, char* sensorname, char* time, float* sensorValue) {
	/*�޾ƿ� ���� �̸�, �ð�, ������ �����ϴ� �Լ�*/
	strtok(data, "="); //�̸� ����
	sprintf(sensorname, "%s", strtok(NULL, "&"));

	strtok(NULL, "=");  //�ð� ����
	sprintf(time, "%s", strtok(NULL, "&"));

	strtok(NULL, "=");  //�� ����
	*sensorValue = atof(strtok(NULL, "&"));
}

void clientSend(int fd, char* filename, char* body, int port)
{
	char buf[MAXLINE];
	char hostname[MAXLINE];

	Gethostname(hostname, MAXLINE);

	sprintf(buf, "POST %s HTTP/1.1\r\n", filename);
	sprintf(buf, "%sHost: %s:%d\r\n", buf, hostname, port);
	sprintf(buf, "%sConnection: keep-alive\r\n", buf);
	sprintf(buf, "%sContent-Length: %d\r\n", buf, (int)strlen(body));
	sprintf(buf, "%s\r\n", buf);
	sprintf(buf, "%s%s", buf, body);
	Rio_writen(fd, buf, strlen(buf));
}

void clientPrint(int fd)
{
	rio_t rio;
	char buf[MAXBUF];
	int length = 0;
	int n;

	Rio_readinitb(&rio, fd);

	/* Read and display the HTTP Header */
	n = Rio_readlineb(&rio, buf, MAXBUF);
	while (strcmp(buf, "\r\n") && (n > 0)) {
		n = Rio_readlineb(&rio, buf, MAXBUF);

		/* If you want to look for certain HTTP tags... */
		if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
		}
	}

	/* Read and display the HTTP Body */
	n = Rio_readlineb(&rio, buf, MAXBUF);
	while (n > 0) {
		printf("%s", buf);
		n = Rio_readlineb(&rio, buf, MAXBUF);
	}
}

void userTask(char* hostname, int port, char* filename, char* body)
{
	int clientfd;
	clientfd = Open_clientfd(hostname, port);
	clientSend(clientfd, filename, body, port);
	clientPrint(clientfd);
	Close(clientfd);
}

int main(void) {
	char hostname[MAXLINE], filename[MAXLINE], name[MAXLINE];
	int port;
	float threshold;

	int pipe;

	getargs_pc(hostname, &port, filename, name, &threshold);

	while (1) {
		while ((pipe = open(FIFO, O_RDWR)) == -1) {} //FIFO ����

		sleep(1);

		int len, nread;
		char data[MAXLINE];

		while ((nread = Read(pipe, &len, sizeof(int))) < 0) {} //���� ���̸� ���� �д´�
		while ((nread = Read(pipe, data, len)) < 0) {} //���� �о�´�
		Close(pipe);
		unlink(FIFO);  //FIFO ����

		char sensorname[MAXLINE]; //���� �̸�
		char time[MAXLINE];  //�ð�
		char sendData[MAXLINE]; //alarm���� ���� ��ü ��
		float sensorValue; //���� ��

		strcpy(sendData, data);  //���� ���� sendData�� ������
		parseData(data, sensorname, time, &sensorValue);  //���� ����
		if (strcmp(name, sensorname) == 0) { //config-pc�� ���� �̸��� �������� ���ٸ�
			if (sensorValue > threshold) { //���� ���� �Ӱ谪���� ũ�ٸ�
				userTask(hostname, port, filename, sendData); //��ü ���� �����ش�.
			}
		}
	}
	return 0;
}
