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

#ifndef SERIALPORT_H_
#define SERIALPORT_H_

//#define BCar
#define ACar







#include <Windows.h>

#include"..\UART2CAN\HUST_CAN.h"
#include"..\Gettime.h"
#include"TCPSocket.h"



/******************************TH��������*************************************/
union THReceiveDataUnion
{
	unsigned char charData[8];
	short shortData[4];
	float floatData[2];
	double doubleData[1];
};


//��ʶ�������� 
typedef enum tagSerialPortType
{
	UnKnow = 0,//δ֪�Ĵ���
	THTheroVision = 1,      
	KinectCarA = 2,      //A�����ȼ�����B��A������߾���Ȩ
	KinectCarB = 3,
} SerialPortType;


//������ʶ 
typedef enum tagActionType
{
	Wait = 0, 
	zijiwan = 1,
	Moving = 2,
	GetTheBallTH = 3,
	GetTheBallLA = 4,
	AvoidTheCar = 5,
} ActionType;


//֡����
//{0XFF , ֡ͷ������ ��x��y��t}
//kinect�ľ���д��guess��
typedef enum tagFramwType
{
	TellPosFrame = 0,//ֻ��kinect���Է�
	ToMoveFrame = 1,//�����ƶ�֡��ֻ��Kinect���Է���˫Ŀ���͵�һ���Ǿ���֡����֡���� ��{0XFF , ��֡ͷ��0�� ��x��y��t} ��
	DecisionFrame = 2,//�������֡��ֻ��˫Ŀ�ſ��Է���֡���� ��{0XFF , ֡ͷ����������0��1���� ��x��y��t} ��
	AskTypeFrame = 3,
	AnswerTypeFrame = 4, //֡���� ��{ 0XFF, ֡ͷ������ ��x��y��t } ��
	ResetFrame = 5,

} FramwType;
/*******************************************************************/






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


	CSerialPort *m_Me;

	double GetActionTypeTime;
	ActionType m_ActionType;

	//��ʶ���ڹ���
	SerialPortType m_SerialPortType;//һ�������߳�ֻ��Ӧһ������

	CSerialPort *AkinectSerialPort;
	CSerialPort *BkinectSerialPort;
	CSerialPort *THSerialPort;


	bool LAsend;

	bool THsend;


	HUST_CAN *m_CAN;//���ڽ����̷߳���

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


	bool OpenSendThread();
	//��������Ѻ�̨������ӵ������߳�
	bool ToSendData(unsigned char* pData, unsigned int length);



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
	bool SendTellPosFrame(short x, short y);

	bool SendToMovingFrame(bool MeOrYou, short x, short y, short t);
	bool SendDecisionFrame(bool _GET_NO, short x, short y, ushort t);

	bool SendAskTypeFrame();
	bool SendAnswerTypeFrame(SerialPortType _SerialPortType);
	bool SendResetFrame();

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

	/** ͬ������,�ٽ������� */
	CRITICAL_SECTION   m_csCommunicationSync;       //!< �����������

};

#endif //SERIALPORT_H_