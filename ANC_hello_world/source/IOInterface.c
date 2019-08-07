/***************************************************************************//**
  @file     IOInterface.c
  @brief    Básicamente esta librería es la encargada de manejar todo el I/O. Con
  FTM, tiene una interrupción periódica de 40KHz. Allí manda a convertir Err y music
  a digital, y que se genere una interrupción cuando esté la conversión. Cuando está,
  se guardan los valores y se manda a convertir ref ( solo hay 2 adc y 3 cosas a leer)
  Estas conversiones se logran hacer antes de cada ciclo del FTM, por lo que al llegar
  a una nueva interrupt de FTM, se pasan las mediciones obtenidas y se pone el output
  pasado.
  @author   Javier	
 ******************************************************************************/

/*************************************************** INCLUDE HEADER FILES **********************************************************/
#include "IOInterface.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_adc16.h"
#include "fsl_ftm.h"
#include "fsl_dac.h"

#include "pin_mux.h"
#include "clock_config.h"




/******************************************** DEFINES, STATIC VARIABLES AND STRUCTS ************************************************/
#ifndef IO_FROM_PC
//FTM Defines
#define FTM_BASEADDR FTM0
#define FTM_IRQ_NUM FTM0_IRQn
#define FTM_HANDLER FTM0_IRQHandler
#define FTM_SOURCE_CLOCK (CLOCK_GetFreq(kCLOCK_BusClk)/4)

//ADC Defines
#define ADC_CHANNEL_GROUP 0U
#define ADC_ERR_CHANNEL 12U /* PTB1, ADC0_SE12 */
#define ADC_REF_CHANNEL 13U /* PTB2, ADC0_SE13 */
#define ADC_MUSIC_CHANNEL 14U /* PTB10, ADC1_SE14 */
#define ADC0_IRQ_HANDLER_FUNC ADC0_IRQHandler
#define ADC1_IRQ_HANDLER_FUNC ADC1_IRQHandler
typedef struct{
	ADC_Type * base;
	unsigned int chGroup,ch;
}ADCInInfo;

static ADCInInfo err={ADC0, ADC_CHANNEL_GROUP,ADC_ERR_CHANNEL};
static ADCInInfo ref={ADC0, ADC_CHANNEL_GROUP,ADC_REF_CHANNEL};
static ADCInInfo music={ADC1, ADC_CHANNEL_GROUP,ADC_MUSIC_CHANNEL};
//DAC Defines
#define DAC_BASEADDR DAC0
#define DAC_FREQ 40000

static unsigned int conv=0,measReady=0,nextOut=2048;
static InputMeasure meas;
static adc16_channel_config_t adc16ChannelConfigStruct; //faster sampling
/************************************FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE********************************/
int initTimer();
int initADC();
int initDAC();
unsigned int voltage2DACOutput(float v);
float ADCRead2Voltage(float i);

/***************************************************GLOBAL FUNCTION DEFINITIONS*****************************************************/
void initIOInterface()
{
	meas.musicSample=0;
	initTimer();
	initADC();
	initDAC();
	EnableIRQ(FTM_IRQ_NUM);
	FTM_StartTimer(FTM_BASEADDR, kFTM_SystemClock);
}
int newInputAvailable()
{
	return measReady;
}
InputMeasure getInputs()
{
	measReady=0;
	return meas;
}
void pushOutput(float output)
{
	nextOut=voltage2DACOutput(output);
}
int isProcessingTimeRisky(float processingTime)
{
	return processingTime>28e-6;
}
/****************************************************LOCAL FUNCTION DEFINITIONS*****************************************************/

int initTimer()
{
	int retVal=0;
	ftm_config_t ftmInfo;
	FTM_GetDefaultConfig(&ftmInfo);
	ftmInfo.prescale = kFTM_Prescale_Divide_4;
	FTM_Init(FTM_BASEADDR, &ftmInfo);
	FTM_SetTimerPeriod(FTM_BASEADDR, USEC_TO_COUNT(28U, FTM_SOURCE_CLOCK)); // Revisar freq de sampling.
	FTM_EnableInterrupts(FTM_BASEADDR, kFTM_TimeOverflowInterruptEnable);
	return retVal;
}
int initADC()
{
	int retVal=0;
	adc16_config_t adc16ConfigStruct;
	ADC16_GetDefaultConfig(&adc16ConfigStruct);
	adc16ConfigStruct.clockDivider = kADC16_ClockDivider1;
	//adc16ConfigStruct.enableHighSpeed = true;
	ADC16_Init(ADC0, &adc16ConfigStruct);
	ADC16_Init(ADC1, &adc16ConfigStruct);
	ADC16_EnableHardwareTrigger(ADC1, false);
	ADC16_DoAutoCalibration(ADC1);
	ADC16_EnableHardwareTrigger(ADC0, false);
	ADC16_DoAutoCalibration(ADC0);
	EnableIRQ(ADC0_IRQn);
	EnableIRQ(ADC1_IRQn);
	adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = true;
	adc16ChannelConfigStruct.enableDifferentialConversion = false;
	return retVal;

}
int initDAC()
{
	int retVal=0;
	dac_config_t dacConfigStruct;
	DAC_GetDefaultConfig(&dacConfigStruct);
	DAC_Init(DAC_BASEADDR, &dacConfigStruct);
	DAC_Enable(DAC_BASEADDR, true);
	DAC_SetBufferReadPointer(DAC_BASEADDR, 0U);
	return retVal;
}



