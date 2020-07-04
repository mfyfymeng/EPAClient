#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "client.h"

#pragma comment (lib, "WS2_32.lib")

#pragma warning (disable: 4996)

void vErrorHandling(char* message);
void vPrintMyInfo(void);

Msg_Header         sHeader;
Msg_Header*        psHeader = &sHeader;
Read_Req_Msg       sReadReqMsg;
Read_Req_Msg*      psReadReqMsg = &sReadReqMsg;
Read_Resp_Msg      sReadRespMsg;
Read_Resp_Msg*     psReadRespMsg = &sReadRespMsg;
Write_Req_Msg      sWriteReqMsg;
Write_Req_Msg*     psWriteReqMsg = &sWriteReqMsg;
Write_Resp_Msg     sWriteRespMsg;
Write_Resp_Msg*    psWriteRespMsg = &sWriteRespMsg;
Event_t            sEvent;
Event_t*           psEvent = &sEvent;

void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime) {
	printf("TIME OUT!\n");
}

int main(void) {
	WSADATA        wsaData;
	SOCKET         cSocket;
	SOCKADDR_IN    servAddr;
	socklen_t      addrSize;
	
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		vErrorHandling("WSAStartup() error!");
		
	cSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (cSocket == INVALID_SOCKET)
		vErrorHandling("socket() error!");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = inet_addr(SERVICE_IP);
	servAddr.sin_port = htons(SERVICE_PORT);

	addrSize = sizeof(servAddr);

	unsigned char  ucObjectID;
	int            flag = 0;
	int            recv = 0;
	Stu_Info       stuInfo[] = { {"姓名", "科室"} };
	Stu_Info       stuToWrite[] = { {"新名字", "新科室"} };
	char           recvBuf[RECV_BUF_SIZE];
	MSG            msg;
	
	vPrintMyInfo();
	
	while (1) {
		printf("\n读操作请输入R，写操作请输入W，请输入：");

		char cCmd = getchar();
		if ((cCmd == 'R') || (cCmd == 'r'))
			flag = READ;
		else if ((cCmd == 'W') || (cCmd == 'w'))
			flag = WRITE;
		else {
			vErrorHandling("Service error!");
		}

		printf("\n请输入学生代号：");
		scanf("%s", &ucObjectID);

		fflush(stdin);
		getchar();
		printf("\n您输入的学生代号为：%c\n", ucObjectID);

		if (flag == READ) {
			psReadReqMsg->sHeader.ucServiceID = READ_REQUEST;
			psReadReqMsg->sHeader.usLength = sizeof(psReadReqMsg->sHeader) + sizeof(unsigned char);
			psReadReqMsg->ucObjectID = ucObjectID;

			sendto(cSocket, (const char*)psReadReqMsg, psReadReqMsg->sHeader.usLength, 0, (struct sockaddr*)&servAddr, sizeof(servAddr));

			recvfrom(cSocket, recvBuf, RECV_BUF_SIZE, 0, (struct sockaddr*)&servAddr, &addrSize);

			memcpy(&psReadRespMsg->header, recvBuf, sizeof(psReadRespMsg->header));
			if (psReadRespMsg->header.ucServiceID == READ_RESPOND) {
				KillTimer(NULL, 3);
				memcpy(psReadRespMsg, recvBuf, psReadRespMsg->header.usLength);
				memcpy(&stuInfo, &psReadRespMsg->stuData, sizeof(Stu_Info));
				printf("姓名：%s，科室：%s\n", stuInfo->ucStuName, stuInfo->ucStuLabNum);
			}
			else if (psReadRespMsg->header.ucServiceID == READ_FAULT) {
				KillTimer(NULL, 3);
				memcpy(psReadRespMsg, recvBuf, psReadRespMsg->header.usLength);
				printf("%s：%s\n", psReadRespMsg->stuData.ucStuName, psReadRespMsg->stuData.ucStuLabNum);
			}
			else {
				vErrorHandling("Error!");
			}
		}
		else if (flag == WRITE) {
			psWriteReqMsg->sHeader.ucServiceID = WRITE_REQUEST;
			psWriteReqMsg->sHeader.usLength = sizeof(psWriteReqMsg->sHeader) + sizeof(unsigned char) + sizeof(Stu_Info);
			psWriteReqMsg->ucObjectID = ucObjectID;
			memcpy(&psWriteReqMsg->stuToWrite, (const void*)stuToWrite, sizeof(Stu_Info));
			
			sendto(cSocket, (const char*)psWriteReqMsg, psWriteReqMsg->sHeader.usLength, 0, (struct sockaddr*)&servAddr, sizeof(servAddr));

			SetTimer(NULL, 1, 5000, &TimerProc);
			
			//recvfrom(cSocket, recvBuf, RECV_BUF_SIZE, 0, (struct sockaddr*)&servAddr, &addrSize);
			if (recvfrom(cSocket, recvBuf, RECV_BUF_SIZE, 0, (struct sockaddr*)&servAddr, &addrSize) != 0) {
				DispatchMessage(&msg);
			}
			
			memcpy(&psWriteRespMsg->sHeader, recvBuf, sizeof(psWriteRespMsg->sHeader));
			if (psWriteRespMsg->sHeader.ucServiceID == WRITE_RESPOND) {
				KillTimer(NULL, 1);
				memcpy(psWriteRespMsg, recvBuf, psWriteRespMsg->sHeader.usLength);
				memcpy(&stuInfo, &psWriteRespMsg->stuData, sizeof(Stu_Info));
				printf("\n修改成功！\n姓名：%s，科室：%s\n", stuInfo->ucStuName, stuInfo->ucStuLabNum);
			}
			else if (psWriteRespMsg->sHeader.ucServiceID == WRITE_FAULT) {
				KillTimer(NULL, 1);
				memcpy(psWriteRespMsg, recvBuf, psWriteRespMsg->sHeader.usLength);
				printf("%s：%s\n", psWriteRespMsg->stuData.ucStuName, psWriteRespMsg->stuData.ucStuLabNum);
			}
			else {
				vErrorHandling("Error!");
			}
		}
		else {
			vErrorHandling("Error!");
		}
	}
	
	closesocket(cSocket);
	return 0;
}

void vErrorHandling(char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void vPrintMyInfo(void) {
	printf("姓名：孟凡宇\n");
	printf("科室：八室\n");
	printf("学号：201928014719016\n");
	printf("=====================\n");
}

