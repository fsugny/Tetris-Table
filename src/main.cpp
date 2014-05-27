
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "stm32f4xx_conf.h"
extern "C" {
#include "sdio.h"
}
#include "snes_gamepad.h"
#include "gpio_utils.h"
#include "hd44780.h"
#include <stdio.h>

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

// Gamepad classes
snesGamepad * gGamePad1;
snesGamepad * gGamePad2;

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

int main(void)
{
// SD Card Test
#if 1
    printf("SD Card Test\n");

    initUsart();
    printf("Init\n");

    // NVIC Configuration
    NVIC_Configuration();
    printf("NVIC Configuration\n");

    if ((Status = SD_Init()) != SD_OK)
    {
        printf("SD_Init failed\n");
    }
    else
    {
        printf("SD_Init OK\n");
    }

    while ( (Status == SD_OK) && (uwSDCardOperation != SD_OPERATION_END) && (SD_Detect () == SD_PRESENT) )
    {
        switch (uwSDCardOperation)
        {
            //-------------------------- SD Single Block Test ---------------------
            case (SD_OPERATION_BLOCK):
            {
                    SD_SingleBlockTest();
                    uwSDCardOperation = SD_OPERATION_ERASE;
                    break;
            }
            //-------------------------- SD Erase Test ----------------------------
            case (SD_OPERATION_ERASE):
            {
                    SD_EraseTest();
                    uwSDCardOperation=SD_OPERATION_MULTI_BLOCK;
                    break;
            }
            //-------------------------- SD Multi Blocks Test ---------------------
            case (SD_OPERATION_MULTI_BLOCK):
            {
                    SD_MultiBlockTest();
                    uwSDCardOperation=SD_OPERATION_END;
                    break;
            }
        }
    }

    while (1)
    {

    }
#endif

// Gamepad test
{
#if 0
    // Init gamepads
    gGamePad1 = new snesGamepad( GPIO_PORT_D, GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2 );
    gGamePad2 = new snesGamepad( GPIO_PORT_D, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5 );

    uint16_t gamePad1_buttons, gamePad2_buttons;

    initOutputPin( GPIO_PORT_D, GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12 );


    // Main loop
    while (1)
    {
        // Get buttons from the gamepads
        gamePad1_buttons = gGamePad1->getButtons();
        gamePad2_buttons = gGamePad2->getButtons();

        if ( gamePad1_buttons & SNES_X )
            setPin( GPIO_PORT_D, GPIO_Pin_12, 1 );
        else
            setPin( GPIO_PORT_D, GPIO_Pin_12, 0 );
        if ( gamePad1_buttons & SNES_B )
            setPin( GPIO_PORT_D, GPIO_Pin_14, 1 );
        else
            setPin( GPIO_PORT_D, GPIO_Pin_14, 0 );
        if ( gamePad1_buttons & SNES_Y )
            setPin( GPIO_PORT_D, GPIO_Pin_15, 1 );
        else
            setPin( GPIO_PORT_D, GPIO_Pin_15, 0 );
        if ( gamePad1_buttons & SNES_A )
            setPin( GPIO_PORT_D, GPIO_Pin_13, 1 );
        else
            setPin( GPIO_PORT_D, GPIO_Pin_13, 0 );
        Delay(30000L);
    };

    delete gGamePad1;
    delete gGamePad2;
#endif
}

// HD44780 Test
{

#if 0
    initOutputPin( GPIO_PORT_E, GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9 );
    setPin( GPIO_PORT_D, GPIO_Pin_13, 1 );
    lcd = new lcd_hd44780(  GPIO_PORT_ADDR(GPIO_PORT_E),
                            GPIO_Pin_9,     // rs
                            GPIO_Pin_10,    // rw
                            GPIO_Pin_11,    // e
                            GPIO_Pin_12,    // db4
                            GPIO_Pin_13,    // db5
                            GPIO_Pin_14,    // db6
                            GPIO_Pin_15,    // db7
                            lcd_hd44780::HD44780_LINES_2,
                            lcd_hd44780::HD44780_FONT_5x10 );

    Delay(10000000);
    lcd->position(0,0);
    lcd->print("Hello World!");
    lcd->position(1,0);
    lcd->print("Counter : ");
    setPin( GPIO_PORT_D, GPIO_Pin_14, 1 );
    char counter = 0;
    while(1)
    {
        setPin( GPIO_PORT_D, GPIO_Pin_12, 1 );
        lcd->position(1,10);
        lcd->printf("%d",counter);
        setPin( GPIO_PORT_D, GPIO_Pin_12, 0 );
        Delay(1000000);
        counter++;
    }
#endif
}
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
