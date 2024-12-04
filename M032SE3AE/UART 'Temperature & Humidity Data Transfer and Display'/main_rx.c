#include "stdio.h"
#include "NuMicro.h"

/*#define RXBUFSIZE   256
uint8_t g_u8RecData[RXBUFSIZE]  = {0};

volatile uint32_t g_u32comRbytes = 0;
volatile uint32_t g_u32comRhead  = 0;
volatile uint32_t g_u32comRtail  = 0;*/

/* The variables that show us the temperature and humidity values. */
float temperature,humidity;

/* 16-bit variables, they're raw hexadecimal datas and will be converted to real temperature and humidity values. */
uint16_t measured,measured2;

/* 8-bit variables to store datas which come from TH06 Thermal Sensor. */
uint8_t measured_t,measured_t2,measured_h,measured_h2;

int humidity_int,temperature_int,Ones,Tens,Hundreds,Thousands,TenThousands,i;
int button_pressed,counter=0;

void SYS_Init(void)
{
	/* Unlock protected registers */
	SYS_UnlockReg();

	/* Enable clock source */
	CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

	/* Waiting for clock source ready */
	CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

	/* Set HCLK clock */
	CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(4));

	/* Set PCLK-related clock */
	CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV1 | CLK_PCLKDIV_APB1DIV_DIV1);

	/* Enable IP clock for modules */
	CLK_EnableModuleClock(TMR0_MODULE);
	CLK_EnableModuleClock(UART0_MODULE);

	/* Set IP clock */
	CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, MODULE_NoMsk);
	CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_PCLK0, CLK_CLKDIV0_UART0(1));

	/* Update System Core Clock */
	/* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
	SystemCoreClockUpdate();

	/* If the macros do not exist in your project, please refer to the corresponding header file in Header folder of the tool package */
	/* PB12 and PB13 pins are configured for USB_UART, meaning data can be transmitted and received via USB. If these pins are to be used for other purposes, the TXD (1) and RXD (2) in the VCOM section of the demo board must be disabled. */
	SYS->GPA_MFPH = SYS_GPA_MFPH_PA11MFP_GPIO;
	SYS->GPB_MFPH = SYS_GPB_MFPH_PB13MFP_UART0_TXD | SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB11MFP_GPIO | SYS_GPB_MFPH_PB10MFP_GPIO | SYS_GPB_MFPH_PB9MFP_GPIO | SYS_GPB_MFPH_PB8MFP_GPIO;
	SYS->GPB_MFPL = SYS_GPB_MFPL_PB7MFP_GPIO | SYS_GPB_MFPL_PB6MFP_GPIO | SYS_GPB_MFPL_PB5MFP_GPIO | SYS_GPB_MFPL_PB4MFP_GPIO | SYS_GPB_MFPL_PB3MFP_GPIO | SYS_GPB_MFPL_PB2MFP_GPIO | SYS_GPB_MFPL_PB1MFP_GPIO | SYS_GPB_MFPL_PB0MFP_GPIO;
	SYS->GPC_MFPH = 0x00000000;
	SYS->GPC_MFPL = SYS_GPC_MFPL_PC7MFP_TM0;
	SYS->GPD_MFPH = 0x00000000;
	SYS->GPD_MFPL = 0x00000000;
	SYS->GPF_MFPH = 0x00000000;
	SYS->GPF_MFPL = SYS_GPF_MFPL_PF1MFP_ICE_CLK | SYS_GPF_MFPL_PF0MFP_ICE_DAT;

	/* Lock protected registers */
	SYS_LockReg();
}

/*
	THE PINS OF FOUR DIGIT SEVEN SEGMENT

	PB10 (pin 61) = SEGMENT A
	PB6 (pin 1) = SEGMENT B
	PB3 (pin 4) = SEGMENT C
	PB1 (pin 6) = SEGMENT D
	PB0 (pin 7) = SEGMENT E
	PB9 (pin 62) = SEGMENT F
	PB4 (pin 3) = SEGMENT G
	PB2 (pin 5) = DP (Decimal Point)
	PB5 (pin 2) = DIGIT 4
	PB7 (pin 64) = DIGIT 3
	PB8 (pin 63) = DIGIT 2
	PB11 (pin 60) = DIGIT 1
*/

