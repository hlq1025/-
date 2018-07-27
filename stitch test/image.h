#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv_modules.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace std;
using namespace cv;
class Image
{
public:
	Image();
	~Image();
	void get_image(vector<string> images_name);
	vector<Mat>image;
	int num;//Í¼Æ¬ÊýÁ¿
};