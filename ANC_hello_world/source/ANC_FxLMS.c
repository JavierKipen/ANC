/***************************************************************************//**
  @file     ANC_FxLMS.c
  @brief    Timer services for application layer
  @author   Javier	
 ******************************************************************************/

/*************************************************** INCLUDE HEADER FILES **********************************************************/
#include <ANC_FxLMS.h>
#include "whiteNoiseGen.h"

/******************************************** DEFINES, STATIC VARIABLES AND STRUCTS ************************************************/
#define EST_SEC_NORM_STD 0.5
/************************************FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE********************************/
void clearArrayq(q31_t * arr, uint32_t len);
void clearArrayf(float32_t * arr, uint32_t len);
void ADCInputTof(unsigned int * arrDAC,float32_t * arrF,unsigned int N);
void fToDACOutput(float32_t * arrF,unsigned int * arrDAC,unsigned int N);
void InputMeasToF(InputMeasure *a, float32_t * err,float32_t * ref,float32_t * music,unsigned int N);


/***************************************************GLOBAL FUNCTION DEFINITIONS*****************************************************/
#ifdef PROCESSING_W_F32
void createFxLMSInstance(FxLMSInstance *I,float32_t muSHat,uint32_t SHatOrder,float32_t mu,uint32_t COrder)
{
	I->SHatOrder=SHatOrder;I->mu=mu;
	I->muSHat=muSHat;I->COrder=COrder; //Se copian los datos, para float no hay que procesar nada
	clearArrayf(I->SHatCoefs,I->SHatOrder);
	clearArrayf(I->SHatStates,I->SHatOrder+BLOCKSIZE);
	clearArrayf(I->ControllerCoefs,I->COrder);
	clearArrayf(I->ControllerStates,I->COrder+BLOCKSIZE);
	clearArrayf(I->ControllerLMSStates,I->COrder+BLOCKSIZE);
	initWNG(); //Inicialización de los filtros, los lms y el generador de ruido blanco
	arm_lms_init_f32(&(I->SecPathEst), I->SHatOrder, I->SHatCoefs, I->SHatStates, I->muSHat, BLOCKSIZE);
	arm_lms_init_f32(&(I->ControllerTuner), I->COrder, I->ControllerCoefs, I->ControllerLMSStates, I->mu, BLOCKSIZE);
	arm_fir_init_f32(&(I->ControllerFIR), I->COrder,I->ControllerCoefs, I->ControllerStates, BLOCKSIZE);
}
void estSecPath(FxLMSInstance* I,unsigned int *errMicSamples,unsigned int *toOutput) //OJO CON ESTO QUE SI EL BLOCKSIZE ES MUY GRANDE LOS CEOFS SIEMPRE VAN A DAR 0!
{
	float32_t DACOutput[BLOCKSIZE],errMicFloat[BLOCKSIZE],err[BLOCKSIZE],OutputEst[BLOCKSIZE];

	whiteNoiseGen(DACOutput,EST_SEC_NORM_STD);
	ADCInputTof(errMicSamples,errMicFloat,BLOCKSIZE);
	arm_lms_f32(&(I->SecPathEst),I->prevOutputs, errMicFloat,OutputEst,err,BLOCKSIZE);
	fToDACOutput(DACOutput,toOutput,BLOCKSIZE);
	for(unsigned int i=0;i<BLOCKSIZE;i++)
		I->prevOutputs[i]=DACOutput[i];
}
void saveSecPath(FxLMSInstance *I)
{
	clearArrayf(I->SHatStates,I->SHatOrder);
	arm_fir_init_f32(&(I->SHatFIR), I->SHatOrder,I->SHatCoefs, I->SHatStates, BLOCKSIZE);
}
void applyFxLMS(FxLMSInstance * I,InputMeasure *samples,unsigned int * toOutput)
{
	float32_t err[BLOCKSIZE],ref[BLOCKSIZE],music[BLOCKSIZE],xHat[BLOCKSIZE],ControllerInput[BLOCKSIZE],retValF[BLOCKSIZE],aux1[BLOCKSIZE],aux2[BLOCKSIZE];
	InputMeasToF(samples, err,ref,music,BLOCKSIZE);
	for(unsigned int j=0;j<BLOCKSIZE;j++)
		ControllerInput[j]=ref[j]+music[j]; //Ponemos el Input al controller de una
	arm_fir_f32(&(I->SHatFIR),ref, xHat, BLOCKSIZE); //Checkeado anda bn
	arm_lms_f32(&(I->ControllerTuner),xHat,err,aux1,aux2,BLOCKSIZE);
	arm_fir_f32(&(I->ControllerFIR),ControllerInput,retValF, BLOCKSIZE);
	for(unsigned int j=0;j<BLOCKSIZE;j++)
		retValF[j]=-retValF[j]; //Esto no se muy bn porque
	fToDACOutput(retValF,toOutput,BLOCKSIZE);

}
#else
#ifdef PROCESSING_W_Q15
void createFxLMSInstanceF(FxLMSInstance *I,float32_t muSHat,uint32_t SHatOrder,float32_t mu,uint32_t COrder)
{

}
void estSecPathF(FxLMSInstance* I,unsigned int *errMicSamples,unsigned int *toOutput)
{

}
void saveSecPathF(FxLMSInstance *I)
{

}
void applyFxLMSF(FxLMSInstance * I,InputMeasure *samples,InputMeasure * toOutput)
{

}
#else //PROCESSING_W_Q31
void createFxLMSInstance(FxLMSInstance *I,float32_t muSHat,uint32_t SHatOrder,float32_t mu,uint32_t COrder)
{
	I->SHatOrder=SHatOrder;I->COrder=COrder;
	arm_float_to_q31(&muSHat, &I->muSHat, 1);
	arm_float_to_q31(&mu, &I->mu, 1);
	clearArrayq(I->SHatCoefs,I->SHatOrder);
	clearArrayq(I->SHatStates,I->SHatOrder+BLOCKSIZE);
	clearArrayq(I->ControllerCoefs,I->COrder);
	clearArrayq(I->ControllerStates,I->COrder+BLOCKSIZE);
	clearArrayq(I->ControllerLMSStates,I->COrder+BLOCKSIZE);
	initWNG(); //Inicialización de los filtros, los lms y el generador de ruido blanco
	arm_lms_init_q31(&(I->SecPathEst), I->SHatOrder, I->SHatCoefs, I->SHatStates, I->muSHat, BLOCKSIZE,0);
	arm_lms_init_q31(&(I->ControllerTuner), I->COrder, I->ControllerCoefs, I->ControllerLMSStates, I->mu, BLOCKSIZE,0);
	arm_fir_init_q31(&(I->ControllerFIR), I->COrder,I->ControllerCoefs, I->ControllerStates, BLOCKSIZE);
}
void estSecPath(FxLMSInstance* I,unsigned int *errMicSamples,unsigned int *toOutput)
{
	float32_t DACOutputF[BLOCKSIZE],errMicFloat[BLOCKSIZE];
	q31_t errMicQ[BLOCKSIZE],err[BLOCKSIZE],OutputEst[BLOCKSIZE];

	whiteNoiseGen(DACOutputF,EST_SEC_NORM_STD);
	ADCInputTof(errMicSamples,errMicFloat,BLOCKSIZE);
	arm_float_to_q31(errMicFloat, errMicQ, BLOCKSIZE);
	arm_lms_q31(&(I->SecPathEst),I->prevOutputs, errMicQ,OutputEst,err,BLOCKSIZE);
	fToDACOutput(DACOutputF,toOutput,BLOCKSIZE);
	arm_float_to_q31(DACOutputF,I->prevOutputs,BLOCKSIZE);


}
void saveSecPath(FxLMSInstance *I)
{
	clearArrayq(I->SHatStates,I->SHatOrder);
	arm_fir_init_q31(&(I->SHatFIR), I->SHatOrder,I->SHatCoefs, I->SHatStates, BLOCKSIZE);
}
void applyFxLMS(FxLMSInstance * I,InputMeasure *samples,unsigned int * toOutput)
{
	float32_t errF[BLOCKSIZE],refF[BLOCKSIZE],musicF[BLOCKSIZE],ControllerInputF[BLOCKSIZE],retValF[BLOCKSIZE];
	q31_t err[BLOCKSIZE],ref[BLOCKSIZE],xHat[BLOCKSIZE],ControllerInput[BLOCKSIZE],retVal[BLOCKSIZE];
	q31_t aux1[BLOCKSIZE],aux2[BLOCKSIZE];
	InputMeasToF(samples, errF,refF,musicF,BLOCKSIZE);
	for(unsigned int j=0;j<BLOCKSIZE;j++)
		ControllerInputF[j]=refF[j]+musicF[j]; //Ponemos el Input al controller de una
	arm_float_to_q31(refF, ref, BLOCKSIZE);arm_float_to_q31(errF, err, BLOCKSIZE);
	arm_fir_q31(&(I->SHatFIR),ref, xHat, BLOCKSIZE); //Checkeado anda bn
	arm_lms_q31(&(I->ControllerTuner),xHat,err,aux1,aux2,BLOCKSIZE);
	arm_float_to_q31(ControllerInputF, ControllerInput, BLOCKSIZE);
	arm_fir_q31(&(I->ControllerFIR),ControllerInput,retVal, BLOCKSIZE);
	arm_q31_to_float(retVal,retValF,BLOCKSIZE);
	for(unsigned int j=0;j<BLOCKSIZE;j++)
		retValF[j]=-retValF[j]; //Esto no se muy bn porque
	fToDACOutput(retValF,toOutput,BLOCKSIZE);

}
#endif
#endif

