#ifndef _READFILES_
#define _READFILES_

#define _DHZ_LEVEL_EXPORTS

#include<fstream>
#include<io.h>
#include<iostream>
#include<vector>
#include<string>
#include"Dhz_Level.h"

_DHZ_LEVEL_API long getJPGs(std::string path, std::vector<std::string>& files);
#endif // HEAP