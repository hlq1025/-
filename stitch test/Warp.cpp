#include "Warp.h"
warp::warp(bool try_gpu, string warp_type, int expos_comp_type, int  blend_type, float blend_strength)
{
	this->try_gpu = try_gpu;
	this->warp_type = warp_type;
	this->expos_comp_type = expos_comp_type;
	this->blend_type = blend_type;
	this->blend_strength = blend_strength;
}
warp::~warp()
{

}
void  warp::warppic(vector<Mat>image, vector<CameraParams> cameras)
{
	vector<double> focals;
	int num = cameras.size();
	for (size_t i = 0; i < cameras.size(); ++i)
	{
		cout << "Camera #" << i + 1 << ":\n" << cameras[i].K() << endl;
		focals.push_back(cameras[i].focal);
	}
	sort(focals.begin(), focals.end());
	if (focals.size() % 2 == 1)
		warped_image_scale = static_cast<float>(focals[focals.size() / 2]);
	else
		warped_image_scale = static_cast<float>(focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) * 0.5f;
	corners.resize(num);
	masks_warped.resize(num);
	masks.resize(num);
	images_warped.resize(num);
	sizes.resize(num);
	// Preapre images masks
	for (int i = 0; i < num; ++i)
	{
		masks[i].create(image[i].size(), CV_8U);
		masks[i].setTo(Scalar::all(255));
	}
	// Warp images and their masks
#if defined(HAVE_OPENCV_GPU)
	if (try_gpu && gpu::getCudaEnabledDeviceCount() > 0)
	{
		if (warp_type == "plane") warper_creator = new cv::PlaneWarperGpu();
		else if (warp_type == "cylindrical") warper_creator = new cv::CylindricalWarperGpu();
		else if (warp_type == "spherical") warper_creator = new cv::SphericalWarperGpu();
	}
	else
#endif
	{
		if (warp_type == "plane") warper_creator = new cv::PlaneWarper();
		else if (warp_type == "cylindrical") warper_creator = new cv::CylindricalWarper();
		else if (warp_type == "spherical") warper_creator = new cv::SphericalWarper();
		else if (warp_type == "fisheye") warper_creator = new cv::FisheyeWarper();
		else if (warp_type == "stereographic") warper_creator = new cv::StereographicWarper();
		else if (warp_type == "compressedPlaneA2B1") warper_creator = new cv::CompressedRectilinearWarper(2, 1);
		else if (warp_type == "compressedPlaneA1.5B1") warper_creator = new cv::CompressedRectilinearWarper(1.5, 1);
		else if (warp_type == "compressedPlanePortraitA2B1") warper_creator = new cv::CompressedRectilinearPortraitWarper(2, 1);
		else if (warp_type == "compressedPlanePortraitA1.5B1") warper_creator = new cv::CompressedRectilinearPortraitWarper(1.5, 1);
		else if (warp_type == "paniniA2B1") warper_creator = new cv::PaniniWarper(2, 1);
		else if (warp_type == "paniniA1.5B1") warper_creator = new cv::PaniniWarper(1.5, 1);
		else if (warp_type == "paniniPortraitA2B1") warper_creator = new cv::PaniniPortraitWarper(2, 1);
		else if (warp_type == "paniniPortraitA1.5B1") warper_creator = new cv::PaniniPortraitWarper(1.5, 1);
		else if (warp_type == "mercator") warper_creator = new cv::MercatorWarper();
		else if (warp_type == "transverseMercator") warper_creator = new cv::TransverseMercatorWarper();
	}

	if (warper_creator.empty())
	{
		cout << "Can't create the following warper '" << warp_type << "'\n";
		return ;
	}
	warper = warper_creator->create(static_cast<float>(warped_image_scale));
	for (int i = 0; i < num; ++i)
	{
		Mat_<float> K;
		cameras[i].K().convertTo(K, CV_32F);
		float swa = 1;
		K(0, 0) *= swa; K(0, 2) *= swa;
		K(1, 1) *= swa; K(1, 2) *= swa;
		corners[i] = warper->warp(image[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, images_warped[i]);
		cout << corners[i] << endl;
		sizes[i] = images_warped[i].size();
		warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masks_warped[i]);
	}
	vector<Mat> images_warped_f(num);
	for (int i = 0; i < num; ++i)
		images_warped[i].convertTo(images_warped_f[i], CV_32F);
	//cout<<"Warping images, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec"<<endl;
	//开始做曝光补偿
	compensator = ExposureCompensator::createDefault(expos_comp_type);
	compensator->feed(corners, images_warped, masks_warped);
}
void  warp::blendpic(vector<Mat>image, vector<CameraParams> cameras)
{
	int num = cameras.size();
	Mat img_warped, img_warped_s;
	Mat dilated_mask, seam_mask, mask, mask_warped;
	Ptr<Blender> blender;
	//double compose_seam_aspect = 1;
	double compose_work_aspect = 1;
	Mat img;
	for (int img_idx = 0; img_idx <num; ++img_idx)
	{
		cout << "Compositing image #" << img_idx + 1 << endl;

		// Read image and resize it if necessary
		Mat	full_img = image[img_idx];


		// Update warped image scale
		warped_image_scale *= static_cast<float>(compose_work_aspect);
		warper = warper_creator->create(warped_image_scale);

		// Update corners and sizes
		for (int i = 0; i < num; ++i)
		{
			// Update intrinsics
			cameras[i].focal *= compose_work_aspect;
			cameras[i].ppx *= compose_work_aspect;
			cameras[i].ppy *= compose_work_aspect;
			// Update corner and size
			Size sz = image[i].size();
			sz.width = cvRound(image[i].size().width);//四舍五入
			sz.height = cvRound(image[i].size().height);
			Mat K;
			cameras[i].K().convertTo(K, CV_32F);
			Rect roi = warper->warpRoi(sz, K, cameras[i].R);
			corners[i] = roi.tl();
			sizes[i] = roi.size();
		}
	Size img_size = full_img.size();
	Mat K;
	cameras[img_idx].K().convertTo(K, CV_32F);
	// Warp the current image
	warper->warp(full_img, K, cameras[img_idx].R, INTER_LINEAR, BORDER_REFLECT, img_warped);
	// Warp the current image mask
		mask.create(img_size, CV_8U);
		mask.setTo(Scalar::all(255));
		warper->warp(mask, K,cameras[img_idx].R, INTER_NEAREST, BORDER_CONSTANT, mask_warped);

		// Compensate exposure
		compensator->apply(img_idx, corners[img_idx], img_warped, mask_warped);
		img_warped.convertTo(img_warped_s, CV_16S);
		img_warped.release();
		img.release();
		mask.release();
		dilate(masks_warped[img_idx], dilated_mask, Mat());//形态学膨胀
		resize(dilated_mask, seam_mask, mask_warped.size());
		mask_warped = seam_mask & mask_warped;

		if (blender.empty())
		{
			blender = Blender::createDefault(blend_type, try_gpu);
			Size dst_sz = resultRoi(corners, sizes).size();//最终大图的尺寸
			float blend_width = sqrt(static_cast<float>(dst_sz.area())) * blend_strength / 100.f;
			if (blend_width < 1.f)
				blender = Blender::createDefault(Blender::NO, try_gpu);
			else if (blend_type == Blender::MULTI_BAND)
			{
				MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(static_cast<Blender*>(blender));
				mb->setNumBands(static_cast<int>(ceil(log(blend_width) / log(2.)) - 1.));
				cout << "Multi-band blender, number of bands: " << mb->numBands() << endl;
			}
			else if (blend_type == Blender::FEATHER)
			{
				FeatherBlender* fb = dynamic_cast<FeatherBlender*>(static_cast<Blender*>(blender));
				fb->setSharpness(1.f / blend_width);
				cout << "Feather blender, sharpness: " << fb->sharpness() << endl;
			}
			blender->prepare(corners, sizes);
		}
		// Blend the current image
		blender->feed(img_warped_s, mask_warped, corners[img_idx]);
	}
	blender->blend(result, result_mask);
	imwrite("result.jpg", result);
	result.convertTo(result, CV_8UC1);
	imshow("stitch", result);
	waitKey(0);
}
