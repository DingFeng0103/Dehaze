#include"stdafx.h"
#include"readfiles.h"



long getJPGs(std::string path, std::vector<std::string>& files)
{
	//get all JPGs in the folder and its subfolder 
	// store the file names in files
	__int64   hFile = 0;
	//文件信息  
	struct __finddata64_t fileinfo;
	std::string strTmp;

	hFile = _findfirst64(path.c_str(), &fileinfo);
	if (hFile == -1L)
	{
		std::cout << "Error： No such file or directory: " << path <<std::endl;
		return -1;
	}

	if (fileinfo.attrib & _A_SUBDIR)
	{
		if ((hFile = _findfirst64(strTmp.assign(path).append("\\*.jpg").c_str(), &fileinfo)) != -1)
		{
			do
			{
				//如果是目录,迭代之  
				//如果不是,加入列表  
				if ((fileinfo.attrib &  _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						getJPGs(strTmp.assign(path).append("\\").append(fileinfo.name), files);
				}
				else
				{
					files.push_back(strTmp.assign(path).append("\\").append(fileinfo.name));
				}
			} while (_findnext64(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}
	else
	{
		files.push_back(path);

	}
	if (0 == files.size())
	{
		std::cout << "no .JPG file is found in " <<path<< std::endl;
		return 1;
	}
	else
	{
		return 0;
	}
	

}