#ifndef _GUIDEDFILTER_
#define _GUIDEDFILTER_

#define _DHZ_LEVEL_EXPORTS

#include <iostream>
#include<cv.h>
#include<highgui.h>
#include"Dhz_Level.h"

_DHZ_LEVEL_API cv::Mat getimage(cv::Mat &a);
_DHZ_LEVEL_API cv::Mat cumsum(cv::Mat &imSrc, int rc);
_DHZ_LEVEL_API cv::Mat boxfilter(cv::Mat imSrc, int r);
_DHZ_LEVEL_API cv::Mat guidedfilter(cv::Mat I, cv::Mat p, int r, double eps);
_DHZ_LEVEL_API cv::Mat guidedfilter_color(cv::Mat I, cv::Mat p, int r, double eps);
#endif // GUIDEDFILTER

