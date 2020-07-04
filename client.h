#ifndef CLIENT_H
#define CLIENT_H

#define SERVICE_IP              ("192.168.0.105")
#define SERVICE_PORT            (6000)

#define READ                    (1 << 1)
#define WRITE                   (1 << 2) 

#define READ_REQUEST            (12)
#define READ_RESPOND            (76)
#define READ_FAULT              (140)

#define WRITE_REQUEST           (13)
#define WRITE_RESPOND           (77)
#define WRITE_FAULT             (141)

#define EVENT_NOTIFY_REQUEST    (15)

#define RECV_BUF_SIZE           (100)

#define ACTIVE_MSG_TIME         (5000)

typedef struct _Stu_Info {
	unsigned char ucStuName[10];
	unsigned char ucStuLabNum[10];
}Stu_Info;

typedef struct _Msg_Header {
	unsigned char  ucServiceID;
	unsigned short usLength;
}Msg_Header;

typedef struct _Read_Req_Msg {
	Msg_Header     sHeader;
	unsigned char  ucObjectID;
}Read_Req_Msg;

typedef struct _Read_Resp_Msg {
	Msg_Header    header;
	Stu_Info      stuData;
}Read_Resp_Msg;

typedef struct _Write_Req_Msg {
	Msg_Header    sHeader;
	unsigned char ucObjectID;
	Stu_Info      stuToWrite;
}Write_Req_Msg;

typedef struct _Write_Resp_Msg {
	Msg_Header     sHeader;
	Stu_Info       stuData;
}Write_Resp_Msg;

typedef struct _Event_t {
	Msg_Header     sHeader;
	unsigned char  ucEventNum;
	Stu_Info       stuData;
}Event_t;


#endif // !CLIENT_H
