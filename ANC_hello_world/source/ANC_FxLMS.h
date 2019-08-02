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
#define MAX_ORDER 50
//#define MAX(a,b) (((a)>(b))?(a):(b))
/***********************************************ENUMERATIONS AND STRUCTURES AND TYPEDEFS*********************************************/
typedef struct{
	q15_t SHat[MAX_SEC_PATH_ORDER]; //Estimación del camino secundario
	q15_t NoiseStates[MAX_WEIGHT_ORDER],SHatStates[MAX_SEC_PATH_ORDER];
	q15_t XFilt[MAX_WEIGHT_ORDER];
	q15_t Weights[MAX_WEIGHT_ORDER];
	q15_t mu,muSHat; //mu para el entrenamiento final, y para la estimacion del cam secundario.
	arm_lms_instance_q15 SecPathEst;
	uint32_t SHatOrder, WOrder;
}FxLMSInstanceQ;

typedef struct{
	float32_t SHatCoefs[MAX_ORDER],SHatStates[MAX_ORDER]; //Estimación del camino secundario
	float32_t ControllerCoefs[MAX_ORDER],ControllerLMSStates[MAX_ORDER],ControllerStates[MAX_ORDER];
	float32_t mu,muSHat; //mu para el entrenamiento final, y para la estimacion del cam secundario.
	arm_lms_instance_f32 SecPathEst,ControllerTuner;
	arm_fir_instance_f32 SHatFIR,ControllerFIR;
	uint32_t SHatOrder, COrder;
}FxLMSInstanceF;
/************************************************FUNCTION PROTOTYPES WITH GLOBAL SCOPE***********************************************/
void createFxLMSInstanceQ(FxLMSInstanceQ *I,float32_t muSHat,uint32_t SHatOrder,float32_t mu,uint32_t WOrder);
q15_t estSecPathQ(FxLMSInstanceQ* I,q15_t errMicSample);

void createFxLMSInstanceF(FxLMSInstanceF *I,float32_t muSHat,uint32_t SHatOrder,float32_t mu,uint32_t COrder);
q15_t estSecPathF(FxLMSInstanceF* I,q15_t errMicSample);
void saveSecPathF(FxLMSInstanceF *I);
q15_t applyFxLMSF(FxLMSInstanceF* I,q15_t err,q15_t ref,q15_t music);
#endif // ANC_FXLMSq_H_

