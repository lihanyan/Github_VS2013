#ifndef ROBOTCAN_PROPLIST_H_
#define ROBOTCAN_PROPLIST_H_

//挥拍
#define MOTOR_WAVEBAT_FAQIU 0x000A
#define MOTOR_WAVEBAT_JIQIU 0x000B
/*******************************************************************************
系统基础属性
*******************************************************************************/
#define DriverType			0x0001	//驱动器类型，只读
#define TEST				0x0003    //测试使用属性，读写
#define Protocol			0x0005	//伺服驱动器子协议，只读
#define DeviceID			0x000D	//设备ID，只读
#define SYSINIT				0x001E	//系统初始化，只写，写1动作
#define RESETSYSTEM			0x0022	//重启系统，只写，写1重启
#define RESTORE				0x0026	//恢复出厂设置，只写，写1恢复
#define ERROR_CAN			0x0029	//系统错误码，只读
#define ACK					0x002D	//数据帧返回ACK，
#define DEVPRIORITY			0x003B	//设备优先级，读写
#define HEARTBEAT			0x003D	//上位控制系统发送请求帧请求心跳，返回表示系统正常，只读
#define SETDEVID			0x004A	//设置DeviceID，只写
/*******************************************************************************
伺服电机控制类属性
*******************************************************************************/
#define MOTOR_CTLEN			0x0403	//设置电机序号；使能状态:0停止，1使能，2刹车，读写
#define MOTOR_CTLMOD		0x0407	//控制模式：0开环控制，1速度闭环，2位置闭环，4速度电流闭环，读写
#define MOTOR_CTLSET		0x0C03	//电机控制参数设置，读写
#define MOTOR_CTLSPD		0x0C13	//设定目标速度，读写
#define MOTOR_CTLPOS		0x0C17	//设定目标位置，读写
#define MOTOR_SPDMAX        0x0C1B  //设定电机最大转速，读写
//#define MOTOR_CTLVOL		0x0C1B	//设定目标电压，读写		PWM?
#define MOTOR_CTLCUR		0x0C1F	//设定目标电流，读写
#define MOTOR_TEST_SPEED		0x0C27	//设定测试目标速度，读写（不能随意设定）
#define ROBOT_INFORMATION 0x0C23		//关康定义，发来的信息
/*******************************************************************************
伺服电机状态类属性
*******************************************************************************/
#define MOTOR_STSDIR		0x1001	//电机当前的转动方向，1为正方向，0为反方向，只读
#define MOTOR_STSAGL		0x1005	//当前电角度，只读
#define MOTOR_STSPOS		0x1025	//当前位置，只读
#define MOTOR_STSVOL		0x1029	//当前DQ电压，只读
#define MOTOR_STSCUR		0x102D	//当前DQ电流，只读
#define MOTOR_STSSPD		0x1031	//当前速度，只读
#define SYSTEM_VOL			0x1039	//当前系统总电压，只读
#define SYSTEM_CUR			0x103D	//当前系统总电流，只读
#define MOTOR_STSTMP		0x1041	//当前系统温度，只读

#define ROBOT_HANDER		0x1047	//控制云台pitch轴和yaw轴
/*******************************************************************************
辅助传感器类属性
*******************************************************************************/
#define	SWITCH_STS			0x3001	//行程开关状态，只读
#define CYLIND_CONTROL		0x3003	//气缸控制
#define ROBOT_RST_ADJUST	0x1117	//调整底盘复位位置
#define ROBOT_RST_ATTACK	0x1123	//升小旗子
#define ROBOT_CAN_HEARTPACK 0x1127	//CAN心跳
#define ROBOT_ATTACK_SCOPE  0x1133	//击球范围
#define KINECT_PITCH		0x1137	//Kinect的俯仰
#define ROBOT_TOWERSWITCH	0x1143	//切塔
#define ROBOT_PALYFIRSTBALL     0x1147 //是否是第一个球
#define ROBOT_NEEDTH		0x1153  //是否需要谭欢数据

/*******************************************************************************
关康 底盘从站属性 0x1***
*******************************************************************************/
#define ROBOT_CTLPOS 0x1013		//机器人整体位置
#define ROBOT_CTLAGL 0x1017		//机器人位姿
#define ROBOT_POSRST 0x1037
#define ROBOT_STATE  0x1033		//机器人状态

#define SEND_CHASSIS_SPEED      0x1003  //设定底盘从站闭环速度，读写
#define SEND_CHASSIS_PWM        0x1007  //设定地盘从站开环模式，给PWM,读写
#define SEND_PARAME				0x100f  //设定从站P参数，读写
#define PC_POWEROFF				0x101f	//PC关机


/*******************************************************************************
关康 重定位相关
*******************************************************************************/
#define ROBOT_RELOCALPOS 0x1023		//重定位位置
#define ROBOT_RELOCALAGL 0x1027		//重定位角度

#define ROBOT_CONTROL_JIQIU_MODE 0x1113
//#define ROBOT_
/*******************************************************************************
错误状态代号
*******************************************************************************/
#define BUS_OVR_VOL 		0		//母线过压
#define BUS_UDR_VOL			1		//母线欠压
#define BUS_OVR_CUR			2		//母线过流
#define OVR_HEAT			3		//过热保护
#define POS_OVR_DIFF		7		//位置偏差过大

#endif
