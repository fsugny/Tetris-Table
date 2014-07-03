
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include "stm32f4xx_conf.h"
#include "ff.h"
#include "diskio.h"
#include "snes_gamepad.h"
#include "gpio_utils.h"
#include "hd44780.h"
#include "music_nz1.h"
#include "ymplayer.h"
#include <stdio.h>

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

// Gamepad classes
snesGamepad * gGamePad1;
snesGamepad * gGamePad2;

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

int16_t tp[] = {//Frequencies related to MIDI note numbers
  15289, 14431, 13621, 12856, 12135, 11454, 10811, 10204,//0-o7
  9631, 9091, 8581, 8099, 7645, 7215, 6810, 6428,//8-15
  6067, 5727, 5405, 5102, 4816, 4545, 4290, 4050,//16-23
  3822, 3608, 3405, 3214, 3034, 2863, 2703, 2551,//24-31
  2408, 2273, 2145, 2025, 1911, 1804, 1703, 1607,//32-39
  1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012,//40-47
  956, 902, 851, 804, 758, 716, 676, 638,//48-55
  602, 568, 536, 506, 478, 451, 426, 402,//56-63
  379, 358, 338, 319, 301, 284, 268, 253,//64-71
  239, 225, 213, 201, 190, 179, 169, 159,//72-79
  150, 142, 134, 127, 119, 113, 106, 100,//80-87
  95, 89, 84, 80, 75, 71, 67, 63,//88-95
  60, 56, 53, 50, 47, 45, 42, 40,//96-103
  38, 36, 34, 32, 30, 28, 27, 25,//104-111
  24, 22, 21, 20, 19, 18, 17, 16,//112-119
  15, 14, 13, 13, 12, 11, 11, 10,//120-127
  0//off
};

int16_t song[][2] ={
{60,500},
{62,500},
{64,500},
{65,500},
{64,500},
{62,500},
{60,500},
{128,500},
{64,500},
{65,500},
{67,500},
{69,500},
{67,500},
{65,500},
{64,500},
{128,500},
{60,500},
{128,500},
{60,500},
{128,500},
{60,500},
{128,500},
{60,500},
{128,500},
{60,128},
{128,128},
{60,128},
{128,128},
{62,128},
{128,128},
{62,128},
{128,128},
{64,128},
{128,128},
{64,128},
{128,128},
{65,128},
{128,128},
{65,128},
{128,128},
{64,250},
{128,250},
{62,250},
{128,250},
{60,250},
{128,1000}
};

void modeInactive()
{
    setPin( GPIO_PORT_B, GPIO_Pin_0, 0 );
    setPin( GPIO_PORT_B, GPIO_Pin_2, 0 );
};

void modeWrite()
{
    setPin( GPIO_PORT_B, GPIO_Pin_0, 0 );
    setPin( GPIO_PORT_B, GPIO_Pin_2, 1 );
};

void modeLatch()
{
    setPin( GPIO_PORT_B, GPIO_Pin_0, 1 );
    setPin( GPIO_PORT_B, GPIO_Pin_2, 1 );
};

void write_data(unsigned char address, unsigned char data)
{
    modeInactive();
    setPin( GPIO_PORT_E, GPIO_Pin_8, address & 0x01 );
    setPin( GPIO_PORT_E, GPIO_Pin_9, address & 0x02 );
    setPin( GPIO_PORT_E, GPIO_Pin_10, address & 0x04 );
    setPin( GPIO_PORT_E, GPIO_Pin_11, address & 0x08 );
    setPin( GPIO_PORT_E, GPIO_Pin_12, address & 0x10 );
    setPin( GPIO_PORT_E, GPIO_Pin_13, address & 0x20 );
    setPin( GPIO_PORT_E, GPIO_Pin_14, address & 0x40 );
    setPin( GPIO_PORT_E, GPIO_Pin_15, address & 0x80 );
    modeLatch();
    modeInactive();
    modeWrite();
    setPin( GPIO_PORT_E, GPIO_Pin_8, data & 0x01 );
    setPin( GPIO_PORT_E, GPIO_Pin_9, data & 0x02 );
    setPin( GPIO_PORT_E, GPIO_Pin_10, data & 0x04 );
    setPin( GPIO_PORT_E, GPIO_Pin_11, data & 0x08 );
    setPin( GPIO_PORT_E, GPIO_Pin_12, data & 0x10 );
    setPin( GPIO_PORT_E, GPIO_Pin_13, data & 0x20 );
    setPin( GPIO_PORT_E, GPIO_Pin_14, data & 0x40 );
    setPin( GPIO_PORT_E, GPIO_Pin_15, data & 0x80 );
    modeLatch();
}

void set_chA(int i)
{
  write_data(0x00, tp[i]&0xff);
  write_data(0x01, (tp[i] >> 8)&0x0f);
}

void delayms(uint32_t ms)
{
    ms *= 3360;
    while(ms--)
    {
        __NOP();
    }
}

int main(void)
{
// AY-3-8910 Test
#if 1
    printf("Init Pins\n");
    // BC1 / BC2 / BCDIR
    initOutputPin( GPIO_PORT_B, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 );
    // DA0-Da7
    initOutputPin( GPIO_PORT_E, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 );

    printf("Setup Mixer\n");
    setPin( GPIO_PORT_B, GPIO_Pin_1, 1 );

    ymPlayer * ym = new ymPlayer();
    ym->openFileFromMemory(musicdata);

    printf("Loop\n");
    ym->printInfo();
    while(1)
    {
        for(uint32_t i=0;i<ym->m_header.nbFrames;i++)
        {
            for (uint8_t r=0;r<16;++r)
                write_data(r,ym->m_frameRegister[16*i+r]);
            delayms(75);
        }
        printf("End of song\n");
    }

    delete ym;
#endif
// SD Card Test
{
#if 0

    DSTATUS errd;                           //error var for Disk
    FRESULT errf;                            //error var vor FAT
    FATFS Fatfs;                           //Fat file system var
    FIL file1;                                       //File var
    printf("SD Card Test\n");

    errd = disk_initialize(0);
    errf = f_mount(&Fatfs, "0:", 1 );

    printf("TETRIS.TXT : \n");
    f_open(&file1, "/TETRIS.TXT", FA_READ);

    int i=0;
    char buffer[1024];
    while ( !f_eof(&file1) )
    {
        if ( f_gets( buffer, 1024, &file1 ) )
            printf("%s\n",buffer);
    }
    f_close( &file1 );

    int res;
    errf=f_open(&file1, "/FOO.TXT", FA_WRITE|FA_CREATE_ALWAYS);
    if ( errf != FR_OK )
        printf("Error Opening file to write : %d\n", errf);
    res = f_puts("Test SD Card\n", &file1);
    res = f_puts("Write\n", &file1);
    f_close( &file1 );

    printf("FOO.TXT : \n");
    f_open(&file1, "/FOO.TXT", FA_READ);
    while ( !f_eof(&file1) )
    {
        if ( f_gets( buffer, 1024, &file1 ) )
            printf("%s\n",buffer);
    }
    f_close( &file1 );

    while (1)
    {

    }
#endif
}

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
