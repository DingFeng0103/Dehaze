#ifndef _DEHAZE_H
#define _DEHAZE_H

#define _DHZ_LEVEL_EXPORTS

#include"heap.h"
#include"guidedfilter.h"
#include"Dhz_Level.h"
#include<string>
#include<cv.h>
#include<highgui.h>
#include<io.h>

_DHZ_LEVEL_API IplImage * dehaze(IplImage *IplSrc, DehazeParameters* pDP);

#endif
