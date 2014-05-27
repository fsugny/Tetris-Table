
#ifndef __SDIO_H
#define __SDIO_H

#include <stm32f4xx.h>
#include <stdio.h>
#include "sdio_high_level.h"
#include "logf.h"

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

#define BLOCK_SIZE            512       /* Block Size in Bytes */
#define NUMBER_OF_BLOCKS      100       /* For Multi Blocks operation (Read/Write) */
#define MULTI_BUFFER_SIZE    (BLOCK_SIZE * NUMBER_OF_BLOCKS)

#define SD_OPERATION_ERASE          0
#define SD_OPERATION_BLOCK          1
#define SD_OPERATION_MULTI_BLOCK    2
#define SD_OPERATION_END            3

uint8_t aBuffer_Block_Tx[BLOCK_SIZE];
uint8_t aBuffer_Block_Rx[BLOCK_SIZE];
uint8_t aBuffer_MultiBlock_Tx[MULTI_BUFFER_SIZE];
uint8_t aBuffer_MultiBlock_Rx[MULTI_BUFFER_SIZE];
extern TestStatus EraseStatus;
extern TestStatus TransferStatus1;
extern TestStatus TransferStatus2;

extern SD_Error Status;
extern uint32_t uwSDCardOperation;

void initUsart (void);
void NVIC_Configuration (void);
void SD_EraseTest (void);
void SD_SingleBlockTest (void);
void SD_MultiBlockTest (void);
void Fill_Buffer (uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset);

TestStatus Buffercmp (uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);
TestStatus eBuffercmp (uint8_t* pBuffer, uint32_t BufferLength);

#endif
