/***************************************************************************//**
  @file     ANC_FxLMS.c
  @brief    Timer services for application layer
  @author   Javier	
 ******************************************************************************/

/*************************************************** INCLUDE HEADER FILES **********************************************************/
#include <ANC_FxLMS.h>
#include "whiteNoiseGen.h"


float32_t outputDebug[1000],micDebug[1000],X[1000],X2[1000];
static unsigned int cont=0;

/******************************************** DEFINES, STATIC VARIABLES AND STRUCTS ************************************************/
#define EST_SEC_NORM_STD 0.05
/************************************FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE********************************/
void pushFrontFlq(q15_t * arr, uint32_t len, q15_t data);
void clearArrayq(q15_t * arr, uint32_t len);
void pushFrontFlf(float32_t * arr, uint32_t len, float32_t data);
void clearArrayf(float32_t * arr, uint32_t len);
/***************************************************GLOBAL FUNCTION DEFINITIONS*****************************************************/
void createFxLMSInstanceQ(FxLMSInstanceQ *I, float32_t muSHat,uint32_t SHatOrder,float32_t mu,uint32_t WOrder)
{
	I->SHatOrder=SHatOrder;
	arm_float_to_q15(&mu,&I->mu,1);
	arm_float_to_q15(&muSHat,&I->muSHat,1);
	I->WOrder=WOrder;
	clearArrayq(I->SHat,I->SHatOrder);
	clearArrayq(I->SHatStates,I->SHatOrder);
	clearArrayq(I->XFilt,I->WOrder);
	clearArrayq(I->NoiseStates,I->WOrder);
	clearArrayq(I->Weights,I->WOrder);
	initWNG();
	arm_lms_init_q15(&(I->SecPathEst), I->SHatOrder, I->SHat, I->SHatStates, I->muSHat, 1, 4);
}
q15_t estSecPathQ(FxLMSInstanceQ* I,q15_t errMicSample)
{
	float32_t noise2Output=whiteNoiseGen(EST_SEC_NORM_STD); //To get normal random noise.
	q15_t DACOutput, err,OutputEst;
	arm_float_to_q15(&noise2Output,&DACOutput,1);
	arm_lms_q15(&(I->SecPathEst),&DACOutput, &errMicSample,&OutputEst,&err,1); //Por que mierda no anda la re concha del pato

	return DACOutput;
}
void createFxLMSInstanceF(FxLMSInstanceF *I,float32_t muSHat,uint32_t SHatOrder,float32_t mu,uint32_t WOrder)
{
	I->SHatOrder=SHatOrder;I->mu=mu;
	I->muSHat=muSHat;I->WOrder=WOrder;
	clearArrayf(I->SHat,I->SHatOrder);
	clearArrayf(I->SHatStates,I->SHatOrder);
	clearArrayf(I->XFilt,I->WOrder);
	clearArrayf(I->NoiseStates,I->WOrder);
	clearArrayf(I->Weights,I->WOrder);
	initWNG();
	arm_lms_init_f32(&(I->SecPathEst), I->SHatOrder, I->SHat, I->SHatStates, I->muSHat, 1);

}
q15_t estSecPathF(FxLMSInstanceF* I,q15_t errMicSample)
{
	float32_t DACOutput=whiteNoiseGen(EST_SEC_NORM_STD); //To get normal random noise.
	float32_t errMicFloat,err,OutputEst;
	q15_t DACOutq15;
	//outputDebug[cont]=DACOutput;
	arm_q15_to_float(&errMicSample,&errMicFloat,1);
	//micDebug[cont++]=errMicFloat;
	arm_lms_f32(&(I->SecPathEst),&DACOutput, &errMicFloat,&OutputEst,&err,1); //Por que mierda no anda la re concha del pato
	arm_float_to_q15(&DACOutput,&DACOutq15,1);
	//if(cont==5)
		//arm_lms_f32(&(I->SecPathEst),outputDebug, micDebug,X,X2,1000);
	return DACOutq15;
}

/****************************************************LOCAL FUNCTION DEFINITIONS*****************************************************/
void pushFrontFlq(q15_t * arr, uint32_t len, q15_t data)
{
	arm_copy_q15 (arr, arr+1, len-1);
	arr[0]=data;
}
void clearArrayq(q15_t * arr, uint32_t len)
{
	for(unsigned int i=0;i<len;i++)
		arr[i]=0;
}
void pushFrontFlf(float32_t * arr, uint32_t len, float32_t data)
{
	arm_copy_f32(arr, arr+1, len-1);
	arr[0]=data;
}
void clearArrayf(float32_t * arr, uint32_t len)
{
	for(unsigned int i=0;i<len;i++)
		arr[i]=0;
}
