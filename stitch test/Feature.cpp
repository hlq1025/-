#include "Feature.h"
Feature::Feature(string type,bool Try_gpu)
{
	features_type = type;
	try_gpu = Try_gpu;
}
Feature::~Feature()
{

}
void Feature::get_features(Mat img)
{
	Ptr<FeaturesFinder> finder;
	if (features_type == "surf")
	{
#if defined(HAVE_OPENCV_NONFREE) && defined(HAVE_OPENCV_GPU)
		if (try_gpu && gpu::getCudaEnabledDeviceCount() > 0)
			finder = new SurfFeaturesFinderGpu();
		else
#endif
			finder = new SurfFeaturesFinder();
	}
	else if (features_type == "orb")
	{
		finder = new OrbFeaturesFinder();
	}
	else
	{
		cout << "Unknown 2D features type: '" << features_type << "'.\n";
		return;
	}
	(*finder)(img, feature);
}