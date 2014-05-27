/*
		128x64 KS0108 LCD Driver for STM32F4
*/
#include "ks0108.h"
#include "font.h"
#include "delay.h"

/* Global Variables */
volatile unsigned char x_ks0108, y_ks0108;

/* Functions */

void strobe_ks0108(void){
	delay_us(1);							/* Delay 1us */
	KS0108_EN	= 1;						/* Strobe */
	delay_us(1);							/* Delay 1us */
	KS0108_EN	= 0;						/* Strobe */
	delay_us(1);							/* Delay 1us */
}

void wait_busy_ks0108(void){
	KS0108_PORT_DIR(KS0108_PORT_INPUT);		/* Change Port Direction */
	KS0108_DI	= 0;						/* Instruction Mode */
	KS0108_RW	= 1;						/* Read Mode */
	strobe_ks0108();						/* LCD Strobe */
	while((KS0108_DATA&0x7F)==0x80);		/* Chek Busy Flag */
	KS0108_RW	= 0;						/* Write Mode */
	KS0108_PORT_DIR(KS0108_PORT_OUTPUT); 	/* Change Port Direction */
}

void putcmd_ks0108(char ins){
	wait_busy_ks0108();						/* Wait Until LCD not Busy */
	KS0108_DI	= 0;						/* Instruction Mode */
	KS0108_LDATA(ins);						/* Load Instruction to Port */
	strobe_ks0108();						/* LCD Strobe */
}

unsigned char getd_ks0108(void){
	char data;
	KS0108_PORT_DIR(KS0108_PORT_INPUT);		/* Set Port Direction */
	KS0108_DI	= 1;						/* Data Mode */
	KS0108_RW	= 1;						/* Read Mode */
	strobe_ks0108();						/* LCD Strobe */
	data = KS0108_DATA;						/* Read LCD Data */
	KS0108_RW	= 0;						/* Write Mode */
	KS0108_PORT_DIR(KS0108_PORT_OUTPUT);	/* Set Port Direction */
	return data;	
}

void putd_ks0108(unsigned char data){
	if(x_ks0108==0){
		KS0108_L();							/* Select Chip */
		putcmd_ks0108(KS0108_Y_ADRESS|(y_ks0108/8));
		putcmd_ks0108(KS0108_X_ADRESS);
	}else if(x_ks0108==64){
		KS0108_R();							/* Select Chip */
		putcmd_ks0108(KS0108_Y_ADRESS|(y_ks0108/8));
		putcmd_ks0108(KS0108_X_ADRESS);
	}
	wait_busy_ks0108();						/* Wait Until LCD not Busy */
	KS0108_DI	= 1;						/* Data Mode */
	KS0108_LDATA(data);						/* Load Data to Port */
	strobe_ks0108();						/* LCD Strobe */
	x_ks0108++;
	if(x_ks0108>127){x_ks0108=0;y_ks0108+=8;}
	if(y_ks0108>63){y_ks0108=0;}
}

void cls_ks0108(void){
	unsigned int i;
	x_ks0108 = 0;y_ks0108 = 0;
	for(i=0;i<1024;i++)putd_ks0108(0x00);
	x_ks0108 = 0;y_ks0108 = 0;
}

void init_ks0108(void){

	KS0108_RST_DIR(KS0108_PIN_OUTPUT);
	KS0108_RW_DIR(KS0108_PIN_OUTPUT);
	KS0108_DI_DIR(KS0108_PIN_OUTPUT);
	KS0108_EN_DIR(KS0108_PIN_OUTPUT);
	KS0108_CS1_DIR(KS0108_PIN_OUTPUT);
	KS0108_CS2_DIR(KS0108_PIN_OUTPUT);
	KS0108_PORT_DIR(KS0108_PORT_OUTPUT);	/* Set Port Direction */
	
	KS0108_DI	= 0;						/* Instruction Mode */
	KS0108_RW	= 0;						/* Write Mode */
	KS0108_N();								/* Deselect Chips */
	KS0108_LDATA(0x00);						/* Load Data to Port */
	KS0108_EN	= 0;
	
	KS0108_RST	= 0;
	delay_ms(2);							/* Delay 2ms */
	KS0108_RST	= 1;
	delay_ms(1);							/* Delay 1ms */
	
	KS0108_L();								/* Select Chip */
	putcmd_ks0108(KS0108_DISPLAY_OFF);
	putcmd_ks0108(KS0108_START_LINE);
	putcmd_ks0108(KS0108_X_ADRESS);
	putcmd_ks0108(KS0108_Y_ADRESS);
	putcmd_ks0108(KS0108_DISPLAY_ON);
	
	KS0108_R();								/* Select Chip */
	putcmd_ks0108(KS0108_DISPLAY_OFF);
	putcmd_ks0108(KS0108_START_LINE);
	putcmd_ks0108(KS0108_X_ADRESS);
	putcmd_ks0108(KS0108_Y_ADRESS);
	putcmd_ks0108(KS0108_DISPLAY_ON);
	
	cls_ks0108();
}

void gotoxy_ks0108(unsigned char x, unsigned char y){
	x_ks0108 = x;
	y_ks0108 = y;
	if(x_ks0108<64){
		KS0108_L();							/* Select Chip */
		putcmd_ks0108(KS0108_Y_ADRESS|(y_ks0108/8));
		putcmd_ks0108(KS0108_X_ADRESS|x_ks0108);
	}else{
		KS0108_R();							/* Select Chip */
		putcmd_ks0108(KS0108_Y_ADRESS|(y_ks0108/8));
		putcmd_ks0108(KS0108_X_ADRESS|(x_ks0108-64));
	}
}

void putc_ks0108(unsigned char c){
	int i;
	switch(c){
		case '\f':cls_ks0108();break;
		case '\n':gotoxy_ks0108(0,y_ks0108+8);break;
		case '\r':gotoxy_ks0108(0,y_ks0108);break;
		default:if(x_ks0108>122)gotoxy_ks0108(0,y_ks0108+8);
				for(i = 0; i < 5; i++)putd_ks0108(Font[(((c-0x20)*5) + i)]);
				putd_ks0108(0x00);
				break;
	}
	
}

void puts_ks0108(unsigned char *s){
	do{
		putc_ks0108(*s);
	}while(*++s);
}