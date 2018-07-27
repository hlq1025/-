#include <iostream>
#include <fstream>
#include <string>
#include "opencv2/opencv_modules.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/detail/autocalib.hpp"
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/stitching/detail/camera.hpp"
#include "opencv2/stitching/detail/exposure_compensate.hpp"
#include "opencv2/stitching/detail/matchers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/detail/seam_finders.hpp"
#include "opencv2/stitching/detail/util.hpp"
#include "opencv2/stitching/detail/warpers.hpp"
#include "opencv2/stitching/warpers.hpp"
using namespace std;
using namespace cv;
using namespace cv::detail;


class warp{
public:
	warp(bool try_gpu, string warp_type, int expos_comp_type, int  blend_type,float blend_strength);
	~warp();
	void warppic(vector<Mat>image, vector<CameraParams> cameras);
	void blendpic(vector<Mat>image,vector<CameraParams> cameras);
private:
	bool try_gpu;
	string warp_type;
	int expos_comp_type;
	float warped_image_scale; //warped_image_scale赋为相机焦距值的中位数
	Ptr<WarperCreator> warper_creator;
	Ptr<RotationWarper> warper;
	vector<Point> corners;
	vector<Mat> masks_warped;
	vector<Mat> images_warped;
	vector<Size> sizes;//存储images_warped的大小
	vector<Mat> masks;
	Ptr<ExposureCompensator> compensator;
	int  blend_type;
	float blend_strength;
	Mat result, result_mask;
};