#include "socket.h"
int g_sockfd=0;   //socket文件描述符

//向服务器发送json对象
void socket_send_data(struct json_object *j){
	char msg[1024]={0};
	const char *s=json_object_to_json_string(j);   //将json对象转化成字符串
	int len=strlen(s);
	memcpy(msg,&len,sizeof(int));
	memcpy(msg+sizeof(int),s,len);
	if(send(g_sockfd,msg,sizeof(int)+len,0)==-1){
		perror("send");
	}
}
