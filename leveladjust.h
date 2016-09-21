#ifndef _LEVEL_ADJUST_
#define _LEVEL_ADJUST_

#define _DHZ_LEVEL_EXPORTS

#include"Dhz_Level.h"
#include<cv.h>
#include<highgui.h>
#include<string>
#define MAX_RGB_VALUE 255
#define MIN_RGB_VALUE 0

_DHZ_LEVEL_API long level_adjust(cv::Mat& src, cv::Mat& dst, double gamma, int input_shadow, int input_light, int output_shadow, int output_light);
_DHZ_LEVEL_API long level_adjust_auto(cv::Mat& matSrcImage, cv::Mat& matDstImage, LevelParameters* lp);

#endif // LEVEL_ADJUST

