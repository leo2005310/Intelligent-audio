#include "device.h"
extern int g_maxfd;
extern fd_set READSET;
int g_mixerfd=0;
int g_buttonfd=0;
int g_serialfd=0;
int g_buzzerfd=0;
//初始化硬件设备
int init_device(){
	//初始化音频设备
	g_mixerfd=open("/dev/mixer",O_RDWR);
	if(g_mixerfd==-1){
		perror("open mixer");
		return -1;
	}
	g_maxfd=(g_maxfd<g_mixerfd) ? g_mixerfd : g_maxfd;  //更新最大文件描述符
#ifdef ARM
	//在开发板上运行
	//初始化按键
	g_buttonfd=open("/dev/buttons",O_RDONLY);
	if(g_buttonfd==-1){
		perror("open buttons");
		return -1;
	}
	g_maxfd=(g_maxfd<g_buttonfd) ? g_butttonfd : g_maxfd;
	//加入集合用于监听
	FD_SET(g_buttonfd,&READSET);
	printf("初始化按键成功\n");
	//初始化串口
	g_serialfd=open("/dev/s3c2410_serial1",O_RDONLY);
	if(g_serialfd==-1){
		perror("open serial");
		return -1;
	}
	if(init_serial()==-1){
		printf("初始化串口失败\n");
		return -1;
	}
	g_maxfd=(g_maxfd<g_serialfd) ? g_serialfd : g_maxfd;
	//加入集合中用于监听 
	FD_SET(g_serialfd,&READSET);
	printf("初始化串口成功\n");


#endif
	
}
