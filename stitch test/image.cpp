#include "image.h"
Image::Image()
{

}
Image::~Image()
{

}
void Image::get_image(vector<string> images_name)//输入文件名序列，得到图片序列
{	
	// Check if have enough images
	num = static_cast<int>(images_name.size());
	if (num < 2)
	{
		cout << "Need more images" << endl;
		return;
	}
	for (int i = 0; i < num; i++)
	{	Mat source = imread(images_name[i]);
		image.push_back(source);
	}
}
