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
#include "image.h"
#include"Feature.h"
#include"match.h"
#include"Camera.h"
#include"Warp.h"
using namespace std;
using namespace cv;
using namespace cv::detail;
// Default command line args
vector<string> img_names;
double compose_megapix = -1;
int main(int argc, char* argv[])
{	//读入图像
	img_names.push_back("image01.jpg");
	img_names.push_back("image02.jpg");
	img_names.push_back("image03.jpg");
	//img_names.push_back("test/image04.jpg");
	//img_names.push_back("test/image05.jpg");
	Image ima;
	ima.get_image(img_names);
	vector<ImageFeatures> features;
	for (int i = 0; i < ima.num; i++)
	{
		Feature fea("surf", false);//surf为选用的特征点类型,false表示不使用gpu
		fea.get_features(ima.image[i]);//提取每幅图像的特征点
		features.push_back(fea.feature);
	}
	Match match(false, 0.3);
	match.feature_match(features);
	Camera camera("ray", 1, "xxxxx", true, detail::WAVE_CORRECT_HORIZ);
	camera.get_cameraparameter(features, match.pairwise_matches);
	camera.adjust_cameraparameter(features, match.pairwise_matches);
	warp war(false, "spherical", ExposureCompensator::GAIN_BLOCKS, Blender::MULTI_BAND, 5);
	war.warppic(ima.image, camera.cameras);
	war.blendpic(ima.image, camera.cameras);
	return 0;
}