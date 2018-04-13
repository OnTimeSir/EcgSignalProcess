#pragma once

#include <stdlib.h> 
#include <stdio.h> 
#define RRS_FORWARD_DELAY 250 //225  320
#define RRS_BACKWARD_DELAY 5
#define RRS_SHIFT_LENGTH 12
#define RRS_AMPLITUDE 0.0004f //0.0004938272f 

#define FIR_1ST_LENGTH 10
#define FIR_2ND_LENGTH 34
#define INTERP_FACTOR 2

#define HPF_COEFF 16139
#define LPF_LENGTH 96

#define INPUT_FRECTION_BIT 12
#define OUTPUT_FRECTION_BIT 10 
#define FILT_COEFF_FRECTION_BIT 14

#define OUTPUT_MAX_VALUE 32767
#define OUTPUT_MIN_VALUE -32768

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef int INPUT_TYPE;     //输入
typedef short OUTPUT_TYPE;  //输出
typedef short LPF_INTFC;    //低通系数

typedef INPUT_TYPE ElementType;
typedef struct _QUEUE_ARRAY_
{
	WORD rear;
	WORD maxSize;
	ElementType *array;
}QUEUE_ARRAY;

typedef struct _NODE_INDEX_
{
	WORD xDelay1;
	WORD xDelay2;
	WORD xFP;
	WORD yDelay1;
	WORD yDelay2;
}NODE_INDEX;


//信号处理接口函数

//整系数递归高通&&工频陷波
INPUT_TYPE RrsFilter(const INPUT_TYPE input, QUEUE_ARRAY* forward, QUEUE_ARRAY* backward, NODE_INDEX *nodeIdx);         //仅+/- 移位操作
INPUT_TYPE RrsFilterFloatAmp(const INPUT_TYPE input, QUEUE_ARRAY* forward, QUEUE_ARRAY* backward, NODE_INDEX *nodeIdx); //1次浮点乘法

//2级fir级联结构的低通滤波
INPUT_TYPE IfirFliter(const INPUT_TYPE input, QUEUE_ARRAY* buff1, QUEUE_ARRAY* buff2, const LPF_INTFC* coeff1, const LPF_INTFC* coeff2);

//fir Direct-Form
INPUT_TYPE firFliter(const INPUT_TYPE input, QUEUE_ARRAY* buff, const LPF_INTFC* coeff);

//iir Direct-II (1-order)
INPUT_TYPE iirFliter(const INPUT_TYPE input, INPUT_TYPE* forward, INPUT_TYPE* backward);

QUEUE_ARRAY *CreateQueue(WORD max);
void RefreshQueue(QUEUE_ARRAY *queue, ElementType data);
void FreeQueue(QUEUE_ARRAY *queue);
NODE_INDEX *CreateNodeIdx(void);
void UpdateNode(NODE_INDEX *nodeIdx);
void FreeNode(NODE_INDEX *nodeIdx);