void FTM_HANDLER(void)
{
    /* Clear interrupt flag.*/
	//DAC_SetBufferValue(DAC_BASEADDR, 0U, 2100);
	DAC_SetBufferValue(DAC_BASEADDR, 0U, nextOut); //Ponemos el output
    FTM_ClearStatusFlags(FTM_BASEADDR, kFTM_TimeOverflowFlag);

    //Hacer las meas de los inputs:
    conv=0;measReady=1;

    adc16ChannelConfigStruct.channelNumber = err.ch;
    ADC16_SetChannelConfig(err.base, err.chGroup, &adc16ChannelConfigStruct); //Mandamos a convertir el error
    //adc16ChannelConfigStruct.channelNumber = music.ch;
    //ADC16_SetChannelConfig(music.base, music.chGroup, &adc16ChannelConfigStruct); //Y también a convertir la música
    //DAC_SetBufferValue(DAC_BASEADDR, 0U, 1900);
    __DSB();
}

void ADC0_IRQHandler(void)
{
	if(conv==0)
	{
		measReady=0;
		//DAC_SetBufferValue(DAC_BASEADDR, 0U, 1000);
		meas.errMicSample = ADC16_GetChannelConversionValue(err.base, err.chGroup);
		conv=1;
		adc16ChannelConfigStruct.channelNumber = ref.ch;
		ADC16_SetChannelConfig(ref.base, ref.chGroup, &adc16ChannelConfigStruct);
		//DAC_SetBufferValue(DAC_BASEADDR, 0U, 2000);
	}
	else
	{
		//DAC_SetBufferValue(DAC_BASEADDR, 0U, 2500);
		meas.noiseSample = ADC16_GetChannelConversionValue(ref.base, ref.chGroup);
		//DAC_SetBufferValue(DAC_BASEADDR, 0U, 3500);
		meas.noiseSample=(meas.noiseSample-2048.0f)/1241.21f; //ADCRead2Voltage faster
		meas.errMicSample=(meas.errMicSample-2048.0f)/1241.21f;
		//meas.musicSample=(meas.musicSample-2048.0f)/1241.21f;
		//DAC_SetBufferValue(DAC_BASEADDR, 0U, 2000);

	}
}//28us
void ADC1_IRQHandler(void)
{
	meas.musicSample = ADC16_GetChannelConversionValue(music.base, music.chGroup);
}
unsigned int voltage2DACOutput(float v)
{
	unsigned int DACOut;
	if(v > 1.65) //Clips the value of v;
		v=1.65;
	else if(v < -1.65)
		v=-1.65;
	v+=1.65; //Now v goes from 0 to 3.3, and the dac from 0 to 2^12-1;
	DACOut=(unsigned int)(v*1240.90909);
	return DACOut;
}
float ADCRead2Voltage(float i)
{
	return (i-2048.0f)/1241.21f;
}
#else

#include "fsl_uart.h"

#define DEMO_UART_CLKSRC UART0_CLK_SRC
#define DEMO_UART_CLK_FREQ CLOCK_GetFreq(UART0_CLK_SRC)

void initIOInterface()
{
	uart_config_t config;
	UART_GetDefaultConfig(&config); //config.baudRate_Bps = 115200U;
	config.enableTx = true;
	config.enableRx = true;
	UART_Init(UART0, &config, DEMO_UART_CLK_FREQ);
}
int newInputAvailable()
{
	return 1;
}
#ifdef EST_SEC_TEST
void getInputs(InputMeasure *Inputs)
{
	uint8_t read[2*BLOCKSIZE],offset;
	UART_ReadBlocking(UART0, read, 2*BLOCKSIZE); //2 bytes para cada read
	for(unsigned int i=0;i<BLOCKSIZE;i++)
	{
		offset=i*BLOCKSIZE;
		Inputs[i].noiseSample=0;
		Inputs[i].errMicSample=(((uint16_t)read[1+offset])<<8)+((uint16_t)read[offset]);
		Inputs[i].musicSample=0;
	}

}
#else
void getInputs(InputMeasure *Inputs)
{
	uint8_t read[6*BLOCKSIZE],offset;
	UART_ReadBlocking(UART0, read, 6*BLOCKSIZE); //2 bytes para cada read
	for(unsigned int i=0;i<BLOCKSIZE;i++)
	{
		offset=i*BLOCKSIZE;
		Inputs[i].noiseSample=(((uint16_t)read[1+offset])<<8)+((uint16_t)read[0+offset]);
		Inputs[i].errMicSample=(((uint16_t)read[3+offset])<<8)+((uint16_t)read[2+offset]);
		Inputs[i].musicSample=(((uint16_t)read[5+offset])<<8)+((uint16_t)read[4+offset]);
	}

}

#endif
void pushOutput(unsigned int *outputs)
{
	uint8_t toSend[2*BLOCKSIZE];
	unsigned int offset=0;
	for(unsigned int i=0;i<BLOCKSIZE;i++)
	{
		offset=i*BLOCKSIZE;
		toSend[offset]=outputs[i]&0xFF;
		toSend[1+offset]=outputs[i]>>8;
	}
	UART_WriteBlocking(UART0, toSend, 2*BLOCKSIZE);
}

int isProcessingTimeRisky(float processingTime)
{
	return 0;
}
#endif
