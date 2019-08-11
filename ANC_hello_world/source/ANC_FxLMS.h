/*****************************************************************************
  @file     ANC_FxLMS.h
  @brief    
  @author   Javier
 ******************************************************************************/

#ifndef ANC_FXLMSQ_H_
#define ANC_FXLMSQ_H_


/*******************************************************INCLUDE HEADER FILES********************************************************/
#include "arm_math.h"
#include "CommonConfig.h"
/*********************************************CONSTANT AND MACRO DEFINITIONS USING #DEFINE******************************************/
#define MAX_ORDER 50

/* Here is the sketch of the system.
%
%              +-----------+                       +
% ref(k)--+--->|   P(z)    |--yp(k)----------------> sum --+---> e(k)
%         |    +-----------+                          ^-   |
%         |                                           |    |
%         |        \                                ys(k)  |
%         |    +-----------+          +-----------+   |    |
%         +--->|   C(z)    |--Out(k)->|   S(z)    |---+    |
%         |    +-----------+          +-----------+        |
%         |            \                                   |
%         |             \----------------\                 |
%         |                               \                |
%         |    +-----------+          +-----------+        |
%         +--->|  SHat(z)  |-XHat(k)->|    LMS    |<-------+
%              +-----------+          +-----------+        */

/***********************************************ENUMERATIONS AND STRUCTURES AND TYPEDEFS*********************************************/
#ifdef PROCESSING_W_F32
typedef struct{
	float32_t SHatCoefs[MAX_ORDER],SHatStates[MAX_ORDER+BLOCKSIZE]; //Estimación del camino secundario
	float32_t ControllerCoefs[MAX_ORDER],ControllerLMSStates[MAX_ORDER+BLOCKSIZE],ControllerStates[MAX_ORDER+BLOCKSIZE];
	float32_t prevOutputs[BLOCKSIZE];
	float32_t mu,muSHat; //mu para el entrenamiento final, y para la estimacion del cam secundario.
	arm_lms_instance_f32 SecPathEst,ControllerTuner;
	arm_fir_instance_f32 SHatFIR,ControllerFIR;
	uint32_t SHatOrder, COrder;
}FxLMSInstance;
#else
	#ifdef PROCESSING_W_Q15
typedef struct{
	q15_t SHatCoefs[MAX_ORDER],SHatStates[MAX_ORDER+BLOCKSIZE]; //Estimación del camino secundario
	q15_t ControllerCoefs[MAX_ORDER],ControllerLMSStates[MAX_ORDER+BLOCKSIZE],ControllerStates[MAX_ORDER+BLOCKSIZE];
	q15_t mu,muSHat; //mu para el entrenamiento final, y para la estimacion del cam secundario.
	arm_lms_instance_q15 SecPathEst,ControllerTuner;
	arm_fir_instance_q15 SHatFIR,ControllerFIR;
	uint32_t SHatOrder, COrder;
}FxLMSInstance;
	#else //PROCESSING_W_Q31
typedef struct{
	q31_t SHatCoefs[MAX_ORDER],SHatStates[MAX_ORDER+BLOCKSIZE]; //Estimación del camino secundario
	q31_t ControllerCoefs[MAX_ORDER],ControllerLMSStates[MAX_ORDER+BLOCKSIZE],ControllerStates[MAX_ORDER+BLOCKSIZE];
	q31_t prevOutputs[BLOCKSIZE];
	q31_t mu,muSHat; //mu para el entrenamiento final, y para la estimacion del cam secundario.
	arm_lms_instance_q31 SecPathEst,ControllerTuner;
	arm_fir_instance_q31 SHatFIR,ControllerFIR;
	uint32_t SHatOrder, COrder;
}FxLMSInstance;
	#endif
#endif
/************************************************FUNCTION PROTOTYPES WITH GLOBAL SCOPE***********************************************/
void createFxLMSInstance(FxLMSInstance *I,float32_t muSHat,uint32_t SHatOrder,float32_t mu,uint32_t COrder);
void estSecPath(FxLMSInstance* I,unsigned int *errMicSamples,unsigned int *toOutput);//Se tomará el input como un array de BLOCKSIZE elementos
void saveSecPath(FxLMSInstance *I);
void applyFxLMS(FxLMSInstance * I,InputMeasure *samples,unsigned int * toOutput);//Se tomará el input como un array de BLOCKSIZE elementos
#endif // ANC_FXLMSq_H_

