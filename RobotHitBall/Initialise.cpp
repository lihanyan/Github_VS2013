
int Intialise()
{
	//Coordinate_robot = new Coordinate(X_CAMERA_TO_ROBOT,Y_CAMERA_TO_ROBOT,Z_CAMERA_TO_ROBOT,ANGLE_CAMERA_TO_ROBOT,&CAN);// (44, 590, 0, 0, &CAN);
	////OpenCV�п���CPU��Ӳ��ָ���Ż����ܺ���
	//setUseOptimized( true );

	//// ��kinect
	//IKinectSensor* pSensor;
	//HRESULT hResult = S_OK;
	//hResult = GetDefaultKinectSensor( &pSensor );
	//if( FAILED( hResult ) ){
	//	cerr << "Error : GetDefaultKinectSensor" << std::endl;
	//	return -1;
	//}

	//hResult = pSensor->Open();
	//if( FAILED( hResult ) ){
	//	cerr << "Error : IKinectSensor::Open()" << std::endl;
	//	return -1;
	//}

	//// ���֡Դ
	//IDepthFrameSource* pDepthSource;
	//hResult = pSensor->get_DepthFrameSource( &pDepthSource );
	//if( FAILED( hResult ) ){
	//	cerr << "Error : IKinectSensor::get_DepthFrameSource()" << std::endl;
	//	return -1;
	//}

	//// ���֡��ȡ
	//IDepthFrameReader* pDepthReader;
	//hResult = pDepthSource->OpenReader( &pDepthReader );
	//if( FAILED( hResult ) ){
	//	cerr << "Error : IDepthFrameSource::OpenReader()" << std::endl;
	//	return -1;
	//}

	//// Description
	//IFrameDescription* pDescription;
	//hResult = pDepthSource->get_FrameDescription( &pDescription );
	//if( FAILED( hResult ) ){
	//	cerr << "Error : IDepthFrameSource::get_FrameDescription()" << std::endl;
	//	return -1;
	//}


	return 0;
}