/* ÀëÉ¢Ð¡²¨±ä»» Àà */
#pragma once
#include <opencv2\core.hpp>
using namespace cv;
class DWT
{
public:
	DWT(void);
	~DWT(void);
	Mat enDWT(Mat srcImage); //Í¼Ïñ DWT±ä»»
	Mat deDWT(Mat srcImage); //Í¼Ïñ DWTÄæ±ä»»
	void enDWTCore(Mat srcImage, int nLayer);
	void deDWTCore(Mat srcImage, int nLayer);
};

