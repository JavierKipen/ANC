/***************************************************************************//**
  @file     ANC_FxLMS.c
  @brief    Timer services for application layer
  @author   Javier	
 ******************************************************************************/

/*************************************************** INCLUDE HEADER FILES **********************************************************/
#include <ANC_FxLMSq.h>
#include "whiteNoiseGen.h"

/******************************************** DEFINES, STATIC VARIABLES AND STRUCTS ************************************************/
#define EST_SEC_NORM_STD 0.02
/************************************FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE********************************/
void pushFrontFl(q15_t * arr, uint32_t len, q15_t data);
void clearArray(q15_t * arr, uint32_t len);
/***************************************************GLOBAL FUNCTION DEFINITIONS*****************************************************/
FxLMSInstance createFxLMSInstance(float32_t muSHat,uint32_t SHatOrder,float32_t mu,uint32_t WOrder)
{
	FxLMSInstance retVal; //Init
	retVal.SHatOrder=SHatOrder;
	arm_float_to_q15(&mu,&retVal.mu,1);
	arm_float_to_q15(&muSHat,&retVal.muSHat,1);
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
q15_t estSecPath(FxLMSInstance* I,q15_t errMicSample)
{
	float32_t noise2Output=whiteNoiseGen(EST_SEC_NORM_STD); //To get normal random noise.
	q15_t Output,errMicHat, err,auxScalar,auxArray[MAX_SEC_PATH_ORDER];
	arm_float_to_q15(&noise2Output,&Output,1);
	/*pushFrontFl(I->SHatEst, I->SHatOrder, Output);
	arm_dot_prod_q15(I->SHat, I->SHatEst, I->SHatOrder, &errMicHat);
	err = errMicSample-errMicHat;
	arm_scale_q15(&(I->muSHat),err,0,&auxScalar,1);
	arm_scale_q15(I->SHatEst, auxScalar,0,(q15_t*)auxArray,I->SHatOrder);
	arm_add_q15(I->SHat, auxArray, I->SHat, I->SHatOrder);*/

	return Output;
}
/****************************************************LOCAL FUNCTION DEFINITIONS*****************************************************/
void pushFrontFl(q15_t * arr, uint32_t len, q15_t data)
{
	arm_copy_q15 (arr, arr+1, len-1);
	arr[0]=data;
}
void clearArray(q15_t * arr, uint32_t len)
{
	for(unsigned int i=0;i<len;i++)
		arr[i]=0;
}
