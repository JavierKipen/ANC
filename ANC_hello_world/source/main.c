#include "math_helper.h"
#include "fsl_device_registers.h"
//#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "IOInterface.h"
#include "ANC_FxLMS.h"

#include "fsl_dac.h"
#include <stdlib.h>

#define DAC_BASEADDR DAC0

//#define ANC_F
//#define TEST_FLMS_SECPATH


#define CALIB_SAMPLES 100000
typedef enum{CALIBRATION, ANC, ERROR_ANC}ANC_STATES;

float32_t SHatPrev[50];

float32_t micTakes[2000];
float32_t noiseOut[2000];

int32_t main(void)
{
  BOARD_InitPins();
  BOARD_BootClockRUN();
  BOARD_InitDebugConsole();

  initIOInterface();
#ifndef ANC_F
  float32_t muSHat=1;float32_t mu=0.1;
  uint32_t SHatOrder=8;uint32_t WOrder=4;uint32_t i=0;
#ifdef FLMS_SECPATH
  FxLMSInstanceF I;
  createFxLMSInstanceF(&I,muSHat,SHatOrder,mu,WOrder);
  while(1)
  {
	  InputMeasure im=getInputs();
	  q15_t sendVal = estSecPathF(&I,im.errMicSample);
	  pushOutput(sendVal);
	  i++;
	  if(i==10000)
		  i++; //Esto parece funcionar
  }
#else
  FxLMSInstanceQ I;
    createFxLMSInstanceQ(&I,muSHat,SHatOrder,mu,WOrder);
    while(1)
    {
  	  InputMeasure im=getInputs();
  	  q15_t sendVal = estSecPathQ(&I,im.errMicSample);
  	  pushOutput(sendVal);
  	  i++;
  	  if(i==10000)
  		  i++; //Esto parece funcionar
    }
#endif

#else
  ANC_STATES state=CALIBRATION;
  //clock_t start;

  float32_t muSHat=0.01;float32_t mu=0.01;
  uint32_t SHatOrder=6;uint32_t WOrder=4;uint32_t sampleCount=0;



  while(state != ERROR_ANC)
  {
	  if(newInputAvailable())
	  {
		  InputMeasure im=getInputs();
		  if(state==CALIBRATION)
		  {
			  pushOutput(estSecPath(&I,im.errMicSample));
		  	  sampleCount++;
		  }
		  else if(state==ANC)
			  pushOutput(applyFxLMS(&I, im.noiseSample,im.musicSample,im.errMicSample));

		  if(state==CALIBRATION && sampleCount > CALIB_SAMPLES)
			  state=ANC;
		  if(isProcessingTimeRisky((float32_t)(clock()-start)/CLOCKS_PER_SEC))
			  state=ERROR_ANC;
		 /* if(state==CALIBRATION && sampleCount%10000 == 0)
		  {
			  float err=0;
			  for(unsigned int j=0; j<SHatOrder;j++)
				  err+=(I.SHat[j]-SHatPrev[j])*(I.SHat[j]-SHatPrev[j]);
			  memcpy(SHatPrev,I.SHat,SHatOrder*sizeof(float));
			  PRINTF("Cost: %d \n",(unsigned int)(err*10000000));

		  }*/
	  }
  }
  while(1);
#endif
  return 0;

}
