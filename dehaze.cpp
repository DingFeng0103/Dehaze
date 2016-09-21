#include"stdafx.h"
#include "dehaze.h"


//dehaze algorithm implementation

IplImage* dehaze(IplImage *IplSrcImage, DehazeParameters* pDP)
{
	int block = pDP->block;
	double w = pDP->w;
	int air_threshold = pDP->air_threshold;
	int mode = pDP->mode;
	//ͼ��ֱ���������ɫͨ��,Ϊ���������ͨ�������ڴ�
	IplImage *dst1 = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IPL_DEPTH_8U, 1);
	IplImage *dst2 = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IPL_DEPTH_8U, 1);
	IplImage *dst3 = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IPL_DEPTH_8U, 1);

	//dst��ROI,Ϊ����ROIͨ�������ڴ�
	IplImage *imgroi1 = cvCreateImage(cvSize(block, block), IPL_DEPTH_8U, 1);
	IplImage *imgroi2 = cvCreateImage(cvSize(block, block), IPL_DEPTH_8U, 1);
	IplImage *imgroi3 = cvCreateImage(cvSize(block, block), IPL_DEPTH_8U, 1);
	//dark channel��ROI
	IplImage *roidark = cvCreateImage(cvSize(block, block), IPL_DEPTH_8U, 1);;
	//��ԭɫ�����ָ��,Ϊ��ԭɫ����ָ������С
	IplImage *dark_channel = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IPL_DEPTH_8U, 1);
	//͸���ʵ�ָ��,Ϊ͸����ָ������С
	IplImage *t = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IPL_DEPTH_8U, 1);

	//ȥ����ͼ��,��ͨ���ϲ���,dst�����С
	IplImage *dst = cvCreateImage(cvSize(IplSrcImage->width, IplSrcImage->height), IPL_DEPTH_8U, 3);
	//Դͼ��ROIλ���Լ���С
	CvRect ROI_rect;
	ROI_rect.width = block;
	ROI_rect.height = block;
	ROI_rect.x = 0;
	ROI_rect.y = 0;

	//��ԭ��ɫͼ��������ͨ��
	cvSplit(IplSrcImage, dst1, dst2, dst3, NULL);

	//��ԭɫ
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
			//�ֱ��������ͨ����ROI����Сֵ
			cvSetImageROI(dst1, ROI_rect);
			cvCopy(dst1, imgroi1, NULL);
			cvMinMaxLoc(imgroi1, &min1, &max1, NULL, NULL);
			cvSetImageROI(dst2, ROI_rect);
			cvCopy(dst2, imgroi2, NULL);
			cvMinMaxLoc(imgroi2, &min2, &max2, NULL, NULL);
			cvSetImageROI(dst3, ROI_rect);
			cvCopy(dst3, imgroi3, NULL);
			cvMinMaxLoc(imgroi3, &min3, &max3, NULL, NULL);
			//������ͨ������Сֵ����Сֵ
			if (min1 < min2)
				min = min1;
			else
				min = min2;
			if (min > min3)
				min = min3;//minΪ���ROI�а�ԭɫ
			value = cvScalar(min, min, min, min);//min����value��
			//min����dark_channel����Ӧ��ROI
			cvSetImageROI(dark_channel, ROI_rect);
			cvSet(roidark, value, NULL);
			cvCopy(roidark, dark_channel, NULL);
			//�ͷŸ���ROI
			cvResetImageROI(dst1);
			cvResetImageROI(dst2);
			cvResetImageROI(dst3);
			cvResetImageROI(dark_channel);
			//ת����һ��ROI
			ROI_rect.x = block*i;
			ROI_rect.y = block*j;
		}
	}

	//���õõ��İ�ԭɫ����dark_channel_prior�������ǿ����ֵ
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

	//��͸����
	int k;
	int l;
	CvScalar m;
	CvScalar n;

	for (k = 0; k < IplSrcImage->height; k++)
	{
		for (l = 0; l < IplSrcImage->width; l++)
		{
			m = cvGet2D(dark_channel, k, l);
			//wĿ���Ǳ���һ���ֵ���,ʹͼ��������ʵЩ
			n = cvScalar(255 - w*m.val[0]);
			cvSet2D(t, k, l, n);
		}
	}

	//�����˲�guided filter
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

	//������ͼ��   recover image
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
				//���������ģ������,��ԭ������ͼ��
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


	//�ͷ�ָ��
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
