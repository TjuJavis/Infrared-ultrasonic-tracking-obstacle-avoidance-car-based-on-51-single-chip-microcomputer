#include <reg52.h>//51ͷ�ļ�
#include <QX_A11.h>//QX_A11����С�������ļ�
#include <intrins.h>
bit Timer1Overflow;	//������1�����־λ
unsigned char disbuff[4]={0,0,0,0};//���ڷֱ��ž����ֵ�ף����ף�����
unsigned int LeftDistance = 0, RightDistance = 0, FrontDistance = 0; //��̨�����뻺��
unsigned int DistBuf[5] = {0};//distance data buffer
unsigned char	pwm_val_left,pwm_val_right;	//�м�������û������޸ġ�
unsigned char 	pwm_left,pwm_right;			//����PWM����ߵ�ƽ��ʱ��ı������û�����PWM�ı�����
#define		PWM_DUTY		200			//�����ڶ��ʱ�����޸ģ�����PWM�����ڣ���ֵΪ��ʱ��0������ڣ����綨ʱ�����ʱ��Ϊ100us����PWM����Ϊ20ms��
#define		PWM_HIGH_MIN	20			//����PWM�������Сռ�ձȡ��û������޸ġ�
#define		PWM_HIGH_MAX	PWM_DUTY	//����PWM��������ռ�ձȡ��û������޸ġ�

void Timer0_Init(void); //��ʱ��0��ʼ��
void Timer1_Init(void);//��ʱ��1��ʼ��
void LoadPWM(void);//װ��PWM���ֵ 
void Delay_Ms(unsigned int ms);//���뼶��ʱ����
void forward(unsigned char LeftSpeed,unsigned char RightSpeed);//QX_A11����С��ǰ�� 
void left_run(unsigned char LeftSpeed,unsigned char RightSpeed);//QX_A11����С����ת  
void right_run(unsigned char LeftSpeed,unsigned char RightSpeed);//QX_A11����С����ת
//void back_run(unsigned char LeftSpeed,unsigned char RightSpeed);//QX_A11����С������
void stop(void);//QX_A11����С��ͣ��
unsigned int QXMBOT_GetDistance(void);//��ȡ����������
void QXMBOT_TrigUltrasonic(void);// ����������
void Tracking();//����ѭ������
/*������*/     
void main(void)
{
	unsigned int Dis;//�����ݴ����
	unsigned int i;
	Timer0_Init();//��ʱ0��ʼ��
	Timer1_Init();//��ʱ0��ʼ��
	EA_on;	//���ж�
	while(1)
	{
		  
		for (i = 0; i < 1000;i++) Tracking();//ѭ��ִ�к���ѭ���ﵽ��ʱִ�ж�ȡ�����������Ч�����������������Ҫ����60ms
		  Dis = QXMBOT_GetDistance();//��ȡ������������,��λ������
			if(Dis < 200)//����
			{	
				do{
					stop();	//ͣ��
					Delay_Ms(100);
					Dis = QXMBOT_GetDistance();//��ȡ������������,��λ������
				}while(Dis < 200);
			}
	}	
}


/*********************************************
QX_A11����С��ǰ��
��ڲ�����LeftSpeed��RightSpeed
���ڲ���: ��
˵����LeftSpeed��RightSpeed�ֱ��������ҳ���ת��
**********************************************/
void forward(unsigned char LeftSpeed,unsigned char RightSpeed)
{
	pwm_left = LeftSpeed,pwm_right =  RightSpeed;//�����ٶ�
	left_motor_go; //����ǰ��
	right_motor_go; //�ҵ��ǰ��
}
/*С����ת*/
/*********************************************
QX_A11����С����ת
��ڲ�����LeftSpeed��RightSpeed
���ڲ���: ��
˵����LeftSpeed��RightSpeed�ֱ��������ҳ���ת��
**********************************************/
void left_run(unsigned char LeftSpeed,unsigned char RightSpeed)
{
	pwm_left = LeftSpeed,pwm_right =  RightSpeed;//�����ٶ�
	left_motor_back; //��������
	right_motor_go; //�ҵ��ǰ��	
}

/*********************************************
QX_A11����С����ת
��ڲ�����LeftSpeed��RightSpeed
���ڲ���: ��
˵����LeftSpeed��RightSpeed�ֱ��������ҳ���ת��
**********************************************/
void right_run(unsigned char LeftSpeed,unsigned char RightSpeed)
{
	pwm_left = LeftSpeed,pwm_right =  RightSpeed;//�����ٶ�
	right_motor_back;//�ҵ������
	left_motor_go;    //����ǰ��
}
/*********************************************
QX_A11����С������
��ڲ�����LeftSpeed��RightSpeed
���ڲ���: ��
˵����LeftSpeed��RightSpeed�ֱ��������ҳ���ת��
**********************************************/
//void back_run(unsigned char LeftSpeed,unsigned char RightSpeed)
//{
//	pwm_left = LeftSpeed,pwm_right =  RightSpeed;//�����ٶ�
//	right_motor_back;//�ҵ������
//	left_motor_back; //��������
//}
/*********************************************
QX_A11����С��ͣ��
��ڲ�������
���ڲ���: ��
˵����QX_A11����С��ͣ��
**********************************************/
void stop(void)
{
	left_motor_stops;
	right_motor_stops;
}

