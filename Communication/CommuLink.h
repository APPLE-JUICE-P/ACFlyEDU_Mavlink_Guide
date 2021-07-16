#pragma once

#include <stdint.h>
#include <stdbool.h>

//�˿ڶ���
typedef struct
{
	//д�˿ں���
	void (*write)( const uint8_t* data , uint16_t length );
	//���˿ں���
	uint16_t (*read)( uint8_t* data , uint16_t length );
	//��ȡ�˿ڴ����ֽ���������
	uint16_t (*DataAvailable)();
}Port;

//ע��˿�����Э��ͨ��
bool PortRegister( Port port );

//��ȡ�˿�
const Port* get_Port( uint8_t port );

/*����״̬*/
	//���Ͳ����б�
	void Send_Param_List();

	//���÷�����Ϣ
	//port_index:�˿����
	//Msg:��Ϣ���
	//Rate:����Ƶ��(hz) 0��ʾ������
	//�����Ƿ����óɹ�
	bool SetMsgRate( uint8_t port_index , uint16_t Msg , uint16_t Rate );
/*����״̬*/

//�趨mavlinkģʽ
bool set_mav_mode_arm();
bool set_mav_mode_disarm();
bool set_mav_mode(uint16_t req_mav_mode);
bool get_mav_mode(uint16_t *req_mav_mode);


void init_CommuLink();