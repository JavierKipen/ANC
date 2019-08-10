#include "fsl_device_registers.h"
//#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "IOInterface.h"
#include "ANC_FxLMS.h"

#include "fsl_dac.h"
#include <stdlib.h>
#include "CommonConfig.h"

#define DAC_BASEADDR DAC0

//#define ANC_FINAL
//#define TEST_FLMS_SECPATH


#define CALIB_SAMPLES 10000
typedef enum{CALIBRATION, ANC, ERROR_ANC}ANC_STATES;

void extractErr(InputMeasure * InputMeasure,unsigned int *errMicSamples);

int32_t main(void)
{
  BOARD_InitPins();
  BOARD_BootClockRUN();
  BOARD_InitDebugConsole();

  initIOInterface();
#ifndef IO_FROM_PC
	ANC_STATES state=CALIBRATION;
	float32_t muSHat=0.01;float32_t mu=0.01;
	uint32_t SHatOrder=8;uint32_t WOrder=10;uint32_t sampleCount=0;
	InputMeasure im[BLOCKSIZE];
	unsigned int DACOut[BLOCKSIZE],errMicSamples[BLOCKSIZE];
	FxLMSInstance I;
	createFxLMSInstance(&I,muSHat,SHatOrder,mu,WOrder);
	while(state != ERROR_ANC)
	{
	  if(newInputAvailable())
	  {
		  getInputs(im);
		  if(state==CALIBRATION)
		  {
			  extractErr(im,errMicSamples);
			  estSecPath(&I,errMicSamples,DACOut);
			  pushOutput(DACOut);
			  sampleCount++;
		  }
		  else if(state==ANC)
		  {
			  applyFxLMS(&I,im,DACOut);//Se tomará el input como un array de BLOCKSIZE elementos
			  pushOutput(DACOut);
		  }

		  if(state==CALIBRATION && sampleCount > CALIB_SAMPLES)
		  {	state=ANC;saveSecPath(&I);}
		  //if(isProcessingTimeRisky((float32_t)(clock()-start)/CLOCKS_PER_SEC))
			 // state=ERROR_ANC;
	  }
	}
	while(1);
#else

#ifdef EST_SEC_TEST//Second path test
	float32_t muSHat=0.01;float32_t mu=0.01;
	uint32_t SHatOrder=8;uint32_t WOrder=10;
	InputMeasure im[BLOCKSIZE];
	unsigned int DACOut[BLOCKSIZE],errMicSamples[BLOCKSIZE],i=0;
	FxLMSInstance I;
	createFxLMSInstance(&I,muSHat,SHatOrder,mu,WOrder);
	pushOutput(0); //Manda cero para empezar debug con matlab
	while(1)
	{
		getInputs(im);
		extractErr(im,errMicSamples);
		estSecPath(&I,errMicSamples,DACOut);
		pushOutput(DACOut);
		i++;
		if(i==10000)
			i++;
	}
#else //ANC  test ya obtenido el sec path
	ANC_STATES state=CALIBRATION;
	float32_t muSHat=0.01;float32_t mu=0.01;
	uint32_t SHatOrder=8;uint32_t WOrder=10;
	InputMeasure im[BLOCKSIZE];
	unsigned int DACOut[BLOCKSIZE];
	FxLMSInstance I;
	createFxLMSInstance(&I,muSHat,SHatOrder,mu,WOrder);
	float32_t secPathCoefs[]={0.1, 0.2, 0.3,0.1,0};
	arm_copy_f32 (secPathCoefs, I.SHatCoefs, 5);
	saveSecPath(&I);
	pushOutput(0);//To start matlab script
	while(1)
	{
		getInputs(im);
		applyFxLMS(&I,im,DACOut);
		pushOutput(DACOut);
	}
#endif

#endif
  return 0;

}

void extractErr(InputMeasure * InputMeasure,unsigned int *errMicSamples)
{
	for(unsigned int i=0;i<BLOCKSIZE;i++)
	{
		errMicSamples[i]=InputMeasure[i].errMicSample;
	}
}
