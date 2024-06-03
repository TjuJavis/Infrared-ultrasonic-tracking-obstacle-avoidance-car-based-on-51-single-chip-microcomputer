#include <reg52.h>//51头文件
#include <QX_A11.h>//QX_A11智能小车配置文件
#include <intrins.h>
bit Timer1Overflow;	//计数器1溢出标志位
unsigned char disbuff[4]={0,0,0,0};//用于分别存放距离的值米，厘米，毫米
unsigned int LeftDistance = 0, RightDistance = 0, FrontDistance = 0; //云台测距距离缓存
unsigned int DistBuf[5] = {0};//distance data buffer
unsigned char	pwm_val_left,pwm_val_right;	//中间变量，用户请勿修改。
unsigned char 	pwm_left,pwm_right;			//定义PWM输出高电平的时间的变量。用户操作PWM的变量。
#define		PWM_DUTY		200			//（用于舵机时不可修改）定义PWM的周期，数值为定时器0溢出周期，假如定时器溢出时间为100us，则PWM周期为20ms。
#define		PWM_HIGH_MIN	20			//限制PWM输出的最小占空比。用户请勿修改。
#define		PWM_HIGH_MAX	PWM_DUTY	//限制PWM输出的最大占空比。用户请勿修改。

void Timer0_Init(void); //定时器0初始化
void Timer1_Init(void);//定时器1初始化
void LoadPWM(void);//装入PWM输出值 
void Delay_Ms(unsigned int ms);//毫秒级延时函数
void forward(unsigned char LeftSpeed,unsigned char RightSpeed);//QX_A11智能小车前进 
void left_run(unsigned char LeftSpeed,unsigned char RightSpeed);//QX_A11智能小车左转  
void right_run(unsigned char LeftSpeed,unsigned char RightSpeed);//QX_A11智能小车右转
//void back_run(unsigned char LeftSpeed,unsigned char RightSpeed);//QX_A11智能小车后退
void stop(void);//QX_A11智能小车停车
unsigned int QXMBOT_GetDistance(void);//获取超声波距离
void QXMBOT_TrigUltrasonic(void);// 触发超声波
void Tracking();//黑线循迹函数
/*主函数*/     
void main(void)
{
	unsigned int Dis;//距离暂存变量
	unsigned int i;
	Timer0_Init();//定时0初始化
	Timer1_Init();//定时0初始化
	EA_on;	//开中断
	while(1)
	{
		  
		for (i = 0; i < 1000;i++) Tracking();//循环执行黑线循迹达到延时执行读取超声波距离的效果，超声波测距间隔需要大于60ms
		  Dis = QXMBOT_GetDistance();//获取超声波测距距离,单位：毫米
			if(Dis < 200)//毫米
			{	
				do{
					stop();	//停车
					Delay_Ms(100);
					Dis = QXMBOT_GetDistance();//获取超声波测距距离,单位：毫米
				}while(Dis < 200);
			}
	}	
}


/*********************************************
QX_A11智能小车前进
入口参数：LeftSpeed，RightSpeed
出口参数: 无
说明：LeftSpeed，RightSpeed分别设置左右车轮转速
**********************************************/
void forward(unsigned char LeftSpeed,unsigned char RightSpeed)
{
	pwm_left = LeftSpeed,pwm_right =  RightSpeed;//设置速度
	left_motor_go; //左电机前进
	right_motor_go; //右电机前进
}
/*小车左转*/
/*********************************************
QX_A11智能小车左转
入口参数：LeftSpeed，RightSpeed
出口参数: 无
说明：LeftSpeed，RightSpeed分别设置左右车轮转速
**********************************************/
void left_run(unsigned char LeftSpeed,unsigned char RightSpeed)
{
	pwm_left = LeftSpeed,pwm_right =  RightSpeed;//设置速度
	left_motor_back; //左电机后退
	right_motor_go; //右电机前进	
}

/*********************************************
QX_A11智能小车右转
入口参数：LeftSpeed，RightSpeed
出口参数: 无
说明：LeftSpeed，RightSpeed分别设置左右车轮转速
**********************************************/
void right_run(unsigned char LeftSpeed,unsigned char RightSpeed)
{
	pwm_left = LeftSpeed,pwm_right =  RightSpeed;//设置速度
	right_motor_back;//右电机后退
	left_motor_go;    //左电机前进
}
/*********************************************
QX_A11智能小车后退
入口参数：LeftSpeed，RightSpeed
出口参数: 无
说明：LeftSpeed，RightSpeed分别设置左右车轮转速
**********************************************/
//void back_run(unsigned char LeftSpeed,unsigned char RightSpeed)
//{
//	pwm_left = LeftSpeed,pwm_right =  RightSpeed;//设置速度
//	right_motor_back;//右电机后退
//	left_motor_back; //左电机后退
//}
/*********************************************
QX_A11智能小车停车
入口参数：无
出口参数: 无
说明：QX_A11智能小车停车
**********************************************/
void stop(void)
{
	left_motor_stops;
	right_motor_stops;
}

