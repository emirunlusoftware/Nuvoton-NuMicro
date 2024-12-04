#include "stdio.h"
#include "NuMicro.h"

#define RXBUFSIZE   256

char text;

uint8_t g_u8RecData[RXBUFSIZE]  = {0};

volatile uint32_t g_u32comRbytes = 0;
volatile uint32_t g_u32comRhead  = 0;
volatile uint32_t g_u32comRtail  = 0;

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
	SYS->GPB_MFPL = 0x00000000;
	SYS->GPC_MFPH = 0x00000000;
	SYS->GPC_MFPL = 0x00000000;
	SYS->GPD_MFPH = 0x00000000;
	SYS->GPD_MFPL = 0x00000000;
	SYS->GPF_MFPH = 0x00000000;
	SYS->GPF_MFPL = SYS_GPF_MFPL_PF1MFP_ICE_CLK | SYS_GPF_MFPL_PF0MFP_ICE_DAT;

	/* Lock protected registers */
	SYS_LockReg();
}

void UART02_IRQHandler(void)
{
	if(UART_GET_INT_FLAG(UART0,UART_INTSTS_RDAINT_Msk))
	{
		printf("\nBasilan tus: ");
		while(UART_IS_RX_READY(UART0))
		{
			text = UART_READ(UART0);
			printf("%c\nGelen karakter: ",text);
			if(g_u32comRbytes < RXBUFSIZE)
			{
				/* Enqueue the character */
				g_u8RecData[g_u32comRtail] = text;
				g_u32comRtail = (g_u32comRtail == (RXBUFSIZE - 1)) ? 0 : (g_u32comRtail + 1);
				g_u32comRbytes++;
			}
		}
	}
	if(UART_GET_INT_FLAG(UART0,UART_INTSTS_THREINT_Msk))
	{
		uint16_t tmp; tmp = g_u32comRtail;
		if(g_u32comRhead != tmp)
		{
			text = g_u8RecData[g_u32comRhead];
			while(UART_IS_TX_FULL(UART0));  /* Wait Tx is not full to transmit data */
			UART_WRITE(UART0, text);
			g_u32comRhead = (g_u32comRhead == (RXBUFSIZE - 1)) ? 0 : (g_u32comRhead + 1);
			g_u32comRbytes--;
		}
	}
	UART_ClearIntFlag(UART0,UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTSTS_BUFERRINT_Msk);
}

int main(void)
{
	/* Init System, IP clock and multi-function I/O. */
	SYS_Init();

	/* Configure UART0 and set UART0 baud rate */
	UART_Open(UART0,115200);
	UART_EnableInt(UART0,UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTEN_BUFERRIEN_Msk);
	NVIC_EnableIRQ(UART02_IRQn);

	while (1);
}
