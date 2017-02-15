#include "DWT.h"
#include <opencv\cv.h>
#include <opencv2\highgui.hpp>
DWT::DWT(void)
{
}


DWT::~DWT(void)
{
}

Mat DWT::enDWT(Mat srcImage) //图像 DWT变换
{
	Mat dwtImage = srcImage.clone(); //创建源图像副本，DWT操作在副本上进行，保留源图像
	int nLayer = 1; //DWT变换层数
	Size waveletSize;
	/* 开始计算DWT图像大小*/
	waveletSize.height = dwtImage.size().height;
	waveletSize.width = dwtImage.size().width;
	if ((dwtImage.size().width >> nLayer) << nLayer != dwtImage.size().width)
		waveletSize.width = ((dwtImage.size().width >> nLayer) + 1) << nLayer;
	if((dwtImage.size().height >> nLayer) << nLayer != dwtImage.size().height)
		waveletSize.height = ((dwtImage.size().height >> nLayer) + 1) << nLayer;
	/* 结束计算 */
	Mat waveletImg(waveletSize,CV_32FC3); //DWT后的图像
	if (waveletImg.data != NULL)
	{
		Mat waveletImgROI(waveletImg,Rect(0, 0, dwtImage.size().width, dwtImage.size().height));  //有效图像区域
		dwtImage.convertTo(waveletImgROI, CV_32FC3, 1, -128); //复制有效图像区域
		vector<Mat> channelImgs; //通道分离后的图像
		split(waveletImg,channelImgs); //按通道分离图像
		for(int i = 0; i < waveletImg.channels(); i++)
			enDWTCore(channelImgs[i], nLayer); //对各通道进行DWT
		merge(channelImgs,waveletImg); //合并DWT后的各通道
		waveletImgROI.convertTo(dwtImage, CV_32FC3, 1, 128); //拷贝DWT后的有效图像区域
	}
	return dwtImage;
}

Mat DWT::deDWT(Mat srcImage) //图像 DWT逆变换
{
	Mat dwtImage = srcImage.clone(); //创建源图像副本，DWT逆变换操作在副本上进行，保留源图像
	int nLayer = 1; //DWT变换层数
	Size waveletSize;
	/* 开始计算DWT逆变换图像大小*/
	waveletSize.height = dwtImage.size().height;
	waveletSize.width = dwtImage.size().width;
	if ((dwtImage.size().width >> nLayer) << nLayer != dwtImage.size().width)
		waveletSize.width = ((dwtImage.size().width >> nLayer) + 1) << nLayer;
	if((dwtImage.size().height >> nLayer) << nLayer != dwtImage.size().height)
		waveletSize.height = ((dwtImage.size().height >> nLayer) + 1) << nLayer;
	/* 结束计算 */
	Mat waveletImg(waveletSize,CV_32FC3); //DWT逆变换后的图像
	if (waveletImg.data != NULL)
	{
		Mat waveletImgROI(waveletImg,Rect(0, 0, dwtImage.size().width, dwtImage.size().height));  //有效图像区域
		dwtImage.convertTo(waveletImgROI, CV_32FC3, 1, -128); //复制有效图像区域
		vector<Mat> channelImgs; //通道分离后的图像
		split(waveletImg,channelImgs); //按通道分离图像
		for(int i = 0; i < waveletImg.channels(); i++)
			deDWTCore(channelImgs[i], nLayer); //对各通道进行DWT逆变换
		merge(channelImgs,waveletImg); //合并DWT逆变换后的各通道
		waveletImgROI.convertTo(dwtImage, CV_32FC3, 1, 128); //拷贝DWT逆变换后的有效图像区域
	}
	return dwtImage;
}

void DWT::enDWTCore(Mat srcImage, int nLayer) //单通道浮点 DWT变换
{
	IplImage* pImage = &srcImage.operator IplImage();
	if (pImage)
	{
		if (pImage->nChannels == 1 && pImage->depth == IPL_DEPTH_32F && ((pImage->width >> nLayer) << nLayer) == pImage->width && ((pImage->height >> nLayer) << nLayer) == pImage->height)
		{
			int i, x, y, n;
			float fValue = 0;
			float fRadius = sqrt(2.0f);
			int nWidth = pImage->width;
			int nHeight = pImage->height;
			int nHalfW = nWidth / 2;
			int nHalfH = nHeight / 2;
			float **pData = new float*[pImage->height];
			float *pRow = new float[pImage->width];
			float *pColumn = new float[pImage->height];
			for (i = 0; i < pImage->height; i++)
				pData[i] = (float*) (pImage->imageData + pImage->widthStep * i);
			for (n = 0; n < nLayer; n++, nWidth /= 2, nHeight /= 2, nHalfW /= 2, nHalfH /= 2) //多层小波变换
			{
				for (y = 0; y < nHeight; y++) //水平变换
				{
					memcpy(pRow, pData[y], sizeof(float) * nWidth); //奇偶分离
					for (i = 0; i < nHalfW; i++)
					{
						x = i * 2;
						pData[y][i] = pRow[x];
						pData[y][nHalfW + i] = pRow[x + 1];
					}
					for(i = 0; i < nHalfW - 1; i++)  //提升小波变换
					{
						fValue = (pData[y][i] + pData[y][i + 1]) / 2;
						pData[y][nHalfW + i] -= fValue;
					}
					fValue = (pData[y][nHalfW - 1] + pData[y][nHalfW - 2]) / 2;
					pData[y][nWidth - 1] -= fValue;
					fValue = (pData[y][nHalfW] + pData[y][nHalfW + 1]) / 4;
					pData[y][0] += fValue;
					for(i = 1; i < nHalfW; i++)
					{
						fValue = (pData[y][nHalfW + i] + pData[y][nHalfW + i - 1]) / 4;
						pData[y][i] += fValue;
					}
					for(i = 0; i < nHalfW; i++) //频带系数
					{
						pData[y][i] *= fRadius;
						pData[y][nHalfW + i] /= fRadius;
					}
				}
				for (x = 0; x < nWidth; x++) //垂直变换
				{
					for(i = 0; i < nHalfH; i++)  //奇偶分离
					{
						y = i * 2;
						pColumn[i] = pData[y][x];
						pColumn[nHalfH + i] = pData[y + 1][x];
					}
					for(i = 0; i < nHeight; i++)
					{
						pData[i][x] = pColumn[i];
					}
					for(i = 0; i < nHalfH - 1; i++) // 提升小波变换
					{
						fValue = (pData[i][x] + pData[i + 1][x]) / 2;
						pData[nHalfH + i][x] -= fValue;
					}
					fValue = (pData[nHalfH - 1][x] + pData[nHalfH - 2][x]) / 2;
					pData[nHeight - 1][x] -= fValue;
					fValue = (pData[nHalfH][x] + pData[nHalfH + 1][x]) / 4;
					pData[0][x] += fValue;
					for(i = 1; i < nHalfH; i++)
					{
						fValue = (pData[nHalfH + i][x] + pData[nHalfH + i - 1][x]) / 4;
						pData[i][x] += fValue;
					}
					for(i = 0; i < nHalfH; i++)  //频带系数
					{
						pData[i][x] *= fRadius;
						pData[nHalfH + i][x] /= fRadius;
					}
				}
			}
			delete[] pData;
			delete[] pRow;
			delete[] pColumn;
		}
	}
	Mat dwtImg(pImage,true);
	dwtImg.copyTo(srcImage);
}