void zero(){PB4=1; PB0=PB1=PB3=PB6=PB9=PB10=0;}
void one(){PB0=PB1=PB4=PB9=PB10=1; PB3=PB6=0;}
void two(){PB3=PB9=1; PB0=PB1=PB4=PB6=PB10=0;}
void three(){PB0=PB9=1; PB1=PB3=PB4=PB6=PB10=0;}
void four(){PB0=PB1=PB10=1; PB3=PB6=PB9=PB4=0;}
void five(){PB0=PB6=1; PB1=PB3=PB4=PB9=PB10=0;}
void six(){PB6=1; PB0=PB1=PB3=PB4=PB9=PB10=0;}
void seven(){PB0=PB1=PB4=PB9=1; PB3=PB6=PB10=0;}
void eight(){PB0=PB1=PB3=PB4=PB6=PB9=PB10=0;}
void nine(){PB0=1; PB1=PB3=PB4=PB6=PB9=PB10=0;}
void minus(){PB4=0; PB0=PB1=PB3=PB6=PB9=PB10=1;}
void delay(){CLK_SysTickDelay(1000);}

void segment (int sayi)
{
	switch(sayi)
	{
	case 0: zero(); break;
	case 1: one(); break;
	case 2: two(); break;
	case 3: three(); break;
	case 4: four(); break;
	case 5: five(); break;
	case 6: six(); break;
	case 7: seven(); break;
	case 8: eight(); break;
	case 9: nine(); break;
	}
}

void temperature_display (void)
{
	TenThousands= (temperature_int/10000)%10;
	Thousands=		(temperature_int/1000)%10;
	Hundreds=			(temperature_int/100)%10;
	Tens=					(temperature_int/10)%10;
	Ones=					 temperature_int%10;

	if(temperature_int >= 0)
	{
		if (temperature_int >= 10000)
		{
			PB11= 1;				segment(TenThousands); delay(); PB11= 0;
			PB8 = 1;				segment(Thousands);		 delay();	PB8 = 0;
			PB7 = 1; PB2=0;	segment(Hundreds);		 delay();	PB7 = 0; PB2=1;
			PB5 = 1;				segment(Tens);				 delay();	PB5 = 0;
		}
		else if (temperature_int >= 0 && temperature_int < 1000)
		{
																											PB5 = 0;
			PB11= 1; PB2=0;	segment(Hundreds);	delay();		PB11= 0; PB2 = 1;
			PB8 = 1;				segment(Tens);			delay();		PB8 = 0;
			PB7 = 1;				segment(Ones);			delay();		PB7 = 0;
		}
		else
		{
			PB11= 1;				segment(Thousands); delay(); PB11= 0; 
			PB8 = 1; PB2=0;	segment(Hundreds);	delay(); PB8 = 0; PB2=1;
			PB7 = 1;				segment(Tens);			delay(); PB7 = 0;
			PB5 = 1;				segment(Ones);			delay(); PB5 = 0;
		}
	}

	if (temperature_int < 0)
	{
		if (temperature_int > -1000 && temperature_int < 0)
		{
			PB11= 1;				minus();					 delay(); PB11= 0;
			PB8 = 1; PB2=0;	segment(Hundreds); delay();	PB8 = 0; PB2 = 1;
			PB7 = 1;				segment(Tens);		 delay(); PB7 = 0;
			PB5 = 1;				segment(Ones);		 delay(); PB5 = 0;
		}
		else
		{
			PB11= 1;				minus();						delay(); PB11= 0;
			PB8 = 1;				segment(Thousands); delay(); PB8 = 0;
			PB7 = 1; PB2=0;	segment(Hundreds);	delay(); PB7 = 0; PB2 = 1;
			PB5 = 1;				segment(Tens);			delay(); PB5 = 0;
		}
	}
}

