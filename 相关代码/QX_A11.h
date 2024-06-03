#ifndef _QX_A11_H
#define _QX_A11_H


#define MAIN_Fosc		12000000L	//定义主时钟
#define Main_Fosc_KHZ	(MAIN_Fosc / 1000)
#define BAUD 9600 //UART 波特率


/*电机驱动IO定义*/
sbit Left_moto_pwm = P1^5; //为1 左电机使能
sbit IN1 = P1^4; //为1 左电机正转
sbit IN2 = P1^3; //为1 左电机反转
sbit IN3 = P1^2; //为1 右电机反转
sbit IN4 = P1^1; //为1 右电机正转
sbit Right_moto_pwm = P1^0; //为1 右电机使能

sbit left_led1 = P3^6;//左寻迹信号 为0 没有识别到黑线 为1识别到黑线
sbit right_led1 = P3^5;//右寻迹信号	为0 没有识别到黑线 为1识别到黑线
sbit left_led2 = P3^7;//左避障信号 为0 识别障碍物 为1没有识别到障碍物
sbit right_led2 = P3^4;//右避障信号	为0 识别障碍物 为1没有识别到障碍物
sbit LED1 = P1^6;//LED1状态指示灯
sbit LED2 = P1^7;//LED2状态指示灯
sbit IR  = P3^2;     //定义红外脉冲数据接口	外部中断O输入口
/*按键定义*/
sbit KEY = P3^3;
sbit beep = P2^2;//蜂鸣器

/*舵机接口*/
sbit ServoPin = P2^3;
/*超声波接口*/
sbit EchoPin = P2^0; //超声波模块Echo	接收端
sbit TrigPin = P2^1; //超声波模块Trig	控制端

#define left_motor_en		EN1 = 1	//左电机使能
#define right_motor_en		EN2 = 1	//右电机使能


#define left_motor_stops	IN1 = 0, IN2 = 0//左电机停止
#define right_motor_stops	IN3 = 0, IN4 = 0//右电机停止

#define left_motor_go		IN1 = 1, IN2 = 0//左电机正传
#define left_motor_back		IN1 = 0, IN2 = 1//左电机反转
#define right_motor_go		IN3 = 0, IN4 = 1//右电机正传
#define right_motor_back	IN3 = 1, IN4 = 0//右电机反转

#define Timer1On			TR1 = 1			//启动定时器1
#define Timer1Off			TR1 = 0			//关闭定时器1
#define EA_on				EA = 1			//开始总中断
#define EA_off				EA = 0			//关闭总中断

#endif