#include "stdio.h"
#include "NuMicro.h"

int d=0, a=0, b=0, c=0, start=0, count_timer=0, count_timer2=0, test=0;

void SYS_Init(void)
{
	SYS_UnlockReg();

	CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk|CLK_PWRCTL_HIRCEN_Msk);
	CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk|CLK_STATUS_HIRCSTB_Msk);
	CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(3));

	CLK_EnableModuleClock(TMR0_MODULE);
	CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, MODULE_NoMsk);

	SystemCoreClockUpdate();

	SYS->GPA_MFPH = 0x00000000;
	SYS->GPA_MFPL = 0x00000000;
	SYS->GPB_MFPH = SYS_GPB_MFPH_PB11MFP_GPIO | SYS_GPB_MFPH_PB10MFP_GPIO | SYS_GPB_MFPH_PB9MFP_GPIO | SYS_GPB_MFPH_PB8MFP_GPIO;
	SYS->GPB_MFPL = SYS_GPB_MFPL_PB7MFP_GPIO | SYS_GPB_MFPL_PB6MFP_GPIO | SYS_GPB_MFPL_PB5MFP_GPIO | SYS_GPB_MFPL_PB4MFP_GPIO | SYS_GPB_MFPL_PB3MFP_GPIO | SYS_GPB_MFPL_PB2MFP_GPIO | SYS_GPB_MFPL_PB1MFP_GPIO | SYS_GPB_MFPL_PB0MFP_GPIO;
	SYS->GPC_MFPH = 0x00000000;
	SYS->GPC_MFPL = SYS_GPC_MFPL_PC7MFP_TM0 | SYS_GPC_MFPL_PC6MFP_TM1;
	SYS->GPD_MFPH = 0x00000000;
	SYS->GPD_MFPL = 0x00000000;
	SYS->GPF_MFPH = 0x00000000;
	SYS->GPF_MFPL = SYS_GPF_MFPL_PF1MFP_ICE_CLK | SYS_GPF_MFPL_PF0MFP_ICE_DAT;

	SYS_LockReg();
}

/*
	SEGMENT A = PB10
	SEGMENT B = PB6
	SEGMENT C = PB3
	SEGMENT D = PB1
	SEGMENT E = PB0
	SEGMENT F = PB9 
	SEGMENT G = PB4
	DIG4 = PB5
	DIG3 = PB7
	DIG2 = PB8
	DIG1 = PB11
*/

void zero() {PB2=PB4=1; PB0=PB1=PB3=PB6=PB9=PB10=0;}
void one() {PB0=PB1=PB2=PB4=PB9=PB10=1; PB3=PB6=0;}
void two() {PB0=PB1=PB4=PB6=PB10=0; PB2=PB3=PB9=1;}
void three() {PB1=PB3=PB4=PB6=PB10=0; PB0=PB2=PB9=1;}
void four() {PB2=PB0=PB1=PB10=1; PB3=PB6=PB9=0;}
void five() {PB0=PB2=PB6=1; PB1=PB3=PB4=PB9=PB10=0;}
void six() {PB2=PB6=1; PB0=PB1=PB3=PB4=PB9=PB10=0;}
void seven() {PB0=PB1=PB2=PB4=PB9=1; PB3=PB6=PB10=0;}
void eight() {PB0=PB1=PB3=PB4=PB6=PB9=PB10=0; PB2=1;}
void nine() {PB2=PB0=1; PB1=PB3=PB4=PB6=PB9=PB10=0;}

void stopped()
{
	PB11=1;
	if (a>=0 && a<10000){zero();}
	if (a>=10000)
	{
		PB11=1;
		one();
		CLK_SysTickDelay(2500);

		PB11=0;
		PB8=1;
		zero();
		CLK_SysTickDelay(2500);

		PB8=0;
		PB7=1;
		zero();
		CLK_SysTickDelay(2500);

		PB7=0;
		PB5=1;
		zero();
		CLK_SysTickDelay(2500);

		PB5=0;
	}
	CLK_SysTickDelay(2500);

	PB11=0;
	PB8=1;
	if (b>=0 && b<1000){zero();}
	if (b>=1000 && b<2000){one();}
	if (b>=2000 && b<3000){two();}
	if (b>=3000 && b<4000){three();}
	if (b>=4000 && b<5000){four();}
	if (b>=5000 && b<6000){five();}
	if (b>=6000 && b<7000){six();}
	if (b>=7000 && b<8000){seven();}
	if (b>=8000 && b<9000){eight();}
	if (b>=9000 && b<10000){nine();}
	if (b>=10000) {b=0; zero();}
	CLK_SysTickDelay(2500);
	PB8=0;

	PB7=1;
	if (c>=0 && c<100){zero();}
	if (c>=100 && c<200){one();}
	if (c>=200 && c<300){two();}
	if (c>=300 && c<400){three();}
	if (c>=400 && c<500){four();}
	if (c>=500 && c<600){five();}
	if (c>=600 && c<700){six();}
	if (c>=700 && c<800){seven();}
	if (c>=800 && c<900){eight();}
	if (c>=900 && c<1000){nine();}
	if (c>=1000) {c=0; zero();}
	CLK_SysTickDelay(2500);
	PB7=0;

	PB5=1;
	if (d>=0 && d<10){zero();}
	if (d>=10 && d<20){one();}
	if (d>=20 && d<30){two();}
	if (d>=30 && d<40){three();}
	if (d>=40 && d<50){four();}
	if (d>=50 && d<60){five();}
	if (d>=60 && d<70){six();}
	if (d>=70 && d<80){seven();}
	if (d>=80 && d<90){eight();}
	if (d>=90 && d<100){nine();}
	if (d>=100) {d=0; zero();}
	CLK_SysTickDelay(2500);
	PB5=0;
}

