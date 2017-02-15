#pragma once
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
using namespace cv;
using namespace std;
class HiddenWatermark
{
public:
	HiddenWatermark(void);
	~HiddenWatermark(void);
	int addWatermark(const string srcImgFilename,const string watermarkImgFilename,const string saveFilename); //添加水印到图片
	int extWatermark(const string srcImgFilename,const string originaImgFilename,const string saveFilename); //从图片中提取水印
private:
	Mat addWatermarkCore(Mat srcImage,Mat watermarkImage); //添加水印到图片核心实现
	Mat extWatermarkCore(Mat srcImage,Mat originalImage); //从图片中提取水印核心实现
};