/****************************************************LOCAL FUNCTION DEFINITIONS*****************************************************/
void clearArrayq(q31_t * arr, uint32_t len)
{
	for(unsigned int i=0;i<len;i++)
		arr[i]=0;
}
void clearArrayf(float32_t * arr, uint32_t len)
{
	for(unsigned int i=0;i<len;i++)
		arr[i]=0;
}

void ADCInputTof(unsigned int * arrDAC,float32_t * arrF,unsigned int N)
{
	for(unsigned int i=0;i<N;i++)
	{
		arrF[i]=((float)arrDAC[i]-2048.0f)/2048.0f;
	}
}
void InputMeasToF(InputMeasure *a, float32_t * err,float32_t * ref,float32_t * music,unsigned int N)
{
	for(unsigned int i=0;i<N;i++)
	{
		err[i]=((float)a[i].errMicSample-2048.0f)/2048.0f;
		ref[i]=((float)a[i].noiseSample-2048.0f)/2048.0f;
		music[i]=((float)a[i].musicSample-2048.0f)/2048.0f;
	}

}
void fToDACOutput(float32_t * arrF,unsigned int * arrDAC,unsigned int N)
{
	float32_t aux;
	for(unsigned int i=0;i<N;i++)
	{
		aux= (arrF[i]>1.0)? 1:((arrF[i]<-1.0)? -1:arrF[i] );
		arrDAC[i]=(aux+1.0f)*2048.0f;
	}
}