/*====================================
函数：void Delay_Ms(INT16U ms)
参数：ms，毫秒延时形参
描述：12T 51单片机自适应主时钟毫秒级延时函数
====================================*/
void Delay_Ms(unsigned int ms)
{
     unsigned int i;
	 do{
	      i = MAIN_Fosc / 96000; 
		  while(--i);   //96T per loop
     }while(--ms);
}
/*********************************************
QX_A11智能小车PWM输出
入口参数：无
出口参数: 无
说明：装载PWM输出,如果设置全局变量pwm_left,pwm_right分别配置左右输出高电平时间
**********************************************/
void LoadPWM(void)
{
	if(pwm_left > PWM_HIGH_MAX)		pwm_left = PWM_HIGH_MAX;	//如果左输出写入大于最大占空比数据，则强制为最大占空比。
	if(pwm_left < PWM_HIGH_MIN)		pwm_left = PWM_HIGH_MIN;	//如果左输出写入小于最小占空比数据，则强制为最小占空比。
	if(pwm_right > PWM_HIGH_MAX)	pwm_right = PWM_HIGH_MAX;	//如果右输出写入大于最大占空比数据，则强制为最大占空比。
	if(pwm_right < PWM_HIGH_MIN)	pwm_right = PWM_HIGH_MIN;	//如果右输出写入小于最小占空比数据，则强制为最小占空比。
	if(pwm_val_left<=pwm_left)		Left_moto_pwm = 1;  //装载左PWM输出高电平时间
	else Left_moto_pwm = 0; 						    //装载左PWM输出低电平时间
	if(pwm_val_left>=PWM_DUTY)		pwm_val_left = 0;	//如果左对比值大于等于最大占空比数据，则为零

	if(pwm_val_right<=pwm_right)	Right_moto_pwm = 1; //装载右PWM输出高电平时间
	else Right_moto_pwm = 0; 							//装载右PWM输出低电平时间
	if(pwm_val_right>=PWM_DUTY)		pwm_val_right = 0;	//如果右对比值大于等于最大占空比数据，则为零
}
/*超声波触发*/
void QXMBOT_TrigUltrasonic()
{
	TrigPin = 0; //超声波模块Trig	控制端
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	TrigPin = 1; //超声波模块Trig	控制端
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();
	TrigPin = 0; //超声波模块Trig	控制端
}
/*====================================
函数名	：QXMBOT_GetDistance
参数	：无
返回值	：获取距离单位毫米
描述	：超声波测距
通过发射信号到收到回响信号的时间测试距离
单片机晶振11.0592Mhz
注意测距周期为60ms以上
====================================*/
unsigned int QXMBOT_GetDistance()
{
	unsigned int Distance = 0;	//超声波距离
	unsigned int  Time=0;		//用于存放定时器时间值
	QXMBOT_TrigUltrasonic();	//超声波触发
	while(!EchoPin);  	//判断回响信号是否为低电平
	Timer1On;			//启动定时器1
	while(EchoPin);		//等待收到回响信号
	Timer1Off;			//关闭定时器1
	Time=TH1*256+TL1;	//读取时间
	TH1=0;
	TL1=0;				//清空定时器
    Distance = (float)(Time*1.085)*0.17;//算出来是MM
	return(Distance);//返回距离				
}

void Tracking()//黑线循迹函数
{
	char data1, data2 = right_led2, data3 = right_led1, data4 = left_led1, data5 = left_led2;
	data1 = data2 * 1000 + data3 * 100 + data4 * 10 + data5;

	switch (data1)
	{
	case 0: // 在黑线上，前进
	case 1001:
		forward(39, 37); // 前进
		break;
	case 100: // 小幅偏右，左转
		left_run(37, 40);
		while (1)
		{
			if (/*data1 == 0 || */data1 == 100 || data1 == 10) break;
		}
		//left_run(31, 35); // 左转
		//forward(32, 31);
		break;
	case 10: // 小幅偏左，右转
		right_run(42, 37);
		while (1)
		{
			if (/*data1 == 0 || */data1 == 10 || data1 == 100) break;
		}
		//right_run(42, 31); // 右转
		//forward(52, 41);
		break;
	case 1100: // 大幅偏右，左转
		
		//if (data1 == 1110 || data1 == 1111 || data1 == 1001 || data1 == 110 || data1 == 1111) forward(40, 40);
		left_run(40, 50); // 左转
		break;
	case 1000: // 左急转弯
		//Delay_Ms(5);
		left_run(60, 60); // 左转
		break;
	case 1010: // 右急转弯
		left_run(50, 60); // 右转
		break;
	case 11: // 大幅偏左，右转
		
		right_run(52, 40);
		//right_run(62, 60);
		//right_run(44, 34); // 右转
		break;
	case 1: // 右急转弯
		
		right_run(60, 60); // 右转

		break;
	case 111:
	case 110:
	case 1110:
		left_run(60, 60);
		break;
	default:
		break;
	}
}

/********************* Timer0初始化************************/
void Timer0_Init(void)
{
	TMOD |= 0x02;//定时器0，8位自动重装模块
	TH0 = 0X9C;
	TL0 = 0X9C;//11.0592M晶振，12T溢出时间约等于100微秒
	TR0 = 1;//启动定时器0
	ET0 = 1;//允许定时器0中断	
}
/*定时器1初始化*/
void Timer1_Init(void)		
{
	TMOD |= 0x10;	//定时器1工作模式1,16位定时模式。T1用测ECH0脉冲长度
	TH1 = 0;		   
    TL1 = 0;
	ET1 = 1;             //允许T1中断
}
 
/********************* Timer0中断函数************************/
void timer0_int (void) interrupt 1
{
	 pwm_val_left++;
	 pwm_val_right++;
	 LoadPWM();//装载PWM输出
}
/* Timer0 interrupt routine */
void tm1_isr() interrupt 3 using 1
{
	Timer1Overflow = 1;	//计数器1溢出标志位
	EchoPin = 0;		//超声波接收端	
}	
