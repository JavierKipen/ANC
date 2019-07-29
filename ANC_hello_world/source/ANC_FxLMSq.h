/*****************************************************************************
  @file     ANC_FxLMS.h
  @brief    
  @author   Javier
 ******************************************************************************/

#ifndef ANC_FXLMSQ_H_
#define ANC_FXLMSQ_H_


/*******************************************************INCLUDE HEADER FILES********************************************************/
#include "arm_math.h"
/*********************************************CONSTANT AND MACRO DEFINITIONS USING #DEFINE******************************************/
#define MAX_SEC_PATH_ORDER 50
#define MAX_WEIGHT_ORDER 25
#define MAX(a,b) (((a)>(b))?(a):(b))
/***********************************************ENUMERATIONS AND STRUCTURES AND TYPEDEFS*********************************************/
typedef struct{
	q15_t SHat[MAX_SEC_PATH_ORDER]; //Estimación del camino secundario
	q15_t NoiseStates[MAX_WEIGHT_ORDER],SHatStates[MAX_SEC_PATH_ORDER];
	q15_t SHatEst[MAX_SEC_PATH_ORDER];
	q15_t XFilt[MAX_WEIGHT_ORDER];
	q15_t Weights[MAX_WEIGHT_ORDER];
	q15_t mu,muSHat; //mu para el entrenamiento final, y para la estimacion del cam secundario.
	uint32_t SHatOrder, WOrder;
}FxLMSInstance;
/************************************************FUNCTION PROTOTYPES WITH GLOBAL SCOPE***********************************************/
FxLMSInstance createFxLMSInstance(float32_t muSHat,uint32_t SHatOrder,float32_t mu,uint32_t WOrder);
q15_t estSecPath(FxLMSInstance* I,q15_t errMicSample);
#endif // ANC_FXLMSq_H_