/*====================================
������void Delay_Ms(INT16U ms)
������ms��������ʱ�β�
������12T 51��Ƭ������Ӧ��ʱ�Ӻ��뼶��ʱ����
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
QX_A11����С��PWM���
��ڲ�������
���ڲ���: ��
˵����װ��PWM���,�������ȫ�ֱ���pwm_left,pwm_right�ֱ�������������ߵ�ƽʱ��
**********************************************/
void LoadPWM(void)
{
	if(pwm_left > PWM_HIGH_MAX)		pwm_left = PWM_HIGH_MAX;	//��������д��������ռ�ձ����ݣ���ǿ��Ϊ���ռ�ձȡ�
	if(pwm_left < PWM_HIGH_MIN)		pwm_left = PWM_HIGH_MIN;	//��������д��С����Сռ�ձ����ݣ���ǿ��Ϊ��Сռ�ձȡ�
	if(pwm_right > PWM_HIGH_MAX)	pwm_right = PWM_HIGH_MAX;	//��������д��������ռ�ձ����ݣ���ǿ��Ϊ���ռ�ձȡ�
	if(pwm_right < PWM_HIGH_MIN)	pwm_right = PWM_HIGH_MIN;	//��������д��С����Сռ�ձ����ݣ���ǿ��Ϊ��Сռ�ձȡ�
	if(pwm_val_left<=pwm_left)		Left_moto_pwm = 1;  //װ����PWM����ߵ�ƽʱ��
	else Left_moto_pwm = 0; 						    //װ����PWM����͵�ƽʱ��
	if(pwm_val_left>=PWM_DUTY)		pwm_val_left = 0;	//�����Ա�ֵ���ڵ������ռ�ձ����ݣ���Ϊ��

	if(pwm_val_right<=pwm_right)	Right_moto_pwm = 1; //װ����PWM����ߵ�ƽʱ��
	else Right_moto_pwm = 0; 							//װ����PWM����͵�ƽʱ��
	if(pwm_val_right>=PWM_DUTY)		pwm_val_right = 0;	//����ҶԱ�ֵ���ڵ������ռ�ձ����ݣ���Ϊ��
}
/*����������*/
void QXMBOT_TrigUltrasonic()
{
	TrigPin = 0; //������ģ��Trig	���ƶ�
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	TrigPin = 1; //������ģ��Trig	���ƶ�
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();
	TrigPin = 0; //������ģ��Trig	���ƶ�
}
/*====================================
������	��QXMBOT_GetDistance
����	����
����ֵ	����ȡ���뵥λ����
����	�����������
ͨ�������źŵ��յ������źŵ�ʱ����Ծ���
��Ƭ������11.0592Mhz
ע��������Ϊ60ms����
====================================*/
unsigned int QXMBOT_GetDistance()
{
	unsigned int Distance = 0;	//����������
	unsigned int  Time=0;		//���ڴ�Ŷ�ʱ��ʱ��ֵ
	QXMBOT_TrigUltrasonic();	//����������
	while(!EchoPin);  	//�жϻ����ź��Ƿ�Ϊ�͵�ƽ
	Timer1On;			//������ʱ��1
	while(EchoPin);		//�ȴ��յ������ź�
	Timer1Off;			//�رն�ʱ��1
	Time=TH1*256+TL1;	//��ȡʱ��
	TH1=0;
	TL1=0;				//��ն�ʱ��
    Distance = (float)(Time*1.085)*0.17;//�������MM
	return(Distance);//���ؾ���				
}

void Tracking()//����ѭ������
{
	char data1, data2 = right_led2, data3 = right_led1, data4 = left_led1, data5 = left_led2;
	data1 = data2 * 1000 + data3 * 100 + data4 * 10 + data5;

	switch (data1)
	{
	case 0: // �ں����ϣ�ǰ��
	case 1001:
		forward(39, 37); // ǰ��
		break;
	case 100: // С��ƫ�ң���ת
		left_run(37, 40);
		while (1)
		{
			if (/*data1 == 0 || */data1 == 100 || data1 == 10) break;
		}
		//left_run(31, 35); // ��ת
		//forward(32, 31);
		break;
	case 10: // С��ƫ����ת
		right_run(42, 37);
		while (1)
		{
			if (/*data1 == 0 || */data1 == 10 || data1 == 100) break;
		}
		//right_run(42, 31); // ��ת
		//forward(52, 41);
		break;
	case 1100: // ���ƫ�ң���ת
		
		//if (data1 == 1110 || data1 == 1111 || data1 == 1001 || data1 == 110 || data1 == 1111) forward(40, 40);
		left_run(40, 50); // ��ת
		break;
	case 1000: // ��ת��
		//Delay_Ms(5);
		left_run(60, 60); // ��ת
		break;
	case 1010: // �Ҽ�ת��
		left_run(50, 60); // ��ת
		break;
	case 11: // ���ƫ����ת
		
		right_run(52, 40);
		//right_run(62, 60);
		//right_run(44, 34); // ��ת
		break;
	case 1: // �Ҽ�ת��
		
		right_run(60, 60); // ��ת

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

/********************* Timer0��ʼ��************************/
void Timer0_Init(void)
{
	TMOD |= 0x02;//��ʱ��0��8λ�Զ���װģ��
	TH0 = 0X9C;
	TL0 = 0X9C;//11.0592M����12T���ʱ��Լ����100΢��
	TR0 = 1;//������ʱ��0
	ET0 = 1;//����ʱ��0�ж�	
}
/*��ʱ��1��ʼ��*/
void Timer1_Init(void)		
{
	TMOD |= 0x10;	//��ʱ��1����ģʽ1,16λ��ʱģʽ��T1�ò�ECH0���峤��
	TH1 = 0;		   
    TL1 = 0;
	ET1 = 1;             //����T1�ж�
}
 
/********************* Timer0�жϺ���************************/
void timer0_int (void) interrupt 1
{
	 pwm_val_left++;
	 pwm_val_right++;
	 LoadPWM();//װ��PWM���
}
/* Timer0 interrupt routine */
void tm1_isr() interrupt 3 using 1
{
	Timer1Overflow = 1;	//������1�����־λ
	EchoPin = 0;		//���������ն�	
}	
