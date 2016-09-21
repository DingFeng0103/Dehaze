#ifndef _EXEC_
#define _EXEC_

#define _DHZ_LEVEL_EXPORTS

#include<vector>
#include<string>
#include<stdio.h>
#include<conio.h>
#include"readfiles.h"
#include"dehaze.h"
#include"leveladjust.h"
#include"Dhz_Level.h"

_DHZ_LEVEL_API long dehaze(const char* SrcPath, const char* DstPath, DehazeParameters *pDP);
_DHZ_LEVEL_API long autoColorLevel(const char* SrcPath, const char* DstPath, LevelParameters* pLP);
_DHZ_LEVEL_API long Dehaze_ColorLevel(const char* SrcPath, const char* DstPath, Parameters* pP);
_DHZ_LEVEL_API long checkOutputFolder(const char* DstPath);

#endif