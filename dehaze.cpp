#include"stdafx.h"
#include "dehaze.h"


//dehaze algorithm implementation

IplImage* dehaze(IplImage *IplSrcImage, DehazeParameters* pDP)
{
	int block = pDP->block;
	double w = pDP->w;
	int air_threshold = pDP->air_threshold;
	int mode = pDP->mode;
	//图像分别有三个颜色通道,为分离的三个通道分配内存
	IplImage *dst1 = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IPL_DEPTH_8U, 1);
	IplImage *dst2 = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IPL_DEPTH_8U, 1);
	IplImage *dst3 = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IPL_DEPTH_8U, 1);

	//dst的ROI,为各个ROI通道分配内存
	IplImage *imgroi1 = cvCreateImage(cvSize(block, block), IPL_DEPTH_8U, 1);
	IplImage *imgroi2 = cvCreateImage(cvSize(block, block), IPL_DEPTH_8U, 1);
	IplImage *imgroi3 = cvCreateImage(cvSize(block, block), IPL_DEPTH_8U, 1);
	//dark channel的ROI
	IplImage *roidark = cvCreateImage(cvSize(block, block), IPL_DEPTH_8U, 1);;
	//暗原色先验的指针,为暗原色先验指针分配大小
	IplImage *dark_channel = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IPL_DEPTH_8U, 1);
	//透射率的指针,为透射率指针分配大小
	IplImage *t = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IPL_DEPTH_8U, 1);

	//去雾后的图像,三通道合并成,dst分配大小
	IplImage *dst = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IPL_DEPTH_8U, 3);
	//源图像ROI位置以及大小
	CvRect ROI_rect;
	ROI_rect.width = block;
	ROI_rect.height = block;
	ROI_rect.x = 0;
	ROI_rect.y = 0;

	//将原彩色图像分离成三通道
	cvSplit(IplSrcImage, dst1, dst2, dst3, NULL);

	//求暗原色
	int i;
	int j;
	double min1 = 0;
	double max1 = 0;
	double min2 = 0;
	double max2 = 0;
	double min3 = 0;
	double max3 = 0;
	double min = 0;
	CvScalar value;
	for (i = 0; i < IplSrcImage->width / block; i++)
	{
		for (j = 0; j < IplSrcImage->height / block; j++)
		{
			//分别计算三个通道内ROI的最小值
			cvSetImageROI(dst1, ROI_rect);
			cvCopy(dst1, imgroi1, NULL);
			cvMinMaxLoc(imgroi1, &min1, &max1, NULL, NULL);
			cvSetImageROI(dst2, ROI_rect);
			cvCopy(dst2, imgroi2, NULL);
			cvMinMaxLoc(imgroi2, &min2, &max2, NULL, NULL);
			cvSetImageROI(dst3, ROI_rect);
			cvCopy(dst3, imgroi3, NULL);
			cvMinMaxLoc(imgroi3, &min3, &max3, NULL, NULL);
			//求三个通道内最小值的最小值
			if (min1 < min2)
				min = min1;
			else
				min = min2;
			if (min > min3)
				min = min3;//min为这个ROI中暗原色
			value = cvScalar(min, min, min, min);//min放在value中
			//min赋予dark_channel中相应的ROI
			cvSetImageROI(dark_channel, ROI_rect);
			cvSet(roidark, value, NULL);
			cvCopy(roidark, dark_channel, NULL);
			//释放各个ROI
			cvResetImageROI(dst1);
			cvResetImageROI(dst2);
			cvResetImageROI(dst3);
			cvResetImageROI(dark_channel);
			//转入下一个ROI
			ROI_rect.x = block*i;
			ROI_rect.y = block*j;
		}
	}

	//利用得到的暗原色先验dark_channel_prior求大气光强估计值
	//get average of all points that satisfy (0.1% pixels that are the brightest)
	int n_pixels = 0.001*(IplSrcImage->imageSize);
	int* tmp_dst = new int[n_pixels];
	memset(tmp_dst, 0, n_pixels*sizeof(int));
	double A_dst1;
	double A_dst2;
	double A_dst3;

	//use Min heap to get largest K among N elements in every channel
	//dst1 highest K value
	HeapK(dst1, n_pixels, tmp_dst);
	long int tmpl = 0;
	for (i = 0; i < n_pixels; i++){
		tmpl += tmp_dst[i];
	}
	A_dst1 = tmpl / n_pixels;
	if (A_dst1 > air_threshold)
		A_dst1 = air_threshold;

	//dst2 highest K value
	HeapK(dst2, n_pixels, tmp_dst);
	tmpl = 0;
	for (i = 0; i < n_pixels; i++){
		tmpl += tmp_dst[i];
	}
	A_dst2 = tmpl / n_pixels;
	if (A_dst2 > air_threshold)
		A_dst2 = air_threshold;

	//dst3 highest K value
	HeapK(dst3, n_pixels, tmp_dst);
	tmpl = 0;
	for (i = 0; i < n_pixels; i++){
		tmpl += tmp_dst[i];
	}
	A_dst3 = tmpl / n_pixels;
	if (A_dst3 > air_threshold)
		A_dst3 = air_threshold;

	//求透射率
	int k;
	int l;
	CvScalar m;
	CvScalar n;

	for (k = 0; k < IplSrcImage->height; k++)
	{
		for (l = 0; l < IplSrcImage->width; l++)
		{
			m = cvGet2D(dark_channel, k, l);
			//w目的是保留一部分的雾,使图像看起来真实些
			n = cvScalar(255 - w*m.val[0]);
			cvSet2D(t, k, l, n);
		}
	}

	//导向滤波guided filter
	//use original gray image as guide image
	IplImage *gray = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IplSrcImage->depth, 1); 

	//convert to gray image
	cvCvtColor(IplSrcImage, gray, CV_BGR2GRAY);

	//guidedfilter use opencv2.0 interface thus should convert to cv::Mat

	cv::Mat matgray(gray, 0);
	cv::Mat matt(t, 0);

	cv::Mat matft;
	//guided filter window size set to 4 times of dehaze window size
	matft = guidedfilter(matgray, matt, block * 4, 0.001);
	IplImage* ft = new IplImage(matft);

	//求无雾图像   recover image
	if (mode == MODE_RGB){
		int p, q;
		double tx;
		double jj1, jj2, jj3;
		CvScalar ix, jx;

		for (p = 0; p < IplSrcImage->height; p++)
		{
			for (q = 0; q < IplSrcImage->width; q++)
			{
				//tx: transmision value is between [0,1] 
				tx = cvGetReal2D(ft, p, q);
				tx = tx / 255;
				//the minimun tx is 0.1
				if (tx < 0.1)
					tx = 0.1;
				ix = cvGet2D(IplSrcImage, p, q);
				//根据雾产生模型运算,还原出无雾图像
				jj1 = (ix.val[0] - A_dst1) / tx + A_dst1;
				jj2 = (ix.val[1] - A_dst2) / tx + A_dst2;
				jj3 = (ix.val[2] - A_dst3) / tx + A_dst3;
				jx = cvScalar(jj1, jj2, jj3, 0.0);
				cvSet2D(dst, p, q, jx);
			}
		}
	}
	else if (mode == MODE_HSV){
		//convert to HSV space then only reconstruct V channel
		IplImage* hsv_src = cvCreateImage(cvGetSize(IplSrcImage), IplSrcImage->depth, IplSrcImage->nChannels);
		cvCvtColor(IplSrcImage, hsv_src, CV_BGR2HSV);
		IplImage* v_src = cvCreateImage(cvGetSize(IplSrcImage), IplSrcImage->depth, 1);
		cvSplit(hsv_src, 0, 0, v_src, 0);
		double jjh, tx, vA = 0.0;
		int p, q;
		//Set atmosphere to be the average of three channel
		vA += A_dst1;
		vA += A_dst2;
		vA += A_dst3;
		vA /= 3;

		if (vA > air_threshold)
			vA = air_threshold;
		for (p = 0; p < IplSrcImage->height; p++)
		{
			for (q = 0; q < IplSrcImage->width; q++)
			{
				tx = cvGetReal2D(ft, p, q);
				tx = tx / 255;
				if (tx < 0.1)
					tx = 0.1;
				jjh = cvGetReal2D(v_src, p, q);
				cvSetReal2D(v_src, p, q, (jjh - vA) / tx + vA);
			}
		}
		cvMerge(0, 0, v_src, 0, hsv_src);
		cvCvtColor(hsv_src, dst, CV_HSV2BGR);
		//Release pointer
		cvReleaseImage(&hsv_src);
		cvReleaseImage(&v_src);
	}
	else if (mode == MODE_YUV){
		//convert to YUV space then only reconstruct Y channel
		IplImage* yuv_src = cvCreateImage(cvGetSize(IplSrcImage), IplSrcImage->depth, IplSrcImage->nChannels);
		cvCvtColor(IplSrcImage, yuv_src, CV_BGR2YUV);
		IplImage* y_src = cvCreateImage(cvGetSize(IplSrcImage), IplSrcImage->depth, 1);
		cvSplit(yuv_src, y_src, 0, 0, 0);
		double jjh, tx, vA;
		int p, q;

		vA = 0.299*A_dst3 + 0.587*A_dst2 + 0.114*A_dst1;
		if (vA > air_threshold)
			vA = air_threshold;

		for (p = 0; p < IplSrcImage->height; p++)
		{
			for (q = 0; q < IplSrcImage->width; q++)
			{
				tx = cvGetReal2D(ft, p, q);
				tx = tx / 255;
				if (tx < 0.1)
					tx = 0.1;
				jjh = cvGetReal2D(y_src, p, q);
				cvSetReal2D(y_src, p, q, (jjh - vA) / tx + vA);
			}
		}
		cvMerge(y_src, 0, 0, 0, yuv_src);
		cvCvtColor(yuv_src, dst, CV_YUV2BGR);

		//Release pointer
		cvReleaseImage(&yuv_src);
		cvReleaseImage(&y_src);
	}



	//=======================================/////


	//释放指针
	cvReleaseImage(&dst1);
	cvReleaseImage(&dst2);
	cvReleaseImage(&dst3);
	cvReleaseImage(&imgroi1);
	cvReleaseImage(&imgroi2);
	cvReleaseImage(&imgroi3);
	cvReleaseImage(&roidark);
	cvReleaseImage(&dark_channel);
	cvReleaseImage(&t);
	cvReleaseImage(&gray);
	delete ft;
	//	cvReleaseImage(&ft);
	return dst;
}
