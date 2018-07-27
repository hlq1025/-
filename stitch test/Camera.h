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
class Camera
{public:
	Camera(string ba_cost_func, float conf_thresh, string ba_refine_mask, bool do_wave_correct, WaveCorrectKind wave_correct);
	~Camera();
	void get_cameraparameter(vector<ImageFeatures> features, vector<MatchesInfo> pairwise_matches);
	void adjust_cameraparameter(vector<ImageFeatures> features, vector<MatchesInfo> pairwise_matches);
	vector<CameraParams> cameras;
private:
	string ba_cost_func;
	float conf_thresh;
	string ba_refine_mask;
	bool do_wave_correct;
	WaveCorrectKind wave_correct;
	
};