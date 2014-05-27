//---------------------------------------------------------------------------------------------------------
// HD44780 Library
//---------------------------------------------------------------------------------------------------------

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "hd44780.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_tim.h"

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

lcd_hd44780 * lcd = NULL;

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::delay( uint8_t delay )
{
	while ( delay != 0 )
		delay--;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::set_output( const bool output )
{
	GPIO_InitTypeDef GPIO_InitStruct;
	uint32_t pins;
	uint8_t dir;

	pins = m_conf.db4 | m_conf.db5 | m_conf.db6 | m_conf.db7;
	dir = GPIO_Mode_IN;

	if ( output )
    {
		pins = pins | m_conf.rs | m_conf.rw | m_conf.e;
		dir = GPIO_Mode_OUT;
	}

	GPIO_InitStruct.GPIO_Pin = pins;
	GPIO_InitStruct.GPIO_Mode = (GPIOMode_TypeDef)(dir);
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;

	GPIO_Init( m_conf.gpio, &GPIO_InitStruct );
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::enable(const bool pulse)
{
	if ( pulse )
    {
		GPIO_ToggleBits( m_conf.gpio, m_conf.e );
		delay( 150 );
	}
	GPIO_ToggleBits( m_conf.gpio, m_conf.e );
	delay( 150 );
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::write( const uint8_t data, const bool reg )
{
	set_output( true );

	GPIO_ResetBits( m_conf.gpio, m_conf.rw );
	GPIO_WriteBit( m_conf.gpio, m_conf.rs, (BitAction)(!reg) );
	GPIO_WriteBit( m_conf.gpio, m_conf.db7, (BitAction)((data & 0x8) >> 3) );
	GPIO_WriteBit( m_conf.gpio, m_conf.db6, (BitAction)((data & 0x4) >> 2) );
	GPIO_WriteBit( m_conf.gpio, m_conf.db5, (BitAction)((data & 0x2) >> 1) );
	GPIO_WriteBit( m_conf.gpio, m_conf.db4, (BitAction)((data & 0x1)) );
	enable( true );
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

bool lcd_hd44780::read_busy()
{
	uint8_t data;

	set_output( false );
	GPIO_ResetBits( m_conf.gpio, m_conf.rs | m_conf.db7 );
	GPIO_SetBits( m_conf.gpio, m_conf.rw );
	enable( false );
	data = GPIO_ReadInputDataBit( m_conf.gpio, m_conf.db7 );
	enable( false );
	enable( true );

	if ( data == 1 )
		return true;

	return false;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::task_add( const hd44780_command command, const bool reg, const uint16_t data, uint8_t nibbles )
{
	if (command == HD44780_WAIT_NOT_BUSY)
		nibbles = 2;

	nibbles--;

	if (m_queueHead != (m_queueTail + 1) % (HD44780_QUEUE_SIZE - 1))
    {
		Queue[m_queueTail].command = command;
		Queue[m_queueTail].reg = reg;
		Queue[m_queueTail].data = data;
		Queue[m_queueTail].nibble = nibbles;
		m_queueTail = (m_queueTail + 1) % (HD44780_QUEUE_SIZE - 1);
	}
	else
    {
		assert_param(false);
    }
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::task_del()
{
	if (m_queueHead != m_queueTail)
		m_queueHead = (m_queueHead + 1) % (HD44780_QUEUE_SIZE - 1);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::exec()
{
	volatile hd44780_task * task;

	if (m_queueHead != m_queueTail)
    {
		task = &Queue[m_queueHead];

		switch (task->command)
		{
		case HD44780_WAIT:
	    {
			if (task->data > 0)
			{
				task->data--;
			}
			else
			{
				task_del();
				exec();
			}
	    }
		break;
		case HD44780_WAIT_NOT_BUSY:
        {
			if (!read_busy())
            {
				task_del();
				exec();
			}
	    }
		break;
		case HD44780_WRITE:
        {

			if (task->nibble == 1)
				write(task->data >> 4, task->reg);
			write(task->data & 0xf, task->reg);
			task_del();
			exec();
	    }
		break;
		}
	}
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::clear(void)
{
	task_add(HD44780_WRITE, true, HD44780_CLEAR, 2);
	task_add(HD44780_WAIT_NOT_BUSY, NONE, NONE, NONE);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::home(void)
{
	task_add(HD44780_WRITE, true, HD44780_HOME, 2);
	task_add(HD44780_WAIT_NOT_BUSY, NONE, NONE, NONE);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::setDisplay(const bool enable, const bool cursor, const bool blink)
{
	uint16_t command = HD44780_DISPLAY;

	if (enable)
		command |= HD44780_DISPLAY_ON;
	else
		m_queueHead = m_queueTail = 0;

	if (cursor)
		command |= HD44780_CURSOR_ON;

	if (blink)
		command |= HD44780_BLINK_ON;

	task_add(HD44780_WRITE, true, command, 2);
	task_add(HD44780_WAIT_NOT_BUSY, NONE, NONE, NONE);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::position(const uint8_t row, const uint8_t col)
{
	uint16_t command = HD44780_DGRAM;
	const uint8_t offsets[] = { 0x00, 0x40, 0x14, 0x54 };

	command |= col + offsets[row];
	task_add(HD44780_WRITE, true, command, 2);
	task_add(HD44780_WAIT_NOT_BUSY, NONE, NONE, NONE);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::cgram(const uint8_t pos, const char udg[8]) {

	uint8_t i;
	uint16_t command = HD44780_CGRAM;

	assert_param(pos < 8);

	command |= pos * 8;

	task_add(HD44780_WRITE, true, command, 2);
	for (i = 0; i < 8; i++)
    {
		task_add(HD44780_WRITE, false, udg[i], 2);
		task_add(HD44780_WAIT_NOT_BUSY, NONE, NONE, NONE);
	}
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::put(const char chr)
{
	task_add(HD44780_WRITE, false, chr, 2);
	task_add(HD44780_WAIT_NOT_BUSY, NONE, NONE, NONE);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::print(const char* string)
{
	uint8_t i = 0;

	while (string[i])
    {
		task_add(HD44780_WRITE, false, string[i], 2);
		task_add(HD44780_WAIT_NOT_BUSY, NONE, NONE, NONE);
		i++;
	}
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void lcd_hd44780::printf(const char *fmt, ...)
{
	uint16_t i;
	uint16_t size;
	uint8_t character;
	char buffer[32];
	va_list args;

	va_start(args, fmt);
	size = vsprintf(buffer, fmt, args);

	for (i = 0; i < size; i++)
    {
		character = buffer[i];

		if (character == 10)
			break;
		else
			put(character);
	}
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

lcd_hd44780::lcd_hd44780(  GPIO_TypeDef* gpio,
                           const uint16_t rs,
                           const uint16_t rw,
                           const uint16_t e,
                           const uint16_t db4,
                           const uint16_t db5,
                           const uint16_t db6,
                           const uint16_t db7,
                           const hd44780_lines lines,
                           const hd44780_font font)
{
    m_queueHead = 0;
    m_queueTail = 0;

	uint32_t periph;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_InitStructure;

	assert_param(IS_GPIO_ALL_PERIPH(gpio));
	assert_param(IS_GPIO_PIN(rs));
	assert_param(IS_GPIO_PIN(rw));
	assert_param(IS_GPIO_PIN(e));
	assert_param(IS_GPIO_PIN(db4));
	assert_param(IS_GPIO_PIN(db5));
	assert_param(IS_GPIO_PIN(db6));
	assert_param(IS_GPIO_PIN(db7));

	m_conf.gpio = gpio;
	m_conf.rs = rs;
	m_conf.rw = rw;
	m_conf.e = e;
	m_conf.db4 = db4;
	m_conf.db5 = db5;
	m_conf.db6 = db6;
	m_conf.db7 = db7;
	m_conf.lines = lines;
	m_conf.font = font;

	if (gpio == GPIOA )
		periph = RCC_AHB1Periph_GPIOA;
	else if (gpio == GPIOB )
		periph = RCC_AHB1Periph_GPIOB;
	else if (gpio == GPIOC )
		periph = RCC_AHB1Periph_GPIOC;
	else if (gpio == GPIOD )
		periph = RCC_AHB1Periph_GPIOD;
	else if (gpio == GPIOE )
		periph = RCC_AHB1Periph_GPIOE;
	else
		return;

	RCC_AHB1PeriphClockCmd(periph, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1 );
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = HD44780_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_InitStructure.TIM_Period = (62500 / HD44780_QUEUE_FREQ) - 1;
	TIM_InitStructure.TIM_Prescaler = ((SystemCoreClock / 2) / 62500) - 1;
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(HD44780_TIMER, &TIM_InitStructure);
	TIM_ITConfig(HD44780_TIMER, TIM_IT_Update, ENABLE);
	TIM_Cmd(HD44780_TIMER, ENABLE);

	task_add(HD44780_WAIT, true, 1000, 1);
	task_add(HD44780_WRITE, true, 3, 1);
	task_add(HD44780_WAIT, NONE, 5, NONE);
	task_add(HD44780_WRITE, true, 3, 1);
	task_add(HD44780_WAIT, NONE, 1, NONE);
	task_add(HD44780_WRITE, true, 3, 1);
	task_add(HD44780_WAIT, NONE, 1, NONE);
	task_add(HD44780_WRITE, true, 2, 1);
	task_add(HD44780_WAIT, NONE, 1, NONE);
	task_add(HD44780_WRITE, true, HD44780_FUNCTION | HD44780_4BIT | m_conf.lines | m_conf.font, 2);
	task_add(HD44780_WAIT, NONE, 1, NONE);
	task_add(HD44780_WRITE, true, HD44780_DISPLAY | HD44780_DISPLAY_ON | !HD44780_CURSOR_ON, 2);
	task_add(HD44780_WAIT, NONE, 1, NONE);
	task_add(HD44780_WRITE, true, HD44780_ENTRY | HDD44780_ENTRY_LEFT, 2);
	task_add(HD44780_WAIT_NOT_BUSY, NONE, NONE, NONE);
	clear();
	home();
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

extern "C" void TIM7_IRQHandler()
{
    if ( lcd )
        lcd->exec();
	TIM_ClearITPendingBit(HD44780_TIMER, TIM_IT_Update );
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

