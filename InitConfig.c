#include "InitConfig.h"
//
#include "SysConfig.h"
#include "Measurement.h"

// Functions
//
Boolean SysClk_Config()
{
	return RCC_PLL_HSE_Config(QUARTZ_FREQUENCY, PREDIV_4, PLL_14);
}
//------------------------------------------------------------------------------

void EI_Config()
{
	// Overflow 90%
	EXTI_Config(EXTI_PB, EXTI_4, FALL_TRIG, 0);
	EXTI_EnableInterrupt(EXTI4_IRQn, 0, true);
	
	// Overflow 10%
	EXTI_Config(EXTI_PB, EXTI_8, FALL_TRIG, 0);
	EXTI_EnableInterrupt(EXTI9_5_IRQn, 0, true);
}
//------------------------------------------------------------------------------

void IO_Config()
{
	// Включение тактирования портов
	RCC_GPIO_Clk_EN(PORTA);
	RCC_GPIO_Clk_EN(PORTB);
	RCC_GPIO_Clk_EN(PORTC);
	
	// Аналоговые входы
	GPIO_InitAnalog(GPIO_ANLG_I_DUT);
	
	// Выходы
	GPIO_InitPushPullOutput(GPIO_FAN);
	GPIO_InitPushPullOutput(GPIO_M_RESET);
	GPIO_InitPushPullOutput(GPIO_LOAD);
	GPIO_InitPushPullOutput(GPIO_CS);
	GPIO_InitPushPullOutput(GPIO_IND);
	GPIO_InitPushPullOutput(GPIO_SYNC);
	GPIO_InitPushPullOutput(GPIO_SFTY_EN);
	GPIO_InitPushPullOutput(GPIO_RLC);
	GPIO_InitPushPullOutput(GPIO_RELAY);
	GPIO_InitPushPullOutput(GPIO_SNC_TOCU);
	GPIO_InitPushPullOutput(GPIO_SREG_CS);
	GPIO_InitPushPullOutput(GPIO_CS_GD2);
	GPIO_InitPushPullOutput(GPIO_CS_DAC);
	GPIO_InitPushPullOutput(GPIO_SREG_OE);
	GPIO_InitPushPullOutput(GPIO_CS_GD1);
	GPIO_InitPushPullOutput(GPIO_LDAC);
	GPIO_InitPushPullOutput(GPIO_LED);
	GPIO_InitPushPullOutput(GPIO_SYNC_GD);
	GPIO_InitPushPullOutput(GPIO_PS_EN);
	//
	GPIO_SetState(GPIO_CS, true);
	GPIO_SetState(GPIO_SREG_CS, true);
	GPIO_SetState(GPIO_CS_GD1, true);
	GPIO_SetState(GPIO_CS_GD2, true);
	GPIO_SetState(GPIO_CS_DAC, true);
	GPIO_SetState(GPIO_LDAC, true);
	GPIO_SetState(GPIO_M_RESET, true);

	// Выход с открытым коллектором
	GPIO_InitOpenDrainOutput(GPIO_TRIG_RST, NoPull);
	GPIO_SetState(GPIO_TRIG_RST, true);
	
	// Входы
	GPIO_InitInput(GPIO_SFTY, NoPull);
	GPIO_InitInput(GPIO_OVERFLOW90, NoPull);
	GPIO_InitInput(GPIO_OVERFLOW10, NoPull);
	GPIO_InitInput(GPIO_PRESSURE, NoPull);
	
	// Альтернативные функции
	GPIO_InitAltFunction(GPIO_ALT_CAN_RX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_CAN_TX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_UART_RX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_UART_TX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_SPI1_CLK, AltFn_5);
	GPIO_InitAltFunction(GPIO_ALT_SPI1_MISO, AltFn_5);
	GPIO_InitAltFunction(GPIO_ALT_SPI2_CLK, AltFn_5);
	GPIO_InitAltFunction(GPIO_ALT_SPI2_MOSI, AltFn_5);
}
//------------------------------------------------------------------------------

void CAN_Config()
{
	RCC_CAN_Clk_EN(CAN_1_ClkEN);
	NCAN_Init(SYSCLK, CAN_BAUDRATE, FALSE);
	NCAN_FIFOInterrupt(TRUE);
	NCAN_FilterInit(0, 0, 0); // Фильтр 0 пропускает все сообщения
}
//------------------------------------------------------------------------------

void UART_Config()
{
	USART_Init(USART1, SYSCLK, USART_BAUDRATE);
	USART_Recieve_Interupt(USART1, 0, true);
}
//------------------------------------------------------------------------------

void ADC_Config()
{
	RCC_ADC_Clk_EN(ADC_12_ClkEN);

	ADC_Calibration(ADC1);
	ADC_Enable(ADC1);
	ADC_SoftTrigConfig(ADC1);

	// Конфигурация и отключение DMA
	ADC_DMAConfig(ADC1);
	ADC_DMAConfig(ADC2);
	ADC_DMAEnable(ADC1, false);
	ADC_DMAEnable(ADC2, false);
}
//------------------------------------------------------------------------------

void ADC_SwitchToSingleMeasurement()
{
	ADC_SoftTrigConfig(ADC1);
	ADC_DMAEnable(ADC1, false);
}
//------------------------------------------------------------------------------

void ADC_SwitchToDMA()
{
	ADC_TrigConfig(ADC1, ADC12_TIM6_TRGO, RISE);

	ADC_ChannelSeqReset(ADC1);
	ADC_ChannelSet_Sequence(ADC1, ADC1_CURRENT_CHANNEL, 1);
	ADC_ChannelSeqLen(ADC1, 1);

	ADC_DMAEnable(ADC1, true);
}
//------------------------------------------------------------------------------

void DMA_Config()
{
	DMA_Clk_Enable(DMA1_ClkEN);

	// DMA для АЦП тока на DUT
	DMA_Reset(DMA_ADC_DUT_I_CHANNEL);
	DMA_Interrupt(DMA_ADC_DUT_I_CHANNEL, DMA_TRANSFER_COMPLETE, 0, true);
	DMAChannelX_DataConfig(DMA_ADC_DUT_I_CHANNEL, (uint32_t)LOGIC_OutputPulseRaw, (uint32_t)(&ADC1->DR), PULSE_ARR_MAX_LENGTH);
	DMAChannelX_Config(DMA_ADC_DUT_I_CHANNEL, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_16BIT, DMA_PSIZE_16BIT,
						DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_DIS, DMA_READ_FROM_PERIPH, 0);
}
//------------------------------------------------------------------------------

void Timer3_Config()
{
	TIM_Clock_En(TIM_3);
	TIM_Config(TIM3, SYSCLK, TIMER3_uS);
	TIM_Interupt(TIM3, 0, true);
	TIM_Start(TIM3);
}
//------------------------------------------------------------------------------

void Timer6_Config()
{
	TIM_Clock_En(TIM_6);
	TIM_Config(TIM6, SYSCLK, TIMER6_uS);
	TIM_DMA(TIM6, DMAEN);
	TIM_MasterMode(TIM6, MMS_UPDATE);
}
//------------------------------------------------------------------------------

void SPI_Config()
{
	SPI_Init(SPI1, SPI1_BAUDRATE_BITS, SPI1_LSB_FIRST);
	SPI_Init(SPI2, SPI2_BAUDRATE_BITS, SPI2_LSB_FIRST);
}
//------------------------------------------------------------------------------

void WatchDog_Config()
{
	IWDG_Config();
	IWDG_ConfigureFastUpdate();
}
//------------------------------------------------------------------------------
