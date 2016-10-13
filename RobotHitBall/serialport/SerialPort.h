//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2009, ���пƼ���ѧtickTick Group  ����Ȩ������
/// All rights reserved.
/// 
/// @file    SerialPort.h  
/// @brief   ����ͨ����ͷ�ļ�
///
/// ���ļ���ɴ���ͨ���������
///
/// @version 1.0   
/// @author  ¬�� 
/// @E-mail��lujun.hust@gmail.com
/// @date    2010/03/19
///
///  �޶�˵����
//////////////////////////////////////////////////////////////////////////

#include <Windows.h>

#include"..\UART2CAN\HUST_CAN.h"
#include"..\Gettime.h"
#include"TCPSocket.h"


#ifndef SERIALPORT_H_
#define SERIALPORT_H_


#define SendDataListNum 10 

extern HUST_CAN CAN;

/******************************TH��������*************************************/
union THReceiveDataUnion
{
	unsigned char charData[10];
	short shortData[4];
	float floatData[2];
	double doubleData[1];
};



//��״̬��ʶ //��״̬
typedef enum tagStateOfAB
{
	AllWait = 0,
	AGetheBall = 1,
	BGetheBall = 2,
} StateOfAB;

//������ʶ //С״̬//�ȼ�
typedef enum tagActionType
{
	AvoidCarKinect = -2,//��һ̨�����������Ŀ��λ��
	AvoidCarTH = -1,//˫Ŀ������ָ����һ̨������
	Wait = 0,
	GetTheBallKinect = 1,
	GetTheBallTH = 2,
	GetTheBallLA = 3,
} ActionType;


//��ʶ�������� 
typedef enum tagSerialPortType
{
	UnKnow = 0,//δ֪�Ĵ���
	THTheroVision = 1,
	KinectCar = 2,
} SerialPortType;



//֡����
//{0XFF , ֡ͷ������ ��x��y��t}
//kinect�ľ���д��guess��
typedef enum tagFramwType
{
	TellPosConditionFrame = 0,
	AimPosFrame = 1,//�����ƶ�֡// {0xFF ֡ͷ ��Ϣ x ��y t } //��Ϣ���� A����0 A������1 B������2 B������3
	AskTypeFrame = 2,
	AnswerTypeFrame = 3, //֡���� ��{ 0XFF, ֡ͷ������ ��x��y��t } ��
	ResetFrame = 4,
} FramwType;
/*******************************************************************/


//AimPosFrame����Ϣ
typedef enum tagAimPosFrameInfo
{
	AGet_HighBall = 0,//00
	BGet_HighBall = 1,//01
	AGet_LowBall = 2,//10
	BGet_LowBall = 3,//11
} AimPosFrameInfo;
/*******************************************************************/




extern StateOfAB m_StateOfAB;//��״̬
extern double GeStateOfABTime;

extern ActionType m_ActionType;//С״̬
extern double GetActionTypeTime;

extern int RESETX;
extern int RESETY;





/** ����ͨ����
*
*  ����ʵ���˶Դ��ڵĻ�������
*  �����������ָ�����ڵ����ݡ�����ָ�����ݵ�����
*/
class CSerialPort
{
public:
	CSerialPort(void);
	~CSerialPort(void);

	bool InitSuccess;


	int Counter;
	float LastNum;//���ڲ鿴����
	int flag;//��־AB��
	THReceiveDataUnion m_thReceiveDataUnion;//�յ������ݱ�־


	int portNum;

	double DistanceToDropPoint;//�������������ľ��룬�����ж��ĸ��������ĸ�������


	short x;
	short y;
	double GetPositionTime;//��¼�ϴεõ���λ�õ�ʱ�䣬���ڲ�ֵ�����ٶ�
	double Vx;
	double Vy;



	//��ʶ���ڹ���
	SerialPortType m_SerialPortType;//һ�������߳�ֻ��Ӧһ������




	//���Ͷ���
	//���Ͷ����У���һλΪ��־λ 0 ����1 1��ʾ������ 0��ʾ�ѷ���
	unsigned char SendData[SendDataListNum][40];
	int SendListHead = 0;
	int SendListTail = 0;
	

	//���λ��
	short AvoidPosX = CAN.m_robotState.robot_xNow;
	short AvoidPosY = CAN.m_robotState.robot_yNow;


public:

