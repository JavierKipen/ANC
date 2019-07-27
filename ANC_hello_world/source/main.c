#include "ANC_FxLMS.h"
#include "math_helper.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "IOInterface.h"

#include "boxmuller.h" //A borrar esto
#include "fsl_dac.h"
#include <stdlib.h>

#define DAC_BASEADDR DAC0


//#define REPLICATE_NOISE //Esto para probar I/O
//#define ONLY_EST_2ND_PATH
//#define TEST_BOX_MULLER
#define ANC_F


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
#ifdef REPLICATE_NOISE
  while(1)
  {
	  if(newInputAvailable())
	  {
		  InputMeasure im=getInputs();
		  pushOutput(0); //No me di cuenta que esto podía ser inestable por el crosstalk del parlante con mic
	  }
  }
#endif

#ifdef ONLY_EST_2ND_PATH
  float32_t muSHat=0.0001;float32_t mu=0.001;
  uint32_t SHatOrder=5;uint32_t WOrder=15;uint32_t sampleCount=0;
  FxLMSInstance I=createFxLMSInstance(muSHat,SHatOrder,mu,WOrder);
  while(1)
  {
	  if(newInputAvailable())
	  {
		  InputMeasure im=getInputs();
		  pushOutput(estSecPath(&I,im.errMicSample));
	  }
  }
#endif

#ifdef ANC_F
  ANC_STATES state=CALIBRATION;
  //clock_t start;

  float32_t muSHat=0.01;float32_t mu=0.01;
  uint32_t SHatOrder=6;uint32_t WOrder=4;uint32_t sampleCount=0;
  FxLMSInstance I=createFxLMSInstance(muSHat,SHatOrder,mu,WOrder);


  unsigned int i=0;
  while(state != ERROR_ANC)
  {
	  if(newInputAvailable())
	  {
		  InputMeasure im=getInputs();
		  if(state==CALIBRATION)
		  {
			  //micTakes[i]=im.errMicSample;
			  //noiseOut[i]=estSecPath(&I,micTakes[i]);
			  //pushOutput(noiseOut[i++]);
			  pushOutput(estSecPath(&I,im.errMicSample));
		  	  sampleCount++;
		  }
		 /* if(state==CALIBRATION && sampleCount%10000 == 0)
		  {
			  float err=0;
			  for(unsigned int j=0; j<SHatOrder;j++)
				  err+=(I.SHat[j]-SHatPrev[j])*(I.SHat[j]-SHatPrev[j]);
			  memcpy(SHatPrev,I.SHat,SHatOrder*sizeof(float));
			  PRINTF("Cost: %d \n",(unsigned int)(err*10000000));

		  }*/


		 // else if(state==ANC)
			//  pushOutput(applyFxLMS(&I, im.noiseSample,im.musicSample,im.errMicSample));

		 // if(state==CALIBRATION && sampleCount > CALIB_SAMPLES)
			 // state=ANC;
//		  if(isProcessingTimeRisky((float32_t)(clock()-start)/CLOCKS_PER_SEC))
//		      state=ERROR_ANC;
	  }
  }
  while(1);
  return state;
#endif

}
