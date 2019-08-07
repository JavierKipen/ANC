/*****************************************************************************
  @file     CommonConfig.h
  @brief    
  @author   Archivo con las cosas en común para la estructura del proyecto
 ******************************************************************************/

#ifndef COMMONCONFIG_H_
#define COMMONCONFIG_H_

/*******************************************************INCLUDE HEADER FILES********************************************************/

/*********************************************CONSTANT AND MACRO DEFINITIONS USING #DEFINE******************************************/

/***********************************************ENUMERATIONS AND STRUCTURES AND TYPEDEFS*********************************************/
typedef struct{
	unsigned int noiseSample,musicSample,errMicSample;
}InputMeasure; //Los sampleos siempre van a ser estos 3 o se puede obviar alguno.
#define SAMPLING_FREQ 35e3
#define BLOCKSIZE 1 //Cantidad de muestras que se procesan al mismo tiempo

//De los de abajo se tiene que elegir 1, para determinar como se hacen los cálculos:
#define PROCESSING_W_F32
//#define PROCESSING_W_Q15
//#define PROCESSING_W_Q31

#define ENABLE_FAST_FIR_FOR_Q //Con este define se elige usar filtros fir más rápidos pero con menos precisión para los casos de procesamiento con punto fijo


#define IO_FROM_PC //Define si el input viene por la pc
#ifdef IO_FROM_PC
	#define EST_SEC_TEST //En el caso de que esté definido IO_FROM_PC indica que es para probar la estimación del camino secundario. Sino es para probar lo otro.
#endif

/************************************************FUNCTION PROTOTYPES WITH GLOBAL SCOPE***********************************************/


#endif // COMMONCONFIG_H_