	/** ��ʼ�����ں���
	*
	*  @param:  UINT portNo ���ڱ��,Ĭ��ֵΪ1,��COM1,ע��,������Ҫ����9
	*  @param:  UINT baud   ������,Ĭ��Ϊ9600
	*  @param:  char parity �Ƿ������żУ��,'Y'��ʾ��Ҫ��żУ��,'N'��ʾ����Ҫ��żУ��
	*  @param:  UINT databits ����λ�ĸ���,Ĭ��ֵΪ8������λ
	*  @param:  UINT stopsbits ֹͣλʹ�ø�ʽ,Ĭ��ֵΪ1
	*  @param:  DWORD dwCommEvents Ĭ��ΪEV_RXCHAR,��ֻҪ�շ�����һ���ַ�,�����һ���¼�
	*  @return: bool  ��ʼ���Ƿ�ɹ�
	*  @note:   ��ʹ�����������ṩ�ĺ���ǰ,���ȵ��ñ��������д��ڵĳ�ʼ��
	*���������� \n�������ṩ��һЩ���õĴ��ڲ�������,����Ҫ����������ϸ��DCB����,��ʹ�����غ���
	*           \n������������ʱ���Զ��رմ���,�������ִ�йرմ���
	*  @see:
	*/
	bool InitPort(UINT  portNo = 1, UINT  baud = CBR_9600, char  parity = 'N', UINT  databits = 8,
		UINT  stopsbits = 1, DWORD dwCommEvents = EV_RXCHAR);

	/** ���ڳ�ʼ������
	*
	*  �������ṩֱ�Ӹ���DCB�������ô��ڲ���
	*  @param:  UINT portNo
	*  @param:  const LPDCB & plDCB
	*  @return: bool  ��ʼ���Ƿ�ɹ�
	*  @note:   �������ṩ�û��Զ���ش��ڳ�ʼ������
	*  @see:
	*/
	bool InitPort(UINT  portNo, const LPDCB& plDCB);

	/** ���������߳�
	*
	*  �������߳���ɶԴ������ݵļ���,�������յ������ݴ�ӡ����Ļ���
	*  @return: bool  �����Ƿ�ɹ�
	*  @note:   ���߳��Ѿ����ڿ���״̬ʱ,����flase
	*  @see:
	*/
	bool OpenListenThread();

	/** �رռ����߳�
	*
	*
	*  @return: bool  �����Ƿ�ɹ�
	*  @note:   ���ñ�������,�������ڵ��߳̽��ᱻ�ر�
	*  @see:
	*/
	bool CloseListenThread();



	/** �򴮿�д����
	*
	*  ���������е�����д�뵽����
	*  @param:  unsigned char * pData ָ����Ҫд�봮�ڵ����ݻ�����
	*  @param:  unsigned int length ��Ҫд������ݳ���
	*  @return: bool  �����Ƿ�ɹ�
	*  @note:   length��Ҫ����pData��ָ�򻺳����Ĵ�С
	*  @see:
	*/
	bool WriteData(unsigned char* pData, unsigned int length);

	/** ��ȡ���ڻ������е��ֽ���
	*
	*
	*  @return: UINT  �����Ƿ�ɹ�
	*  @note:   �����ڻ�������������ʱ,����0
	*  @see:
	*/
	UINT GetBytesInCOM();

	/** ��ȡ���ڽ��ջ�������һ���ֽڵ�����
	*
	*
	*  @param:  char & cRecved ��Ŷ�ȡ���ݵ��ַ�����
	*  @return: bool  ��ȡ�Ƿ�ɹ�
	*  @note:
	*  @see:
	*/
	bool ReadChar(char &cRecved);



	/************************�Լ�����ĺ���****************************************/
	bool OpenSendThread();
	static UINT WINAPI SendThread(void* pParam);

	bool AddDataToSend(FramwType _FramwType, unsigned char Infor, short x, short y, short t);
	

private:

	/** �򿪴���
	*
	*
	*  @param:  UINT portNo �����豸��
	*  @return: bool  ���Ƿ�ɹ�
	*  @note:
	*  @see:
	*/
	bool openPort(UINT  portNo);

	/** �رմ���
	*
	*
	*  @return: void  �����Ƿ�ɹ�
	*  @note:
	*  @see:
	*/
	void ClosePort();

	/** ���ڼ����߳�
	*
	*  �������Դ��ڵ����ݺ���Ϣ
	*  @param:  void * pParam �̲߳���
	*  @return: UINT WINAPI �̷߳���ֵ
	*  @note:
	*  @see:
	*/
	static UINT WINAPI ListenThread(void* pParam);



	

private:

	/** ���ھ�� */
	HANDLE  m_hComm;

	/** �߳��˳���־���� */
	static bool s_bExit;

	/** �߳̾�� */
	volatile HANDLE    m_hListenThread;
	volatile HANDLE    m_hSendDataThread;

	/** ͬ������,�ٽ������� */
	CRITICAL_SECTION   m_csCommunicationSync;       //!< �����������

};

#endif //SERIALPORT_H_