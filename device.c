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
	//初始化蜂鸣器
	g_buzzerfd=open("/dev/pwm",O_WRONLY);
	if(g_buzzerfd==-1){
		perror("open pwm");
		return -1;
	}
	g_maxfd=(g_maxfd<g_buzzerfd) ? g_buzzerfd : g_maxfd;
	printf("蜂鸣器初始化成功\n");

#endif
	return 0;	
}

//启动蜂鸣器
void start_buzzer(){
	int freq=1000;
	int ret=ioctl(g_buzzerfd,1,freq);   //启动蜂鸣器,向设备发送命令1和频率值
	if(-1==ret){
		perror("ioctl");
		return;
	}
	usleep(500000);
	//关闭蜂鸣器,向设备发送命令0
	ret=ioctl(g_buzzerfd,0);
	if(-1==ret){
		perror("ioctl");
	}
	
}

//获开发板按键的id
int get_key_id(){
	char buttons[6]={'0','0','0','0','0','0'};   //一开始初始化所有按键未被按下
	char cur_buttons[6]={0};    //当前按键的状态
	size_t size=read(g_buttonfd,cur_buttons,6);
	if(size==-1){
		perror("read");
		return -1;
	}
	int i;
	for(i=0;i<6;i++){
		if(buttons[i]!=cur_buttons[i]){
			return i+1;
		}
	}    //找到状态发生改变的按键
	return -1;


}

//初始化串口设备
int init_serial(){
	struct termios TtyAttr;
	memset(&TtyAttr,0,sizeof(struct termios));
	TtyAttr.c_flag=IGNPAR;
	TtyAttr.c_cflag = B115200 | HUPCL | CS8 | CREAD | CLOCAL;
	TtyAttr.c_cc[VMIN]=1;
	if(tcsetattr(g_serialfd,TCSANOW,&TtyAttr)==-1){
		perror("tcsetattr");
		return -1;
	}
}

