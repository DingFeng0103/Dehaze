#include"stdafx.h"
#include"exec.h"


long dehaze(const char* SrcPath, const char* DstPath, DehazeParameters *pDP)
{
	std::string strSrcPath(SrcPath);
	std::string strDstPath(DstPath);

	if (0 != checkOutputFolder(DstPath))
	{
		return -1;
	}

	std::vector<std::string> vecFileList;
	if (0 != getJPGs(SrcPath, vecFileList))
	{
		std::cout << "Error in getJPGs: can't find jpg images" << std::endl;
		return -1;
	}

	int tmp = 0;
	std::cout << "Total image numbers: " << vecFileList.size() << std::endl;
	std::string tmpDstPath;
	for (int i = 0; i < vecFileList.size(); i++)
	{
		//get output file name
		tmpDstPath.clear();
		tmp = vecFileList[i].find_last_of("\\");
		if (tmp == -1){
			tmp = 0;
			tmpDstPath = strDstPath + "\\" + vecFileList[i];
		}
		else{
			tmpDstPath = strDstPath + vecFileList[i].substr(tmp, vecFileList[i].size());
		}
		//dehaze
		std::cout << "now dehaze " << i << "-th image: " << vecFileList.at(i) << std::endl;
		IplImage* IplSrcImage = cvLoadImage(vecFileList.at(i).data(), 1);
		IplImage* IplDstImage = dehaze(IplSrcImage, pDP);
		//save image
		cvSaveImage(tmpDstPath.data(), IplDstImage);
		tmp = 0;
		cvReleaseImage(&IplSrcImage);
		cvReleaseImage(&IplDstImage);

	}

	return 0;
}

long autoColorLevel(const char* SrcPath, const char* DstPath, LevelParameters* pLP)
{
	std::string strSrcPath(SrcPath);
	std::string strDstPath(DstPath);

	if (0 != checkOutputFolder(DstPath))
	{
		return -1;
	}

	std::vector<std::string> vecFileList;
	if (0 != getJPGs(SrcPath, vecFileList))
	{
		std::cout << "Error in getJPGs: can't find jpg images" << std::endl;
		return -1;
	}

	int tmp = 0;
	std::cout << "Total image numbers: " << vecFileList.size() << std::endl;

	std::string tmpDstPath;
	cv::Mat matSrcImage;
	cv::Mat matDstImage;
	for (int i = 0; i < vecFileList.size(); i++)
	{
		
		//get output file name
		tmpDstPath.clear();
		tmp = vecFileList[i].find_last_of("\\");
		if (tmp == -1){
			tmp = 0;
			tmpDstPath = strDstPath + "\\" + vecFileList[i];
		}
		else{
			tmpDstPath = strDstPath + vecFileList[i].substr(tmp, vecFileList[i].size());
		}
		//dehaze
		std::cout << "now adjust color level " << i << "-th image: " << vecFileList.at(i) << std::endl;
		matSrcImage = cv::imread(vecFileList.at(i));

		if (0==level_adjust_auto(matSrcImage, matDstImage, pLP))
		{
			matSrcImage.release();
			matDstImage.release();
			cv::imwrite(tmpDstPath, matDstImage);
			tmp = 0;
		}
		else
		{
			return -1;
		}
		//save image
	}
	return 0;
}

long Dehaze_ColorLevel(const char* SrcPath, const char* DstPath, Parameters* pP)
{
	std::string strSrcPath(SrcPath);
	std::string strDstPath(DstPath);

	if (0!=checkOutputFolder(DstPath))
	{
		return -1;
	}

	std::vector<std::string> vecFileList;
	if (0 != getJPGs(SrcPath, vecFileList))
	{
		std::cout << "Error in getJPGs: can't find jpg images" << std::endl;
		return -1;
	}

	int tmp = 0;
	std::cout << "Total image numbers: " << vecFileList.size() << std::endl;

	std::string tmpDstPath;
	IplImage* IplSrcImage = NULL;
	IplImage* IplDstImage = NULL;
	cv::Mat matSrcImage;
	cv::Mat matDstImage;
	for (int i = 0; i < vecFileList.size(); i++)
	{
		//get output file name
		tmpDstPath.clear();
		tmp = vecFileList[i].find_last_of("\\");
		if (tmp == -1){
			tmp = 0;
			tmpDstPath = strDstPath + "\\" + vecFileList[i];
		}
		else{
			tmpDstPath = strDstPath + vecFileList[i].substr(tmp, vecFileList[i].size());
		}
		//dehaze
		std::cout << "now dehaze " << i << "-th image: " << vecFileList.at(i) << std::endl;
		IplSrcImage = cvLoadImage(vecFileList.at(i).data(), 1);
		IplDstImage = dehaze(IplSrcImage, pP->dp);
		matSrcImage=cv::Mat(IplDstImage);
		std::cout << "now adjust color level for: " << i << "-th image: " << vecFileList.at(i) << std::endl;
		if (0 == level_adjust_auto(matSrcImage, matDstImage, pP->lp))
		{
			cv::imwrite(tmpDstPath, matDstImage);
			cvReleaseImage(&IplSrcImage);
			cvReleaseImage(&IplDstImage);
			matSrcImage.release();
			matDstImage.release();
			tmp = 0;
		}
		else
		{
			return -1;
		}
		//save image
	}
	return 0;


}

long checkOutputFolder(const char* DstPath)
{
	std::string strDstPath(DstPath);
	//if the outputfolder doesn't exist, create one
	//must convert to wchar_t in order to use MultiByteToWideChar
	__int64   hFile = 0;
	struct __finddata64_t fileinfo;
	std::string p;

	hFile = _findfirst64(DstPath, &fileinfo);
	if (hFile == -1L)
	{
		std::string mkdircmd("md ");
		mkdircmd += strDstPath;
		system(mkdircmd.data());
	}
	else
	{
		std::cout << "folder: " << strDstPath << " already exist." << std::endl;
	}
	return 0;
}
