#ifndef ROBOTCAN_PROPLIST_H_
#define ROBOTCAN_PROPLIST_H_

//����
#define MOTOR_WAVEBAT_FAQIU 0x000A
#define MOTOR_WAVEBAT_JIQIU 0x000B
/*******************************************************************************
ϵͳ��������
*******************************************************************************/
#define DriverType			0x0001	//���������ͣ�ֻ��
#define TEST				0x0003    //����ʹ�����ԣ���д
#define Protocol			0x0005	//�ŷ���������Э�飬ֻ��
#define DeviceID			0x000D	//�豸ID��ֻ��
#define SYSINIT				0x001E	//ϵͳ��ʼ����ֻд��д1����
#define RESETSYSTEM			0x0022	//����ϵͳ��ֻд��д1����
#define RESTORE				0x0026	//�ָ��������ã�ֻд��д1�ָ�
#define ERROR_CAN			0x0029	//ϵͳ�����룬ֻ��
#define ACK					0x002D	//����֡����ACK��
#define DEVPRIORITY			0x003B	//�豸���ȼ�����д
#define HEARTBEAT			0x003D	//��λ����ϵͳ��������֡�������������ر�ʾϵͳ������ֻ��
#define SETDEVID			0x004A	//����DeviceID��ֻд
/*******************************************************************************
�ŷ��������������
*******************************************************************************/
#define MOTOR_CTLEN			0x0403	//���õ����ţ�ʹ��״̬:0ֹͣ��1ʹ�ܣ�2ɲ������д
#define MOTOR_CTLMOD		0x0407	//����ģʽ��0�������ƣ�1�ٶȱջ���2λ�ñջ���4�ٶȵ����ջ�����д
#define MOTOR_CTLSET		0x0C03	//������Ʋ������ã���д
#define MOTOR_CTLSPD		0x0C13	//�趨Ŀ���ٶȣ���д
#define MOTOR_CTLPOS		0x0C17	//�趨Ŀ��λ�ã���д
#define MOTOR_SPDMAX        0x0C1B  //�趨������ת�٣���д
//#define MOTOR_CTLVOL		0x0C1B	//�趨Ŀ���ѹ����д		PWM?
#define MOTOR_CTLCUR		0x0C1F	//�趨Ŀ���������д
#define MOTOR_TEST_SPEED		0x0C27	//�趨����Ŀ���ٶȣ���д�����������趨��
#define ROBOT_INFORMATION 0x0C23		//�ؿ����壬��������Ϣ
/*******************************************************************************
�ŷ����״̬������
*******************************************************************************/
#define MOTOR_STSDIR		0x1001	//�����ǰ��ת������1Ϊ������0Ϊ������ֻ��
#define MOTOR_STSAGL		0x1005	//��ǰ��Ƕȣ�ֻ��
#define MOTOR_STSPOS		0x1025	//��ǰλ�ã�ֻ��
#define MOTOR_STSVOL		0x1029	//��ǰDQ��ѹ��ֻ��
#define MOTOR_STSCUR		0x102D	//��ǰDQ������ֻ��
#define MOTOR_STSSPD		0x1031	//��ǰ�ٶȣ�ֻ��
#define SYSTEM_VOL			0x1039	//��ǰϵͳ�ܵ�ѹ��ֻ��
#define SYSTEM_CUR			0x103D	//��ǰϵͳ�ܵ�����ֻ��
#define MOTOR_STSTMP		0x1041	//��ǰϵͳ�¶ȣ�ֻ��

#define ROBOT_HANDER		0x1047	//������̨pitch���yaw��
/*******************************************************************************
����������������
*******************************************************************************/
#define	SWITCH_STS			0x3001	//�г̿���״̬��ֻ��
#define CYLIND_CONTROL		0x3003	//���׿���
#define ROBOT_RST_ADJUST	0x1117	//�������̸�λλ��
#define ROBOT_RST_ATTACK	0x1123	//��С����
#define ROBOT_CAN_HEARTPACK 0x1127	//CAN����
#define ROBOT_ATTACK_SCOPE  0x1133	//����Χ
#define KINECT_PITCH		0x1137	//Kinect�ĸ���
#define ROBOT_TOWERSWITCH	0x1143	//����
#define ROBOT_PALYFIRSTBALL     0x1147 //�Ƿ��ǵ�һ����
#define ROBOT_NEEDTH		0x1153  //�Ƿ���Ҫ̷������

/*******************************************************************************
�ؿ� ���̴�վ���� 0x1***
*******************************************************************************/
#define ROBOT_CTLPOS 0x1013		//����������λ��
#define ROBOT_CTLAGL 0x1017		//������λ��
#define ROBOT_POSRST 0x1037
#define ROBOT_STATE  0x1033		//������״̬

#define SEND_CHASSIS_SPEED      0x1003  //�趨���̴�վ�ջ��ٶȣ���д
#define SEND_CHASSIS_PWM        0x1007  //�趨���̴�վ����ģʽ����PWM,��д
#define SEND_PARAME				0x100f  //�趨��վP��������д
#define PC_POWEROFF				0x101f	//PC�ػ�


/*******************************************************************************
�ؿ� �ض�λ���
*******************************************************************************/
#define ROBOT_RELOCALPOS 0x1023		//�ض�λλ��
#define ROBOT_RELOCALAGL 0x1027		//�ض�λ�Ƕ�

#define ROBOT_CONTROL_JIQIU_MODE 0x1113
//#define ROBOT_
/*******************************************************************************
����״̬����
*******************************************************************************/
#define BUS_OVR_VOL 		0		//ĸ�߹�ѹ
#define BUS_UDR_VOL			1		//ĸ��Ƿѹ
#define BUS_OVR_CUR			2		//ĸ�߹���
#define OVR_HEAT			3		//���ȱ���
#define POS_OVR_DIFF		7		//λ��ƫ�����

#endif
