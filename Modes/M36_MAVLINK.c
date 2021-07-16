#include "Modes.h"
#include "Basic.h"
#include "stdlib.h"
#include <stdio.h>
#include "M36_MAVLINK.h"

#include "AC_Math.h"
#include "Receiver.h"
#include "InteractiveInterface.h"
#include "ControlSystem.h"
#include "MeasurementSystem.h"

#include "CommuLink.h"
#include "mavlink.h"


static uint16_t mav_mode;
bool guided_enabled=false;
bool pos_enabled=true;
static void M36_MAVLINK_MainFunc();
static void M36_MAVLINK_enter();
static void M36_MAVLINK_exit();
const Mode M36_MAVLINK =
{
    50,  //mode frequency
    M36_MAVLINK_enter,  //enter
    M36_MAVLINK_exit,	//exit
    M36_MAVLINK_MainFunc,	//mode main func
};

typedef struct
{
    //�˳�ģʽ������
    uint16_t exit_mode_counter;

} MODE_INF;
static MODE_INF* Mode_Inf;

static void M36_MAVLINK_enter()
{
    Led_setStatus( LED_status_running1 );

    //��ʼ��ģʽ����
    Mode_Inf = malloc( sizeof( MODE_INF ) );
    Mode_Inf->exit_mode_counter = 0;
    Altitude_Control_Enable();
		Position_Control_Enable();
}

static void M36_MAVLINK_exit()
{
    Altitude_Control_Disable();
    Attitude_Control_Disable();

    free( Mode_Inf );
}
static void M36_MAVLINK_MainFunc()
{
    const Receiver* rc = get_current_Receiver();

    if( rc->available == false )
    {
        //���ջ�������
        //����
        Position_Control_set_XYLock();
        Position_Control_set_TargetVelocityZ( -50 );
        return;
    }
    float throttle_stick = rc->data[0];
    float yaw_stick = rc->data[1];
    float pitch_stick = rc->data[2];
    float roll_stick = rc->data[3];
    float mode_stick = rc->data[5];
		bool 	position_ready= get_Position_Measurement_System_Status() == Measurement_System_Status_Ready ;
    /*�ж��˳�ģʽ*/
    if( throttle_stick < 5 && yaw_stick < 5 && pitch_stick < 5 && roll_stick > 95 )
    {
        if( ++Mode_Inf->exit_mode_counter >= 50 )
        {
            change_Mode( 1 );
            return;
        }
    }
    else
        Mode_Inf->exit_mode_counter = 0;
    /*�ж��˳�ģʽ*/

    //�ж�ҡ���Ƿ����м�
    bool sticks_in_neutral =
        in_symmetry_range_offset_float( throttle_stick, 5, 50 ) && \
        in_symmetry_range_offset_float( yaw_stick, 5, 50 ) && \
        in_symmetry_range_offset_float( pitch_stick, 5, 50 ) && \
        in_symmetry_range_offset_float( roll_stick, 5, 50 );

    if( sticks_in_neutral)
    {
        //MODE��ť�ڵ�λ��ҡ������λ
        //��ˮƽλ�ÿ���
        //Position_Control_Enable();
        guided_enabled=true;

        //����Guidedģʽ
		get_mav_mode(&mav_mode);
		set_mav_mode(mav_mode | MAV_MODE_FLAG_GUIDED_ENABLED);

		if(get_Position_Measurement_System_Status() == Measurement_System_Status_Ready&&pos_enabled==true)
		{
			Position_Control_Enable();
		}
    }
    else
    {
        //MODE��ť�ڸ�λ��ҡ�˲�����λ
				guided_enabled=false;
        //ȡ��GUIDEDģʽ
		get_mav_mode(&mav_mode);
		set_mav_mode(mav_mode & ~MAV_MODE_FLAG_GUIDED_ENABLED);
        //�ر�ˮƽλ�ÿ���
        Position_Control_Disable();

        //�߶ȿ�������
        if( in_symmetry_range_offset_float( throttle_stick, 5, 50 ) )
            Position_Control_set_ZLock();
        else
            Position_Control_set_TargetVelocityZ( ( throttle_stick - 50.0f ) * 6 );

        //ƫ����������
        if( in_symmetry_range_offset_float( yaw_stick, 5, 50 ) )
            Attitude_Control_set_YawLock();
        else
            Attitude_Control_set_Target_YawRate( ( 50.0f - yaw_stick )*0.05f );

        //Roll Pitch��������
        Attitude_Control_set_Target_RollPitch( \
                                               ( roll_stick 	- 50.0f )*0.015f, \
                                               ( pitch_stick - 50.0f )*0.015f );
    }
}

bool Get_Guided_Mode_Enabled(void)
{
    return guided_enabled;
}
bool Get_POS_Control_Enabled(void)
{
    return pos_enabled;
}
void Set_POS_Control_Enabled(bool value)
{
		pos_enabled=value;
}