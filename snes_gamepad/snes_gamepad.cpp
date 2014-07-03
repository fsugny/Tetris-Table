
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "stm32f4xx_conf.h"

#include "snes_gamepad.h"
#include "gpio_utils.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

snesGamepad::snesGamepad( uint16_t GPIO_PORT,
                          uint16_t PIN_CLK,
                          uint16_t PIN_LATCH,
                          uint16_t PIN_DATA )
{
    initOutputPin( GPIO_PORT, PIN_CLK | PIN_LATCH );
    initInputPin( GPIO_PORT, PIN_DATA );

    m_gpio_port = GPIO_PORT;
	m_pin_clk = PIN_CLK;
	m_pin_latch = PIN_LATCH;
	m_pin_data = PIN_DATA;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

uint16_t snesGamepad::getButtons()
{
    uint16_t result = 0x0000;

    // Latch = 1
    //GPIOD->BSRRH = 0x1 << m_pin_latch;
    setPin( m_gpio_port, m_pin_latch, 0x01 );
    // Latch = 0
    //GPIOD->BSRRL = 0x1 << m_pin_latch;
    setPin( m_gpio_port, m_pin_latch, 0x00 );

    for ( int i=0; i<12; ++i )
    {
        // Get a bit from the data line
        //result |= (GPIOD->IDR & ( 0x1 << m_pin_data ) );
        if ( getPin( m_gpio_port, m_pin_data ) )
            result |= 0x01 << i;

        // Clk = 1
        //GPIOD->BSRRH = 0x1 << m_pin_clk;
        setPin( m_gpio_port, m_pin_clk, 1 );
        // Clk = 0
        //GPIOD->BSRRL = 0x1 << m_pin_clk;
        setPin( m_gpio_port, m_pin_clk, 0 );
    }


    return ~result;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
