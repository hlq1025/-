#include "image.h"
Image::Image()
{

}
Image::~Image()
{

}
void Image::get_image(vector<string> images_name)//�����ļ������У��õ�ͼƬ����
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
