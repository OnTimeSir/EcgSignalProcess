#pragma once

#include <stdlib.h> 
#include <stdio.h> 
#define RRS_FORWARD_DELAY 250 //225  320
#define RRS_BACKWARD_DELAY 5
#define RRS_SHIFT_LENGTH 12
#define RRS_AMPLITUDE 0.0004f //0.0004938272f 

#define FIR_1ST_LENGTH 21
#define FIR_2ND_LENGTH 44
#define INTERP_FACTOR 2

#define INPUT_FRECTION_BIT 15
#define OUTPUT_FRECTION_BIT 12 //��Χ��-8~+8��

#define OUTPUT_MAX_VALUE 32767
#define OUTPUT_MIN_VALUE -32768

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef int INPUT_TYPE;     //����
typedef short OUTPUT_TYPE;  //���
typedef short LPF_INTFC;    //��ͨϵ��

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


//�źŴ���ӿں���
OUTPUT_TYPE SignalProcess(const INPUT_TYPE inputData, QUEUE_ARRAY* rrsBuf1, QUEUE_ARRAY* rrsBuf2, NODE_INDEX* nodeIdx, QUEUE_ARRAY*ifirBuf1, QUEUE_ARRAY*ifirBuf2, const LPF_INTFC* fir1Coeff, const LPF_INTFC* fir2Coeff);

//��ϵ���ݹ��ͨ&&��Ƶ�ݲ�
INPUT_TYPE RrsFilter(const INPUT_TYPE input, QUEUE_ARRAY* forward, QUEUE_ARRAY* backward, NODE_INDEX *nodeIdx);
INPUT_TYPE RrsFilterFloatAmp(const INPUT_TYPE input, QUEUE_ARRAY* forward, QUEUE_ARRAY* backward, NODE_INDEX *nodeIdx);

//2��fir�����ṹ�ĵ�ͨ�˲�
INPUT_TYPE IfirFliter(const INPUT_TYPE input, QUEUE_ARRAY* buff1, QUEUE_ARRAY* buff2, const LPF_INTFC* coeff1, const LPF_INTFC* coeff2);

QUEUE_ARRAY *CreateQueue(WORD max);
void RefreshQueue(QUEUE_ARRAY *queue, ElementType data);
void FreeQueue(QUEUE_ARRAY *queue);
NODE_INDEX *CreateNodeIdx(void);
void UpdateNode(NODE_INDEX *nodeIdx);
void FreeNode(NODE_INDEX *nodeIdx);