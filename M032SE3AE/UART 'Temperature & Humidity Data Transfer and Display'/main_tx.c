#include "stdio.h"
#include "NuMicro.h"

/* 16-bit variables, they're raw hexadecimal datas and will be converted to real temperature and humidity values. */
uint16_t measured,measured2;

/* 8-bit arrays to store and send datas which come from TH06 Thermal Sensor. */
uint8_t array[2],array2[2];

void SYS_Init(void)
{
		/* Unlock protected registers */
		SYS_UnlockReg();

		/* If the macros do not exist in your project, please refer to the related clk.h in Header folder of the tool package */
		/* Enable clock source */
		CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

		/* Waiting for clock source ready */
		CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

		/* Set HCLK clock */
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(4));

		/* Set PCLK-related clock */
		CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV1 | CLK_PCLKDIV_APB1DIV_DIV1);

		/* Enable IP clock */
		CLK_EnableModuleClock(I2C0_MODULE);
		CLK_EnableModuleClock(UART0_MODULE);

		/* Set IP clock */
		CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_PCLK0, CLK_CLKDIV0_UART0(1));

		/* Update System Core Clock */
		/* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
		SystemCoreClockUpdate();

		/* If the macros do not exist in your project, please refer to the corresponding header file in Header folder of the tool package */
		SYS->GPA_MFPH = 0x00000000;
		SYS->GPA_MFPL = 0x00000000;
		SYS->GPB_MFPH = SYS_GPB_MFPH_PB13MFP_UART0_TXD | SYS_GPB_MFPH_PB12MFP_UART0_RXD;
		SYS->GPB_MFPL = SYS_GPB_MFPL_PB5MFP_I2C0_SCL | SYS_GPB_MFPL_PB4MFP_I2C0_SDA;
		SYS->GPC_MFPL = 0x00000000;
		SYS->GPF_MFPH = 0x00000000;
		SYS->GPF_MFPL = SYS_GPF_MFPL_PF1MFP_ICE_CLK | SYS_GPF_MFPL_PF0MFP_ICE_DAT;

		/* Lock protected registers */
		SYS_LockReg();
}

uint16_t TEMPERATURE (void)
{
	/* Read two 8-bit datas from TH06 thermal sensor [TEMPERATURE] */
	I2C_ReadMultiBytesOneReg(I2C0, 0x40, 0xE3, array, 2);

	/* Combine two 8-bit datas */
	measured = (array[0] << 8) | (array[1]);

	/* We need to mask the combined data according to datasheet. It disregards the last 2 bits and makes the bits to zero.
	Q: WHY?
	A: To get more accurate temperature data. */
	measured = measured & 0xFFFC;

	return measured;
}

uint16_t HUMIDITY (void)
{
	/* Read two 8-bit datas from TH06 thermal sensor [HUMIDITY] */
	I2C_ReadMultiBytesOneReg(I2C0, 0x40, 0xE5, array2, 2);

	/* Combine two 8-bit datas */
	measured2 = (array2[0] << 8) | (array2[1]);

	/* We need to mask the combined data according to datasheet. It makes the last two bits one and zero (10)
	Q: WHY?
	A: To get more accurate humidity data. */
	measured2 = measured2 & 0xFFFE;

	return measured2;
}

int main(void)
{
	/* Init System, IP clock and multi-function I/O. */
	SYS_Init();

	/* Configure UART0 and set UART0 baud rate */
	UART_Open(UART0,115200);

	/* Open I2C module and set bus clock */
	I2C_Open(I2C0,100000);

	while (1)
	{
		CLK_SysTickDelay(1000);

		/* Get temperature & humidity datas from the functions. */
		TEMPERATURE(); HUMIDITY();

		/* Send (transmit) two 8-bit temperature and two 8-bit humidity datas to receiver. */
		UART_WRITE(UART0,array[0]); UART_WRITE(UART0,array[1]);
		UART_WRITE(UART0,array2[0]); UART_WRITE(UART0,array2[1]);
	}
}
