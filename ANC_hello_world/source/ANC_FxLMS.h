/*****************************************************************************
  @file     ANC_FxLMS.h
  @brief    
  @author   Javier
 ******************************************************************************/

#ifndef ANC_FXLMS_H_
#define ANC_FXLMS_H_


/*******************************************************INCLUDE HEADER FILES********************************************************/
#include "arm_math.h"
/*********************************************CONSTANT AND MACRO DEFINITIONS USING #DEFINE******************************************/
#define MAX_SEC_PATH_ORDER 50
#define MAX_WEIGHT_ORDER 25
#define MAX(a,b) (((a)>(b))?(a):(b))
/***********************************************ENUMERATIONS AND STRUCTURES AND TYPEDEFS*********************************************/
typedef struct{
	float32_t SHat[MAX_SEC_PATH_ORDER];
	float32_t NoiseStates[MAX_WEIGHT_ORDER],SHatStates[MAX_SEC_PATH_ORDER];
	float32_t SHatEst[MAX_SEC_PATH_ORDER];
	float32_t XFilt[MAX_WEIGHT_ORDER];
	float32_t Weights[MAX_WEIGHT_ORDER];
	float32_t mu,muSHat;
	uint32_t SHatOrder, WOrder;
}FxLMSInstance;
/************************************************FUNCTION PROTOTYPES WITH GLOBAL SCOPE***********************************************/
FxLMSInstance createFxLMSInstance(float32_t muSHat,uint32_t SHatOrder,float32_t mu,uint32_t WOrder);
float32_t applyFxLMS(FxLMSInstance* I, float32_t noiseSample,float32_t musicSample,float32_t errMicSample);
float32_t estSecPath(FxLMSInstance* I,float32_t errMicSample);
#endif // ANC_FXLMS_H_

