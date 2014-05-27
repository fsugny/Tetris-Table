
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "gpio_utils.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void initOutputPin( uint16_t GPIO_PORT, uint16_t GPIO_PIN )
{
    GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd( GPIO_PORT_PERIPH_ADDR( GPIO_PORT ), ENABLE );
	GPIO_InitStruct.GPIO_Pin = GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( GPIO_PORT_ADDR( GPIO_PORT ), &GPIO_InitStruct );
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void initInputPin( uint16_t GPIO_PORT, uint16_t GPIO_PIN )
{
    GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd( GPIO_PORT_PERIPH_ADDR( GPIO_PORT ), ENABLE );
	GPIO_InitStruct.GPIO_Pin = GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init( GPIO_PORT_ADDR( GPIO_PORT ), &GPIO_InitStruct );
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void    setPin( uint16_t GPIO_PORT, uint16_t GPIO_PIN, uint8_t value )
{
    if ( value )
    {
        ((GPIO_TypeDef *) (GPIO_PORT_ADDR(GPIO_PORT)))->BSRRL = GPIO_PIN;
    }
    else
    {
        ((GPIO_TypeDef *) (GPIO_PORT_ADDR(GPIO_PORT)))->BSRRH = GPIO_PIN;
    }
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

uint8_t getPin( uint16_t GPIO_PORT, uint16_t GPIO_PIN )
{
     return (((GPIO_TypeDef *) (GPIO_PORT_ADDR(GPIO_PORT)))->IDR & GPIO_PIN);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
