#include"stdafx.h"
#include"heap.h"
bool HeapK(IplImage* src, int k, int* dst)
{
	//����Ԫ����һ�������ʾ, �ٰ��Ѳ�������
	//����ֻ����weightֵ
	int* weight=new int[k];
	memset(weight, 0, sizeof(int)*k);

	long i;

	//�Ƚ�ǰk ��Ԫ�ص�weight���뵽����
	for (i = 0; i<k; i++)
		weight[i] = (uchar)src->imageData[i];

	//����ΪС����,����Ƚ�
	MinHeap(weight, k);


	//����һ�μ���
	for (i = k; i<src->imageSize; i++)
	{
		if (weight[0] < (uchar)src->imageData[i])
		{

			//ÿ�û�һ�ζѶ���Ҫ���µ���
			weight[0] = (uchar)src->imageData[i];
			AdjHeap(weight, 0, k - 1);

		}
	}


	//���weight����Ϊǰk������Ԫ��,���Ҫ�������,��ôҪ��һ�ζ�����
	memcpy(dst, weight, sizeof(int)*k);

	return true;

}


bool AdjHeap(int * WArray, int i, int len)
{
	//������Ϊ��С��
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

	//���ݶѵ�����: ����Ϊ 2i +1 , �Һ�Ϊ 2i + 2( ��Ϊ��0��ʼ���� )

	int i;


	//�����һ���к��ӵĽ�㿪ʼ,��ǰ����
	len--;
	for (i = (len - 1) / 2; i >= 0; i--)
		AdjHeap(WArray, i, len);

}
