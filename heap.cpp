#include"stdafx.h"
#include"heap.h"
bool HeapK(IplImage* src, int k, int* dst)
{
	//所有元素以一个数组表示, 再按堆操作调整
	//这里只保存weight值
	int* weight=new int[k];
	memset(weight, 0, sizeof(int)*k);

	long i;

	//先将前k 个元素的weight加入到堆中
	for (i = 0; i<k; i++)
		weight[i] = (uchar)src->imageData[i];

	//调整为小根堆,方便比较
	MinHeap(weight, k);


	//遍历一次即可
	for (i = k; i<src->imageSize; i++)
	{
		if (weight[0] < (uchar)src->imageData[i])
		{

			//每置换一次堆顶就要重新调整
			weight[0] = (uchar)src->imageData[i];
			AdjHeap(weight, 0, k - 1);

		}
	}


	//最后weight数组为前k个最大的元素,如果要排序输出,那么要做一次堆排序
	memcpy(dst, weight, sizeof(int)*k);

	return true;

}


bool AdjHeap(int * WArray, int i, int len)
{
	//调整堆为最小堆
	bool change = false;
	int j, t;
	for (j = i * 2 + 1; j <= len; j = j * 2 + 1)
	{

		if (j + 1 <= len && WArray[j] > WArray[j + 1])
			j++;

		if (WArray[j] < WArray[(j - 1) / 2])
		{
			change = true;
			t = WArray[(j - 1) / 2];
			WArray[(j - 1) / 2] = WArray[j];
			WArray[j] = t;
		}

	}

	return change;
}

void MinHeap(int * WArray, int len)
{
	if (len <= 0)
		return;

	//根据堆的性质: 左孩子为 2i +1 , 右孩为 2i + 2( 因为从0开始计数 )

	int i;


	//从最后一个有孩子的结点开始,向前调整
	len--;
	for (i = (len - 1) / 2; i >= 0; i--)
		AdjHeap(WArray, i, len);

}
