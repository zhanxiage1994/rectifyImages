#include<stdio.h>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
	Mat imgL = imread("5\\00000002.jpg"),imgLr;
	Mat imgR = imread("5\\00000001.jpg"),imgRr;
	if(imgL.size() != imgR.size())
	{
		std::cout<<"Images size don't equal";
		while (1);
	}
	Size imageSize = imgL.size();
	
	Mat RRight = (cv::Mat_<double>(3,3) << 0.969081,0.00190664,0.246734,0.00926729,0.998983,-0.0441182,-0.246568,0.0450406,0.968078);
	//cout<<"RRight"<<endl<<RRight<<endl;
	Mat RLeft = (cv::Mat_<double>(3,3) << 0.914639,-0.00155425,0.404269,0.0219432,0.998709,-0.0458059,-0.403676,0.0507668,0.913493);
	//cout<<"RLeft"<<endl<<RLeft<<endl;
	Mat KRight = (cv::Mat_<double>(3,3) <<597.89375,0,323.39810,0,600.66325,252.24752,0,0,1);
	//cout<<"KRight"<<endl<<KRight<<endl;
	Mat KLeft = (cv::Mat_<double>(3,3) << 587.77398,0,314.07005,0,591.68901,251.10127,0,0,1);
	//cout<<"KLeft"<<endl<<KLeft<<endl;
	Mat TLeft = (cv::Mat_<double>(3,1) << -34.48228,-1.53045,2.69210);
	Mat TRight = (cv::Mat_<double>(3,1) << -25.45477 ,-0.63171,-2.06869);

	//R = Rr*inv(Rl); T = Tr - R*Tl;
	invert(RLeft,RLeft);
	Mat R = RRight.clone();
	gemm(R,RLeft,1,1,0,R);
	Mat T;
	gemm(R,TLeft,1,1,0,T);
	T = TRight - T;
	
	Mat R1, R2, P1, P2, Q;
	stereoRectify(KLeft,Mat::zeros(1,5,CV_32F),KRight,Mat::zeros(1,5,CV_32F),
		imageSize,R,T,R1,R2,P1,P2,Q,CALIB_ZERO_DISPARITY,0);
	cout<<"R"<<endl<<R<<endl;
	cout<<"R1"<<endl<<R1<<endl;
	cout<<"R2"<<endl<<R2<<endl;
	cout<<"P1"<<endl<<P1<<endl;
	cout<<"P2"<<endl<<P2<<endl;
	cv::FileStorage temp("tempData.xml",cv::FileStorage::WRITE);  
	temp<<"R"<<R;
	temp<<"T"<<T;
	temp<<"R1"<<R1;
	temp<<"R2"<<R2;
	temp<<"P1"<<P1;
	temp<<"P2"<<P2;
	temp.release();
	
	Mat rmap[2][2];
    initUndistortRectifyMap(KLeft,Mat::zeros(1,5,CV_32F), R1, P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
    initUndistortRectifyMap(KRight, Mat::zeros(1,5,CV_32F), R2, P2, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);
	remap(imgL, imgLr, rmap[0][0], rmap[0][1], CV_INTER_AREA);	//left
    remap(imgR, imgRr, rmap[1][0], rmap[1][1], CV_INTER_AREA);	//right
	//imshow("imgL",imgLr);
	//waitKey(30);
	//imshow("imgR",imgRr);
	//waitKey(0);

	Mat showImage(2 * imageSize.height,2*imageSize.width,CV_8UC3);
	Rect originLeft(0,0,imageSize.width,imageSize.height);
	Rect originRight(imageSize.width,0,imageSize.width,imageSize.height);
	imgL.copyTo(showImage(originLeft));
	imgR.copyTo(showImage(originRight));
	Rect rectLeft(0,imageSize.height,imageSize.width,imageSize.height);
	Rect rectRight(imageSize.width,imageSize.height,imageSize.width,imageSize.height);
	imgLr.copyTo(showImage(rectLeft));
	imgRr.copyTo(showImage(rectRight));
	
    for( int i = 0; i < 2 * imageSize.height; i += 32 )
	{
		line(showImage, Point(0, i), Point(2*imageSize.width, i), Scalar(0, 255, 0), 1, 8);
	}
	//imshow("test",showImage);
	//waitKey(0);
	imwrite("imgLr.bmp",imgLr);
	imwrite("imgRr.bmp",imgRr);
	imwrite("hahaprj.bmp",showImage);
	system("pause");
	return 0;
}
