#include "Modes.h"

#include "STS.h"

//0-9Ϊ�Ƿ��к�У׼ģʽ
#include "M00_init.h"
#include "M01_Ground.h"

//10-19ΪУ׼ģʽ
#include "M10_RCCalib.h"
#include "M12_AccCalib.h"
#include "M13_MagCalib.h"
#include "M15_HorizontalCalib.h"

//30-39Ϊ����ģʽ
#include "M30_Att.h"
#include "M32_PosCtrl.h"
#include "M35_Auto1.h"
#include "M36_MAVLINK.h"

//20210713Y
#include "mavlink.h"

//���׻��ζ���
#define MSG_Q_SIZE 20//������󳤶�
static ModeMsg message_queue[MSG_Q_SIZE];
static uint8_t msg_q_head = 0;
static uint8_t msg_q_tail = 0;
static uint8_t msg_q_len = 0;//��ǰ���г���(Ԫ������)

bool SendMsgToMode( ModeMsg msg )//��β���
{
	if(msg_q_len >= MSG_Q_SIZE)//���з�����
	{
		return false;
	}

	message_queue[msg_q_tail++] = msg;//�Ž�ȥ,tail++
	msg_q_tail %= MSG_Q_SIZE;//���ζ���
	msg_q_len++;//����+1

	return true;
}

bool ModeReceiveMsg( ModeMsg* msg )//��ͷ����
{
	if(msg_q_len <= 0)//�����ǿյ�
	{
		return false;
	}

	*msg = message_queue[msg_q_head++];//ȡ����,head++
	msg_q_head %= MSG_Q_SIZE;//���ζ���
	msg_q_len--;//����-1
	return true;
}
//20210713Y



static unsigned int Mode_Task_ID;
static uint8_t current_mode = 0;
static Mode Modes[50] = {0};

static void Modes_Server( unsigned int Task_ID )
{
	Modes[ current_mode ].mode_main_func();
}

bool change_Mode( unsigned char mode )
{
	if( Modes[ mode ].mode_main_func == 0 )
		return false;
	Modes[ current_mode ].mode_exit();
	current_mode = mode;
	STS_Change_Task_Mode( Mode_Task_ID , STS_Task_Trigger_Mode_RoughTime , 1.0f / Modes[current_mode].mode_frequency , 0 );
	Modes[ current_mode ].mode_enter();
	return true;
}
unsigned char get_current_Mode()
{
	return current_mode;
}

void init_Modes()
{
	/*��ʼ��ģʽ*/
		//0-9Ϊ�Ƿ��к�У׼ģʽ
		Modes[0] = M00_init;
		Modes[1] = M01_Ground;
	
		//10-19ΪУ׼ģʽ
		Modes[10] = M10_RCCalib;
		Modes[12] = M12_AccCalib;
		Modes[13] = M13_MagCalib;
		Modes[15] = M15_HorizontalCalib;

		//30-39Ϊ����ģʽ
		Modes[30] = M30_Att;
		Modes[32] = M32_PosCtrl;
		Modes[35] = M35_Auto1;
		Modes[36] = M36_MAVLINK;
		
	/*��ʼ��ģʽ*/
	
	current_mode = 0;
	Mode_Task_ID = STS_Add_Task( STS_Task_Trigger_Mode_RoughTime , 1.0f / Modes[current_mode].mode_frequency , 0 , Modes_Server );
	Modes[ current_mode ].mode_enter();
}