/***************************************************************************//**
  @file     ANC_FxLMS.c
  @brief    Timer services for application layer
  @author   Javier	
 ******************************************************************************/

/*************************************************** INCLUDE HEADER FILES **********************************************************/
#include "whiteNoiseGen.h"
#include "ANC_FxLMS.h"

/******************************************** DEFINES, STATIC VARIABLES AND STRUCTS ************************************************/
#define EST_SEC_NORM_STD 0.02
/************************************FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE********************************/
void pushFrontFl(float32_t * arr, uint32_t len, float32_t data);
void clearArray(float32_t * arr, uint32_t len);
/***************************************************GLOBAL FUNCTION DEFINITIONS*****************************************************/
FxLMSInstance createFxLMSInstance(float32_t muSHat,uint32_t SHatOrder,float32_t mu,uint32_t WOrder)
{
	FxLMSInstance retVal; //Init
	retVal.SHatOrder=SHatOrder;
	retVal.mu=mu; retVal.muSHat=muSHat;
	retVal.WOrder=WOrder;
	clearArray(retVal.SHat,retVal.SHatOrder);
	clearArray(retVal.SHatStates,retVal.SHatOrder);
	clearArray(retVal.SHatEst,retVal.SHatOrder);
	clearArray(retVal.XFilt,retVal.WOrder);
	clearArray(retVal.NoiseStates,retVal.WOrder);
	clearArray(retVal.Weights,retVal.WOrder);
	initWNG();
	return retVal;
}
float32_t applyFxLMS(FxLMSInstance* I, float32_t noiseSample,float32_t musicSample,float32_t errMicSample)
{
	float32_t aux[MAX_WEIGHT_ORDER], output,XFilt;
	arm_scale_f32(I->XFilt, I->mu*errMicSample, aux, I->WOrder);
	arm_add_f32(I->Weights, aux, I->Weights, I->WOrder); //Given the output, Adjusts weights from previous states

	pushFrontFl(I->NoiseStates, I->WOrder, noiseSample); //Obtains New states
	pushFrontFl(I->SHatStates, I->SHatOrder, noiseSample);
	arm_dot_prod_f32(I->SHat, I->SHatStates, I->SHatOrder, &XFilt);
	pushFrontFl(I->XFilt, I->WOrder, XFilt);
	arm_dot_prod_f32(I->Weights, I->NoiseStates, I->WOrder, &output);//Noise at err mic estimation
	output*=(-1); //IMPORTANTISIMOOOO!
	output+=musicSample;
	return output;
}
float32_t estSecPath(FxLMSInstance* I,float32_t errMicSample)
{
	float32_t noiseIn=whiteNoiseGen(EST_SEC_NORM_STD); //To get normal random noise.
	float32_t errMicHat, err, aux[MAX_SEC_PATH_ORDER];
	arm_dot_prod_f32(I->SHat, I->SHatEst, I->SHatOrder, &errMicHat);
	err= errMicSample-errMicHat;
	arm_scale_f32(I->SHatEst, I->muSHat*err, aux, I->SHatOrder);
	arm_add_f32(I->SHat, aux, I->SHat, I->SHatOrder);
	pushFrontFl(I->SHatEst, I->SHatOrder, noiseIn);
	return noiseIn;
}
/****************************************************LOCAL FUNCTION DEFINITIONS*****************************************************/
void pushFrontFl(float32_t * arr, uint32_t len, float32_t data)
{
	arm_copy_f32 (arr, arr+1, len-1);
	arr[0]=data;
}
void clearArray(float32_t * arr, uint32_t len)
{
	for(unsigned int i=0;i<len;i++)
		arr[i]=0;
}