void humidity_display (void)
{
	TenThousands= (humidity_int/10000)%10;
	Thousands=		(humidity_int/1000)%10;
	Hundreds=			(humidity_int/100)%10;
	Tens=					(humidity_int/10)%10;
	Ones=					 humidity_int%10;

	if (humidity_int >= 100)
	{
														 PB5 = 0; PB2 = 1;
		PB11= 1;				one();	delay();  PB11= 0;
		PB8 = 1;				zero(); delay();	PB8 = 0;
		PB7 = 1;				zero(); delay();	PB7 = 0;
	}
	else if (humidity_int >= 0 && humidity_int < 10)
	{
												PB8 = PB7 = PB5 = 0; PB2 = 1;
		PB11= 1;				segment(Ones); delay();  PB11= 0;
	}
	else if (humidity_int < 0)
	{
												PB8 = PB7 = PB5 = 0; PB2 = 1;
		PB11= 1;								zero(); delay(); PB11= 0;
	}
	else
	{
														 PB7 = PB5 = 0; PB2 = 1;
		PB11= 1;				segment(Tens); delay(); PB11= 0;
		PB8 = 1;				segment(Ones); delay(); PB8 = 0;
	}
}

void GPABGH_IRQHandler(void) {button_pressed++; GPIO_CLR_INT_FLAG(PA, BIT11);}

void TMR0_IRQHandler(void)
{
	if(TIMER_GetIntFlag(TIMER0) == 1)
	{
		counter++;
		if (counter == 60) {counter = 0;}
		TIMER_ClearIntFlag(TIMER0);
	}
}


int main(void)
{
	/* Init System, IP clock and multi-function I/O */
	SYS_Init();

	GPIO_SetMode(PB,BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7|BIT8|BIT9|BIT10|BIT11,GPIO_MODE_OUTPUT);//SEGMENTS & DIGITS
	GPIO_SetMode(PA,BIT11,GPIO_MODE_INPUT);//BUTTON

	/* Enable GPIO Interrupt for button */
	GPIO_EnableInt(PA,11,GPIO_INT_FALLING);
	NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);
	GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC,GPIO_DBCTL_DBCLKSEL_512);
	GPIO_ENABLE_DEBOUNCE(PA,BIT11);

	/* Configure UART0 and set UART0 baud rate */
	UART_Open(UART0,115200);

	/* Wait until PA11 (button) is pressed */
	while(button_pressed==0);
	
	/* Enable Timer Interrupt for counting */
	TIMER_Open(TIMER0,TIMER_PERIODIC_MODE,1);
	TIMER_EnableInt(TIMER0);
	NVIC_EnableIRQ(TMR0_IRQn);
	TIMER_Start(TIMER0);

	while (1)
	{
		/* To understand if UART pins are connected correctly */
		if(UART_IS_RX_READY(UART0))
		{
			/* Get the 8-bit temperature and humidity values */
			/* You can only get maximum 8-bit values if you want to send any data */
			measured_t = UART_READ(UART0); measured_t2 = UART_READ(UART0);
			measured_h = UART_READ(UART0); measured_h2 = UART_READ(UART0);

			/* Combine two 8-bit datas */
			measured = (measured_t << 8) | (measured_t2);
			measured2 = (measured_h << 8) | (measured_h2);

			/* We need to mask the combined data according to datasheet
			Q: WHY?
			A: To get more accurate temperature & humidity data. */
			measured = measured & 0xFFFC;
			measured2 = measured2 & 0xFFFE;
		}

		/* If not, the system will send "NO DATA" message on Docklight */
		else {printf("\nNO DATA! NO DATA!\nNO DATA! NO DATA!\nNO DATA! NO DATA!\n");}

		/* Mathematical operations needed in order to get significant temperature & humidity values */
		temperature = (((measured * 175.72) / 65536.0) - 46.85);
		humidity = (((measured2 * 125) / 65536.0) - 6);

		/* For temperature / humidity_display function */
		temperature_int = temperature*100; humidity_int = humidity;

		/* Start the display function. The function that displays the temperature or humidity values on the four digit seven segment */
		/* "for loop" is for processing this function longer */
		for (i=0;i<200;i++)
		{
			if (counter >= 0 && counter < 40 && temperature_int >= -4675) {temperature_display();}
			else if (counter >= 40 && counter < 60) {humidity_display();}
		}
	}
}
