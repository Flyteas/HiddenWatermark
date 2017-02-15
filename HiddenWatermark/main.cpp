#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include "HiddenWatermark.h"
using namespace cv;
int main()
{
	HiddenWatermark hiddenWatermark;
	hiddenWatermark.addWatermark("C:\\1.jpg","C:\\1.jpg","C:\\2.jpg"); //添加水印
	hiddenWatermark.extWatermark("C:\\2.jpg","C:\\2.jpg","C:\\3.jpg"); //提取水印
	return 0;
}
