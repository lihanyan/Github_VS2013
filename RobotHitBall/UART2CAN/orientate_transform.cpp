#include "orientate_transform.h"

void Coordinate::Coordinate_transformation()
{
	double coordinate_trobot[4] = { 0, 0, 0, 1 };

	MatrixMultiplication(this->Coordinate_robot.T_camera_to_robot, this->Coordinate_robot.coordinate_camera, coordinate_trobot);
	MatrixMultiplication(this->Coordinate_robot.T_robot_to_Generalized, coordinate_trobot, this->Coordinate_robot.coordinate_Generalized);
}

Coordinate::Coordinate(double x_camera_to_robot, double y_camera_to_robot, double z_camera_to_robot, double angle_camera_to_robot, HUST_CAN *can_src)
{
	CAN = can_src;
	//结构体里面不允许对数组赋值，故在初始化里面进行赋值
	this->Coordinate_robot.x_camera_to_robot = x_camera_to_robot;;
	this->Coordinate_robot.y_camera_to_robot = y_camera_to_robot;
	this->Coordinate_robot.z_camera_to_robot = z_camera_to_robot;
	this->Coordinate_robot.theta_camera_to_robot = angle_camera_to_robot;
	for (int i = 0; i < 4; i++)
	{
		this->Coordinate_robot.coordinate_camera[i] = 0;
		this->Coordinate_robot.coordinate_Generalized[i] = 0;
		this->Coordinate_robot.coordinate_robot[i] = 0;
		for (int j = 0; j < 4; j++)
		{
			this->Coordinate_robot.T_camera_to_robot[i][j] = 0;
			this->Coordinate_robot.T_robot_to_Generalized[i][j] = 0;
		}
	}
	this->Coordinate_robot.coordinate_camera[3] = this->Coordinate_robot.coordinate_Generalized[3] = this->Coordinate_robot.coordinate_robot[3] = 1;
	this->Coordinate_robot.T_camera_to_robot[2][2] = this->Coordinate_robot.T_camera_to_robot[3][3] = this->Coordinate_robot.T_robot_to_Generalized[2][2] = this->Coordinate_robot.T_robot_to_Generalized[3][3] = 1;

	//摄像机在车上的位置是固定的，故只初始化一次就好
	//theta输入是角度
	double x = this->Coordinate_robot.x_camera_to_robot;
	double y = this->Coordinate_robot.y_camera_to_robot;
	double z = this->Coordinate_robot.z_camera_to_robot;
	double theta = this->Coordinate_robot.theta_camera_to_robot;
	this->Coordinate_robot.T_camera_to_robot[0][0] = this->Coordinate_robot.T_camera_to_robot[1][1] = cos(theta*3.141592653 / 180);
	this->Coordinate_robot.T_camera_to_robot[0][1] = -sin(theta*3.141592653 / 180);
	this->Coordinate_robot.T_camera_to_robot[1][0] = sin(theta*3.141592653 / 180);
	this->Coordinate_robot.T_camera_to_robot[0][3] = x;
	this->Coordinate_robot.T_camera_to_robot[1][3] = y;
	this->Coordinate_robot.T_camera_to_robot[2][3] = z; 
}

void Coordinate::MatrixMultiplication(double in1[4][4], double in2[4], double *out)
{
	for (int i = 0; i < 4; i++)
	{
		out[i] = 0;
		for (int j = 0; j < 4; j++)
		{
			out[i] += (in1[i][j] * in2[j]);
		}
	}
}
void Coordinate::Coordinate_calculate(int listnumber)
{
	//车身位置是不断在动的，故要实时更新
	//theta输入是角度
	this->Coordinate_robot.T_robot_to_Generalized[0][0] = this->Coordinate_robot.T_robot_to_Generalized[1][1] = cos(this->Coordinate_robot.theta_robot[listnumber] * 3.141592653 / 180);
	this->Coordinate_robot.T_robot_to_Generalized[0][1] = -sin(this->Coordinate_robot.theta_robot[listnumber] * 3.141592653 / 180);
	this->Coordinate_robot.T_robot_to_Generalized[1][0] = sin(this->Coordinate_robot.theta_robot[listnumber] * 3.141592653 / 180);
	this->Coordinate_robot.T_robot_to_Generalized[0][3] = this->Coordinate_robot.coordinate_robot[0] = this->Coordinate_robot.x_robot[listnumber];
	this->Coordinate_robot.T_robot_to_Generalized[1][3] = this->Coordinate_robot.coordinate_robot[1] = this->Coordinate_robot.y_robot[listnumber];
	this->Coordinate_robot.coordinate_robot[2] = this->Coordinate_robot.theta_robot[listnumber];

}
void Coordinate::Coordinate_set_position_to_camera(double x, double y, double z)
{
	this->Coordinate_robot.coordinate_camera[0] = (double)x;
	this->Coordinate_robot.coordinate_camera[1] = (double)y;
	this->Coordinate_robot.coordinate_camera[2] = (double)z;
	this->Coordinate_robot.coordinate_camera[3] = 1;
}
void  Coordinate::Coordinate_calculate_position_to_court( double x_to_camerra, double y_to_camerra, double z_to_camerra,int listnumber)
{
	Coordinate_set_position_to_camera(x_to_camerra, y_to_camerra, z_to_camerra);
	Coordinate_calculate(listnumber);
	Coordinate_transformation();
}
void Coordinate::Coordinate_printf_information(int listnum)
{
	/*cout << "T_robot_to_Generalized :" << endl;
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			cout << this->Coordinate_robot.T_robot_to_Generalized[x][y] << " ";
		}
		cout << endl;
	}
	cout << endl;
	cout << "T_camera_to_robot :" << endl;
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			cout << this->Coordinate_robot.T_camera_to_robot[x][y] << " ";
		}
		cout << endl;
	}
	cout << "camera:" << Coordinate_robot.coordinate_camera[0] << " " << Coordinate_robot.coordinate_camera[1] << " " << Coordinate_robot.coordinate_camera[2] << endl;
	cout << "general:" << Coordinate_robot.coordinate_Generalized[0] << " " << Coordinate_robot.coordinate_Generalized[1] << " " << Coordinate_robot.coordinate_Generalized[2] << endl;
	*/
	cout << "robot:" << this->Coordinate_robot.x_robot[listnum] << " " << this->Coordinate_robot.y_robot[listnum] << " " << this->Coordinate_robot.theta_robot[listnum] << endl;

	cout << endl;
} 
void Coordinate::Coordinate_get_position_from_can(int listnum)
{
	this->Coordinate_robot.x_robot[listnum] = (double)this->CAN->m_robotState.robot_x;
	this->Coordinate_robot.y_robot[listnum] = (double)this->CAN->m_robotState.robot_y;
	this->Coordinate_robot.theta_robot[listnum] = (double)this->CAN->m_robotState.angle;
}