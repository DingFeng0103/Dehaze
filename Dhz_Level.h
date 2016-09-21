/*
* Copyright (c) 2016, Daspatial
* All rights reserved.
*
* 文件名称：Dhz_Level.h
* 摘    要：do dark prior dehaze and color level adjust operations
*
* 当前版本：1.0
* 作    者：丁枫
* 完成日期：2016年6月22日
*/



#ifndef _DHZ_LEVEL_
#define _DHZ_LEVEL_

#ifdef _DHZ_LEVEL_EXPORTS
#define _DHZ_LEVEL_API __declspec(dllexport)
#else
#define _DHZ_LEVEL_API __declspec(dllimport)
#pragma comment( lib, "Dhz_Level.lib")		
#endif



#define FREE_POINTER(p)	{if (p!=NULL){delete p; p = NULL;}}
/* =============		definition for dehazing mode		============================
MODE_RGB:				do dark channel prior dehaze on RGB channels separately
MODE_HSV:				do dark channel prior dehaze only on V channel of HSV
MODE_YUV:				do dark channel prior dehaze only on Y channel of YUV
========================================================================================
*/
#define MODE_RGB    0
#define MODE_HSV    1
#define MODE_YUV    2

typedef struct DehazeParameters
{

	int block = 40;				//window size for dark prior dehaze
	double w = 0.9;				//haze preserve coefficients, 1 means completely remove haze, 0 means contain only haze
	int air_threshold = 230;	//Limit for atmosphere evaluation,
	int mode = MODE_RGB;		//only RGB mode result in dehazing image, other modes are not used because of bad performance
}DehazeParameters;

typedef struct LevelParameters
{
	double dCustomGamma = 1.0;	//level adjust use gamma correction method, default value is 1.
}LevelParameters;

typedef struct Parameters
{
	Parameters()
	{
		dp = new DehazeParameters();
		lp = new LevelParameters();
	}

	~Parameters()
	{
		FREE_POINTER(dp);
		FREE_POINTER(lp);
	}

	DehazeParameters* dp = NULL;
	LevelParameters*  lp = NULL;
}Parameters;


_DHZ_LEVEL_API long dehaze(const char* SrcPath, const char* DstPath, DehazeParameters *pDP);
/*=================================================================
* Function Name：dehaze

* Function Description: dehaze a image using dark prior method
						use guided filter to process the transmission matrix
						evaluate atmosphere by average over all pixels that has high values

* Input Parameters:		const char *SrcPath	---input image path, can be a folder or .jpg image
						const char *DstPath	---output image folder path, this function will create a 
						folder named DstPath to store dehazed image
						DehazeParameters *dp---parameters that define how to dehaze
* Output Parameters:	0	means succeed
						-1	means failed

* Author：丁枫
* Date: 2016-6-22
=================================================================
**/



_DHZ_LEVEL_API long autoColorLevel(const char* SrcPath, const char* DstPath, LevelParameters* lp);
/*=================================================================
* Function Name：autoColorLevel

* Function Description: do color level adjust using predefined parameters, essentially this is gamma correction
						shadow input level is set to RGB value that cover 0.5% pixels of the histogram
						highlight input level is set to RGB value that cover 99.5% pixels of the histogram
						output levels are set to 0 and 255 
						gamma value can be set in Level Parameters

* Input Parameters:		const char *SrcPath	---input image path, can be a folder or .jpg image
						const char *DstPath	---output image folder path, this function will create a 
						folder named DstPath to store dehazed image
						LevelParameters* lp ---parameters that define how to adjust color level
* Output Parameters:	0	means succeed
						-1	means failed

* Author：丁枫
* Date: 2016-6-22
=================================================================
**/


_DHZ_LEVEL_API long Dehaze_ColorLevel(const char* SrcPath, const char* DstPath, Parameters* pP);
/*=================================================================
* Function Name：Dehaze_ColorLevel

* Function Description: Do dehaze operation first then do color level adjustment, this function
						will call above two functions. 

* Input Parameters:		const char *SrcPath	---input image path, can be a folder or .jpg image
						const char *DstPath	---output image folder path, this function will create a 
						folder named DstPath to store dehazed image
						Parameters* lp ---parameters that contain DehazeParameters and LevelParameters
* Output Parameters:	0	means succeed
						-1	means failed

* Author：丁枫
* Date: 2016-6-22
=================================================================
**/


#endif