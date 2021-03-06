#include "SignalProcessing.h"

//创建队列并初始化   
QUEUE_ARRAY *CreateQueue(WORD max)
{
	QUEUE_ARRAY *queue = NULL;
	WORD i;
	queue = (QUEUE_ARRAY*)malloc(sizeof(QUEUE_ARRAY));
	queue->array = (ElementType*)malloc(sizeof(ElementType)*max);
	if (queue->array == NULL)
	{
		printf("Memory error！");
		exit(-1);
	}
	else
	{
		queue->rear = max - 1;  //队尾索引
		queue->maxSize = max; //队列大小
		for (i = 0; i < max; i++)
		{
			queue->array[i] = 0; //初始化所有元素为0
		}
		return queue;
	}
}
//更新队列
void RefreshQueue(QUEUE_ARRAY *queue, ElementType data)
{
	queue->rear = (++queue->rear) % queue->maxSize;   //当前队列尾位置+1
	queue->array[queue->rear] = data;                //新加入数据放入队尾位置
}
//释放队列
void FreeQueue(QUEUE_ARRAY *queue)
{
	if (queue->array)
	{
		free(queue->array);
		queue->array = NULL;
	}
	if (queue)
	{
		free(queue);
		queue = NULL;
	}
}

//
NODE_INDEX *CreateNodeIdx(void)
{
	NODE_INDEX *nodeIdx = NULL;
	nodeIdx = (NODE_INDEX*)malloc(sizeof(NODE_INDEX));
	//初始化索引位置
	nodeIdx->xDelay1 = RRS_FORWARD_DELAY;
	nodeIdx->xDelay2 = 0;
	nodeIdx->xFP = RRS_FORWARD_DELAY + RRS_BACKWARD_DELAY;
	nodeIdx->yDelay1 = RRS_BACKWARD_DELAY;
	nodeIdx->yDelay2 = 0;
	return nodeIdx;
}
void UpdateNode(NODE_INDEX *nodeIdx)
{
	nodeIdx->xDelay1 = (++nodeIdx->xDelay1) % (RRS_FORWARD_DELAY << 1);
	nodeIdx->xDelay2 = (++nodeIdx->xDelay2) % (RRS_FORWARD_DELAY << 1);
	nodeIdx->xFP = (++nodeIdx->xFP) % (RRS_FORWARD_DELAY << 1);
	nodeIdx->yDelay1 = (++nodeIdx->yDelay1) % (RRS_BACKWARD_DELAY << 1);
	nodeIdx->yDelay2 = (++nodeIdx->yDelay2) % (RRS_BACKWARD_DELAY << 1);
}
void FreeNode(NODE_INDEX *nodeIdx)
{
	if (nodeIdx)
	{
		free(nodeIdx);
		nodeIdx = NULL;
	}
}

INPUT_TYPE RrsFilter(const INPUT_TYPE input, QUEUE_ARRAY* forward, QUEUE_ARRAY* backward, NODE_INDEX *nodeIdx)
{
	INPUT_TYPE xSum = input - (forward->array[nodeIdx->xDelay1] << 1) + forward->array[nodeIdx->xDelay2];
	INPUT_TYPE ySum = (backward->array[nodeIdx->yDelay1] << 1) - backward->array[nodeIdx->yDelay2];
	INPUT_TYPE tmpOut = xSum + ySum;
	INPUT_TYPE outPut = forward->array[nodeIdx->xFP] - (tmpOut >> RRS_SHIFT_LENGTH);
	RefreshQueue(forward, input);
	RefreshQueue(backward, tmpOut);
	UpdateNode(nodeIdx);
	return outPut;
}

INPUT_TYPE RrsFilterFloatAmp(const INPUT_TYPE input, QUEUE_ARRAY* forward, QUEUE_ARRAY* backward, NODE_INDEX *nodeIdx)
{
	INPUT_TYPE xSum = input - (forward->array[nodeIdx->xDelay1] << 1) + forward->array[nodeIdx->xDelay2];
	INPUT_TYPE ySum = (backward->array[nodeIdx->yDelay1] << 1) - backward->array[nodeIdx->yDelay2];
	INPUT_TYPE tmpOut = xSum + ySum;
	INPUT_TYPE outPut = (INPUT_TYPE)(forward->array[nodeIdx->xFP] - ((float)tmpOut * RRS_AMPLITUDE));
	RefreshQueue(forward, input);
	RefreshQueue(backward, tmpOut);
	UpdateNode(nodeIdx);
	return outPut;
}

INPUT_TYPE IfirFliter(const INPUT_TYPE input, QUEUE_ARRAY* buff1, QUEUE_ARRAY* buff2, const LPF_INTFC* coeff1, const LPF_INTFC* coeff2)
{
	INPUT_TYPE outPut = 0;
	INPUT_TYPE tmpOut = 0;
	long long tmpRst = 0; //64bit
	WORD idx = 0;
	WORD i = 0;


	idx = buff1->rear;
	for (i = 0; i < FIR_1ST_LENGTH; i++)
	{
		if (i < (FIR_1ST_LENGTH >> 1))
		{
			tmpRst += buff1->array[idx] * coeff1[i];
		}
		else
		{
			tmpRst += buff1->array[idx] * coeff1[FIR_1ST_LENGTH - i - 1];
		}
		idx = (idx + buff1->maxSize - 1) % buff1->maxSize;
	}
	tmpOut = (INPUT_TYPE)(tmpRst >> FILT_COEFF_FRECTION_BIT);

	tmpRst = 0;
	idx = buff2->rear;
	for (i = 0; i < FIR_2ND_LENGTH; i++)
	{
		if (i < (FIR_2ND_LENGTH >> 1) )
		{
			tmpRst += buff2->array[idx] * coeff2[i];
		}
		else
		{
			tmpRst += buff2->array[idx] * coeff2[FIR_2ND_LENGTH - i - 1];
		}
		idx = (idx + buff2->maxSize - INTERP_FACTOR) % buff2->maxSize;
	}
	outPut = (INPUT_TYPE)(tmpRst >> FILT_COEFF_FRECTION_BIT);

	RefreshQueue(buff1, input);
	RefreshQueue(buff2, tmpOut);
	return outPut;
}

INPUT_TYPE firFliter(const INPUT_TYPE input, QUEUE_ARRAY* buff, const LPF_INTFC* coeff)
{
	INPUT_TYPE outPut = 0;
	long long tmpRst = 0; //64bit
	WORD idx = 0;
	WORD i = 0;


	idx = buff->rear;
	for (i = 0; i < LPF_LENGTH; i++)
	{
		if (i < (LPF_LENGTH >> 1))
		{
			tmpRst += buff->array[idx] * coeff[i];
		}
		else
		{
			tmpRst += buff->array[idx] * coeff[LPF_LENGTH - i - 1];
		}
		idx = (idx + buff->maxSize - 1) % buff->maxSize;
	}
	outPut = (INPUT_TYPE)(tmpRst >> FILT_COEFF_FRECTION_BIT);
	RefreshQueue(buff, input);
	return outPut;
}

INPUT_TYPE iirFliter(const INPUT_TYPE input, INPUT_TYPE* forward, INPUT_TYPE* backward)
{
	INPUT_TYPE outPut = 0;
	outPut = input - (*forward) + ((*backward*HPF_COEFF) >> FILT_COEFF_FRECTION_BIT);
	*forward = input;
	*backward = outPut;
	return outPut;
}