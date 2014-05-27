//---------------------------------------------------------------------------------------------------------
// HD44780 Library
//---------------------------------------------------------------------------------------------------------

#ifndef __HD44780_H
#define __HD44780_H

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#include <stdbool.h>
#include "stm32f4xx_conf.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#define HD44780_QUEUE_FREQ  5000	/**< Update Frequency */
#define HD44780_QUEUE_SIZE  500		/**< Queue Size */

#define HD44780_TIMER 		TIM7
#define HD44780_PRIORITY 	6

#define NONE 0

#define HD44780_4BIT 		0x00
#define HDD44780_ENTRY_LEFT	0x02
#define HD44780_DISPLAY_ON	0x04
#define HD44780_CURSOR_ON	0x02
#define HD44780_BLINK_ON	0x01

#define HD44780_CLEAR		0x0001
#define HD44780_HOME		0x0002
#define HD44780_ENTRY		0x0004
#define HD44780_DISPLAY		0x0008
#define HD44780_CURSOR		0x0010
#define HD44780_FUNCTION	0x0020
#define HD44780_CGRAM		0x0040
#define HD44780_DGRAM		0x0080

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

class lcd_hd44780
{
public:
    typedef enum
    {
        HD44780_LINES_1 = 0x00,
        HD44780_LINES_2 = 0x08
    } hd44780_lines;

    typedef enum
    {
        HD44780_FONT_5x8 = 0x00,
        HD44780_FONT_5x10 = 0x04
    } hd44780_font;

    typedef struct
    {
        GPIO_TypeDef* gpio;
        int32_t rs;
        int32_t rw;
        int32_t e;
        int32_t db4;
        int32_t db5;
        int32_t db6;
        int32_t db7;
        int8_t lines;
        int8_t font;
    } hd44780_conf;

    typedef enum
    {
        HD44780_WAIT = 1,
        HD44780_WAIT_NOT_BUSY,
        HD44780_WRITE
    } hd44780_command;

    typedef struct
    {
        hd44780_command command;
        bool reg;
        int16_t data;
        int8_t nibble;
    } hd44780_task;

public:
    // Initialize a display
    lcd_hd44780( GPIO_TypeDef * port,
                 const uint16_t rs,
                 const uint16_t rw,
                 const uint16_t e,
                 const uint16_t db4,
                 const uint16_t db5,
                 const uint16_t db6,
                 const uint16_t db7,
                 const hd44780_lines lines,
                 const hd44780_font font );

    // Clear display
    void clear();

    // Move cursor to home position
    void home();

    // Set up display
    void setDisplay( const bool enable, const bool cursor, const bool blink );

    // Set cursor to position
    void position( const uint8_t row, const uint8_t col );

    // Set a custom char
    void cgram( const uint8_t pos, const char row[8] );

    // Write a character to display
    void put( const char chr );

    // Prints a string to display
    void print( const char * string );

    // Print a formatted string to display
    void printf( const char *fmt, ... );

public:
    void delay( uint8_t delay );
    void enable( const bool pulse );
    void set_output( const bool output );
    void write( const uint8_t data, const bool reg );
    bool read_busy();
    void task_add( const hd44780_command command, const bool reg, const uint16_t data, uint8_t nibbles );
    void task_del();
    void exec();

private:
    hd44780_conf    m_conf;

    hd44780_task Queue[HD44780_QUEUE_SIZE];
    uint16_t m_queueHead;
    uint16_t m_queueTail;
};

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

extern lcd_hd44780 * lcd;

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
