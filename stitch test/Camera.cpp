#include "Camera.h"
Camera::Camera(string ba_cost_func, float conf_thresh, string ba_refine_mask, bool do_wave_correct, WaveCorrectKind wave_correct)
{
	this->ba_cost_func = ba_cost_func;
	this->conf_thresh	= conf_thresh;
	this->ba_refine_mask = ba_refine_mask;
	this->do_wave_correct = do_wave_correct;
	this->wave_correct = wave_correct;
}
Camera::~Camera()
{

}
void Camera::get_cameraparameter(vector<ImageFeatures> features, vector<MatchesInfo> pairwise_matches)
{
	HomographyBasedEstimator estimator;
	estimator(features, pairwise_matches, cameras);
	for (size_t i = 0; i < cameras.size(); ++i)
	{
		Mat R;
		cameras[i].R.convertTo(R, CV_32F);
		cameras[i].R = R;
	}
}
void Camera::adjust_cameraparameter(vector<ImageFeatures> features, vector<MatchesInfo> pairwise_matches)
{
	Ptr<detail::BundleAdjusterBase> adjuster;
	if (ba_cost_func == "reproj") adjuster = new detail::BundleAdjusterReproj();
	else if (ba_cost_func == "ray") adjuster = new detail::BundleAdjusterRay();//ba_cost_func == "ray"
	else
	{
		cout << "Unknown bundle adjustment cost function: '" << ba_cost_func << "'.\n";
		return;
	}
	adjuster->setConfThresh(conf_thresh);//？？？
	Mat_<uchar> refine_mask = Mat::zeros(3, 3, CV_8U);
	if (ba_refine_mask[0] == 'x') refine_mask(0, 0) = 1;
	if (ba_refine_mask[1] == 'x') refine_mask(0, 1) = 1;
	if (ba_refine_mask[2] == 'x') refine_mask(0, 2) = 1;
	if (ba_refine_mask[3] == 'x') refine_mask(1, 1) = 1;
	if (ba_refine_mask[4] == 'x') refine_mask(1, 2) = 1;
	adjuster->setRefinementMask(refine_mask);
	(*adjuster)(features, pairwise_matches, cameras);
	for (size_t i = 0; i < cameras.size(); ++i)
	{
		Mat R;
		cameras[i].R.convertTo(R, CV_32F);
		cameras[i].R = R;
	}
	if (do_wave_correct)//对相机参数中的R阵做校正
	{
		vector<Mat> rmats;
		for (size_t i = 0; i <cameras.size(); ++i)
			rmats.push_back(cameras[i].R.clone());
		waveCorrect(rmats, wave_correct);//wave_correct是校正方式
		for (size_t i = 0; i < cameras.size(); ++i)
			cameras[i].R = rmats[i];
	}

}