void TMR0_IRQHandler(void)
{
	if (TIMER_GetIntFlag(TIMER0))
	{
		if (PB12==0)
		{
			count_timer++;
		}
		TIMER_ClearIntFlag(TIMER0);
	}
}

void GPABGH_IRQHandler(void)
{
	if (GPIO_GET_INT_FLAG(PB, BIT12))
	{
		if (count_timer<100 && start==0) {test=0;}

		if (count_timer<100 && start>0 && test==0) {count_timer=0; test=1;}
		else if (count_timer<100 && start>0 && test==1) {count_timer=0; test=0;}

		if (count_timer>=100 && test==0)
		{
			a=b=c=d=0;
			count_timer=0; test=1;
		}
		else if (count_timer>=100 && test==1 && a==0 && b==0 && c==0 && d==0) {count_timer=0; test=0;}
		else if (count_timer>=100 && test==1) {a=b=c=d=0;}
	}
	start++;
	GPIO_CLR_INT_FLAG(PB, BIT12);
}
int main(void)
{
	SYS_Init();
	GPIO_SetMode(PB, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7|BIT8|BIT9|BIT10|BIT11, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PB, BIT12, GPIO_MODE_INPUT);
	GPIO_SetMode(PB, BIT14, GPIO_MODE_OUTPUT);
	PB11=PB8=PB7=PB5=0; PB2=1;

	GPIO_EnableInt(PB, 12, GPIO_INT_FALLING);
	NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);
	GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_512);
	GPIO_ENABLE_DEBOUNCE(PB, BIT12);

	while (start==0);

	TIMER_Open(TIMER0,TIMER_PERIODIC_MODE,100);
	TIMER_EnableInt(TIMER0);
	NVIC_EnableIRQ(TMR0_IRQn);
	TIMER_Start(TIMER0);

	while (1)
	{
		if (test==1)
		{
			while (1)
			{
				stopped();
				if (count_timer==0 && test==0) {break;}
			}
		}

		PB11=1;
		if (a>=0 && a<10000){zero();}
		while (a>=10000)
		{
			PB11=1;
			one();
			CLK_SysTickDelay(2490);

			PB11=0;
			PB8=1;
			zero();
			CLK_SysTickDelay(2490);

			PB8=0;
			PB7=1;
			zero();
			CLK_SysTickDelay(2490);

			PB7=0;
			PB5=1;
			zero();
			CLK_SysTickDelay(2490);

			PB5=0;
		}
		a+=1; b+=1; c+=1; d+=1;
		CLK_SysTickDelay(2490);

		PB11=0;
		PB8=1;
		if (b>=0 && b<1000){zero();}
		if (b>=1000 && b<2000){one();}
		if (b>=2000 && b<3000){two();}
		if (b>=3000 && b<4000){three();}
		if (b>=4000 && b<5000){four();}
		if (b>=5000 && b<6000){five();}
		if (b>=6000 && b<7000){six();}
		if (b>=7000 && b<8000){seven();}
		if (b>=8000 && b<9000){eight();}
		if (b>=9000 && b<10000){nine();}
		if (b>=10000) {b=0; zero();}
		CLK_SysTickDelay(2490);
		PB8=0;

		PB7=1;
		if (c>=0 && c<100){zero();}
		if (c>=100 && c<200){one();}
		if (c>=200 && c<300){two();}
		if (c>=300 && c<400){three();}
		if (c>=400 && c<500){four();}
		if (c>=500 && c<600){five();}
		if (c>=600 && c<700){six();}
		if (c>=700 && c<800){seven();}
		if (c>=800 && c<900){eight();}
		if (c>=900 && c<1000){nine();}
		if (c>=1000) {c=0; zero();}
		CLK_SysTickDelay(2490);
		PB7=0;

		PB5=1;
		if (d>=0 && d<10){zero();}
		if (d>=10 && d<20){one();}
		if (d>=20 && d<30){two();}
		if (d>=30 && d<40){three();}
		if (d>=40 && d<50){four();}
		if (d>=50 && d<60){five();}
		if (d>=60 && d<70){six();}
		if (d>=70 && d<80){seven();}
		if (d>=80 && d<90){eight();}
		if (d>=90 && d<100){nine();}
		if (d>=100) {d=0; zero();}
		CLK_SysTickDelay(2490);
		PB5=0;
	}
}
