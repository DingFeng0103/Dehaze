#ifndef _HEAP_
#define _HEAP_

#define _DHZ_LEVEL_EXPORTS

#include<cv.h>
#include<highgui.h>
#include"Dhz_Level.h"

class Heap{
	float weight;
	int index;
};


_DHZ_LEVEL_API bool AdjHeap(int* weight, int i, int len);
_DHZ_LEVEL_API void MinHeap(int* weight, int len);
_DHZ_LEVEL_API bool HeapK(IplImage* src, int k, int* dst);
#endif // HEAP