void DWT::deDWTCore(Mat srcImage, int nLayer) //单通道浮点 DWT逆变换
{
	IplImage *pImage = &srcImage.operator IplImage();
	if (pImage)
	{
		if (pImage->nChannels == 1 && pImage->depth == IPL_DEPTH_32F && ((pImage->width >> nLayer) << nLayer) == pImage->width && ((pImage->height >> nLayer) << nLayer) == pImage->height)
		{
			int i, x, y, n;
			float fValue = 0;
			float fRadius = sqrt(2.0f);
			int nWidth = pImage->width >> (nLayer - 1);
			int nHeight = pImage->height >> (nLayer - 1);
			int nHalfW = nWidth / 2;
			int nHalfH = nHeight / 2;
			float **pData = new float*[pImage->height];
			float *pRow = new float[pImage->width];
			float *pColumn = new float[pImage->height];
			for(i = 0; i < pImage->height; i++)
			{
				pData[i] = (float*) (pImage->imageData + pImage->widthStep * i);
			}
			for(n = 0; n < nLayer; n++, nWidth *= 2, nHeight *= 2, nHalfW *= 2, nHalfH *= 2) //多层小波恢复
			{
				for(x = 0; x < nWidth; x++) //垂直恢复
				{
					for(i = 0; i < nHalfH; i++) //频带系数
					{
						pData[i][x] /= fRadius;
						pData[nHalfH + i][x] *= fRadius;
					}
					fValue = (pData[nHalfH][x] + pData[nHalfH + 1][x]) / 4; //提升小波恢复
					pData[0][x] -= fValue;
					for(i = 1; i < nHalfH; i++)
					{
						fValue = (pData[nHalfH + i][x] + pData[nHalfH + i - 1][x]) / 4;
						pData[i][x] -= fValue;
					}
					for(i = 0; i < nHalfH - 1; i++)
					{
						fValue = (pData[i][x] + pData[i + 1][x]) / 2;
						pData[nHalfH + i][x] += fValue;
					}
					fValue = (pData[nHalfH - 1][x] + pData[nHalfH - 2][x]) / 2;
					pData[nHeight - 1][x] += fValue;
					for(i = 0; i < nHalfH; i++) //奇偶合并
					{
						y = i * 2;
						pColumn[y] = pData[i][x];
						pColumn[y + 1] = pData[nHalfH + i][x];
					}
					for(i = 0; i < nHeight; i++)
						pData[i][x] = pColumn[i];
				}
				for (y = 0; y < nHeight; y++) //水平恢复
				{
					for (i = 0; i < nHalfW; i++) //频带系数
					{
						pData[y][i] /= fRadius;
						pData[y][nHalfW + i] *= fRadius;
					}
					fValue = (pData[y][nHalfW] + pData[y][nHalfW + 1]) / 4; //提升小波恢复
					pData[y][0] -= fValue;
					for (i = 1; i < nHalfW; i++)
					{
						fValue = (pData[y][nHalfW + i] + pData[y][nHalfW + i - 1]) / 4;
						pData[y][i] -= fValue;
					}
					for (i = 0; i < nHalfW - 1; i++)
					{
						fValue = (pData[y][i] + pData[y][i + 1]) / 2;
						pData[y][nHalfW + i] += fValue;
					}
					fValue = (pData[y][nHalfW - 1] + pData[y][nHalfW - 2]) / 2;
					pData[y][nWidth - 1] += fValue;
					for (i = 0; i < nHalfW; i++) //奇偶合并
					{
						x = i * 2;
						pRow[x] = pData[y][i];
						pRow[x + 1] = pData[y][nHalfW + i];
					}
					memcpy(pData[y], pRow, sizeof(float) * nWidth);
				}
			}
			delete[] pData;
			delete[] pRow;
			delete[] pColumn;
		}
	}
	Mat dwtImg(pImage,true);
	dwtImg.copyTo(srcImage);
}