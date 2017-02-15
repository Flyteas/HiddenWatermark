#include "HiddenWatermark.h"
#include "DWT.h"

HiddenWatermark::HiddenWatermark(void)
{
}


HiddenWatermark::~HiddenWatermark(void)
{
}

int HiddenWatermark::addWatermark(const string srcImgFilename,const string watermarkImgFilename,const string saveFilename) //添加水印到图片
{
	Mat srcImg = imread(srcImgFilename,CV_32FC3);
	if(srcImg.data == NULL) //读取图片文件失败
		return 1;
	Mat watermarkImg = imread(watermarkImgFilename);
	if(watermarkImg.data == NULL) //读取水印图片文件失败
		return 2;
	Mat resultImg = addWatermarkCore(srcImg,watermarkImg);
	if(resultImg.data == NULL) //添加水印失败
		return 3;
	vector<int> saveParams;
	saveParams.push_back(CV_IMWRITE_JPEG_QUALITY); //设置JPEG压缩质量
	saveParams.push_back(100); //设置为100
	if(!imwrite(saveFilename,resultImg,saveParams)) //保存失败
		return 4;
	return 0;
}

int HiddenWatermark::extWatermark(const string srcImgFilename,const string originaImgFilename,const string saveFilename) //从图片中提取水印
{
	Mat srcImg = imread(srcImgFilename,CV_32FC3);
	if(srcImg.data == NULL) //读取图片文件失败
		return 1;
	Mat originaImg = imread(originaImgFilename);
	if(originaImg.data == NULL) //读取原始图片文件失败
		return 2;
	Mat watermarkImg = extWatermarkCore(srcImg,originaImg); //提取水印
	if(watermarkImg.data == NULL) //提取水印失败
		return 3;
	vector<int> saveParams;
	saveParams.push_back(CV_IMWRITE_JPEG_QUALITY); //设置JPEG压缩质量
	saveParams.push_back(100); //设置为100
	if(!imwrite(saveFilename,watermarkImg,saveParams)) //保存失败
		return 4;
	return 0;
}

Mat HiddenWatermark::addWatermarkCore(Mat srcImage,Mat watermarkImage) //添加水印到图片核心实现
{
	DWT dwt;
	return dwt.enDWT(srcImage);
}

Mat HiddenWatermark::extWatermarkCore(Mat srcImage,Mat originalImage) //从图片中提取水印核心实现
{
	DWT dwt;
	return dwt.deDWT(srcImage);
}