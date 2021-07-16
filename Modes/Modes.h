#pragma once

#include <stdbool.h>
//20210713Y
#include <stdint.h>
//20210713Y

typedef struct
{
	float mode_frequency;	//ģʽ����Ƶ��
	void (*mode_enter)();	//����ģʽ��������
	void (*mode_exit)();	//�뿪ģʽ��������
	void (*mode_main_func)();	//ģʽ������������Ƶ��ִ�У�
}Mode;

//20210713Y

#define CMD_TYPE_MAVLINK (1<<4)//b 0001 0000
#define CMD_TYPE_MASK 0xf0
#define CMD_TYPE_PORT_MASK 0x0f

typedef struct
{
	//��4λ��1-mavlink��Ϣ
	//��4λ������port
	uint8_t cmd_type;
	uint8_t sd_sysid;
	uint8_t sd_compid;
	uint8_t frame;
	uint32_t cmd;
	double params[8];
}ModeMsg;

bool ModeReceiveMsg( ModeMsg* msg );
bool SendMsgToMode( ModeMsg msg );

//20210713Y

void init_Modes();

bool change_Mode( unsigned char mode );
unsigned char get_current_Mode();

#define Is_Calibration_Mode(m) ( m>=10 && m<=19 )
#define Is_Flight_Mode(m) ( m>=30 && m<=39 )