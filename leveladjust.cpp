#include"stdafx.h"
#include"leveladjust.h"


long level_adjust_auto(cv::Mat& matSrcImage, cv::Mat& matDstImage, LevelParameters* lp)
{
	// use gamma correction to do level adjust
	if (matSrcImage.rows == 0 || matSrcImage.cols == 0)
	{
		std::cout << "Error: Invalid input strSrcImage at level_adjust_auto" << std::endl;
		return -1;
	}

	double dCustomGamma = lp->dCustomGamma;
	//set default values
	int iInputShadow = 0, iInputLight = 255, iOutputShadow = 0, iOutputLight = 255, midtone = 0;
	double dGamma = 1.0; //default gamma value;


	std::vector<cv::Mat> rgb_src;

	split(matSrcImage, rgb_src);

	//Calculate histogram of RGB channel values, vary parameters according to the histogram
	int histSize = 256;

	cv::Mat rgb_hist;
	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };
	bool uniform = true; bool accumulate = true;
	calcHist(&rgb_src[0], 1, 0, cv::Mat(), rgb_hist, 1, &histSize, &histRange, uniform, false);
	calcHist(&rgb_src[1], 1, 0, cv::Mat(), rgb_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&rgb_src[2], 1, 0, cv::Mat(), rgb_hist, 1, &histSize, &histRange, uniform, accumulate);

	int sum = 0;
	int m;

	for (m = 0; m <= 255; m++){
		sum += rgb_hist.at<float>(m);

		//set iInputShadow value to the value that cover 0.005 of the histogram 
		if (((double)sum > 0.005*(3 * matSrcImage.cols*matSrcImage.rows)) && iInputShadow == 0)
			iInputShadow = m;

		//set iInputLight value to the value that cover 0.995 of the histogram
		if (((double)sum > 0.995*(3 * matSrcImage.cols*matSrcImage.rows)) && iInputLight == 255)
			iInputLight = m;
	}

	// if the RGB values is distributed in a narrow range, don't extend its distribution too much 
	if ((iInputLight - iInputShadow) <= 120)
	{
		iInputShadow = iInputShadow / 2;
		iInputLight = (iInputLight + 255) / 2;
	}

	dGamma = (double)(dCustomGamma);



	// gamma correction 
	if (0 == level_adjust(matSrcImage, matDstImage, dGamma, iInputShadow, iInputLight, iOutputShadow, iOutputLight))
	{
		return 0;
	}
	else
	{
		std::cout << "Error£º level adjust fail" << std::endl;
		return -1;
	}
}


long level_adjust(cv::Mat& matSrcImage, cv::Mat& matDstImage, double dGamma, int iInputShadow, int iInputLight, int iOutputShadow, int iOutputLight)
{
	//Check input parameters
	if (dGamma<0.01 || dGamma>9.99)
	{
		std::cout << "Error: Invalid gamma value,"\
			" gamma must be set to [0.01 9.99] " << std::endl;
		return -1;
	} 
	if (iInputShadow > iInputLight || iInputShadow > MAX_RGB_VALUE|| iInputShadow < MIN_RGB_VALUE)
	{
		std::cout << "Error: Invalid shadow input level,"\
			" shadow input level must be set to [0, 255] and no larger than highlight input level" << std::endl;
		return -1;
	}
	if (iInputShadow > iInputLight || iInputLight > MAX_RGB_VALUE || iInputLight < MIN_RGB_VALUE)
	{
		std::cout << "Error: Invalid highlight input level,"\
			" highlight input level must be set to [0, 255] and no smaller than shadow input level" << std::endl;
		return -1;
	}
	if (iOutputShadow > iOutputLight || iOutputShadow > MAX_RGB_VALUE || iOutputShadow < MIN_RGB_VALUE)
	{
		std::cout << "Error: Invalid shadow output level, "\
			"shadow output level must be set to [0, 255] and no larger than highlight output level" << std::endl;
		return -1;
	}
	if (iOutputShadow > iOutputLight || iOutputLight > MAX_RGB_VALUE || iOutputLight < MIN_RGB_VALUE)
	{
		std::cout << "Error: Invalid highlight output level, "\
			"highlight output level must be set to [0, 255] and no smaller than shadow output level" << std::endl;
		return -1;
	}

	matDstImage = matSrcImage.clone();

	int rgbdiff=0;
	double clrgb=0.0;
	int rgbov=0;		//rgb output value
	int rgbv=0;			//rgb value
	int diff = iInputLight - iInputShadow;

	uchar* data = matDstImage.data;
	for (int c = 0; c < matSrcImage.channels(); c++){
		for (int p = 0; p < matSrcImage.rows; p++)
		{
			for (int q = 0; q < matSrcImage.cols; q++)
			{
				rgbv = *(data);

				if ((rgbv - iInputShadow) < 0)
					*(data++) = iOutputShadow;
				else if ((rgbv - iInputShadow) > diff){
					*(data++) = iOutputLight;
				}
				else{
					//equation of gamma correction: 
					// new_value=((rgbv-iInputShadow)/(iInputLight-iInputShadow))^(1/gamma)*(iOutputLight-iOutputShadow)+iOutputShadow
					rgbdiff = rgbv - iInputShadow;
					clrgb = pow((double)rgbdiff / diff, (double)1 / dGamma);
					rgbov = clrgb*(iOutputLight - iOutputShadow) + iOutputShadow;
					*(data++) = rgbov;
				}
			}
		}
	}

	return 0;

}
