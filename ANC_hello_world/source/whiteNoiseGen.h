/*****************************************************************************
  @file     boxmuller.h
  @brief    
  @author   Javier
 ******************************************************************************/

#ifndef WHITENOISEGEN_H_
#define WHITENOISEGEN_H_

/*******************************************************INCLUDE HEADER FILES********************************************************/
#include "arm_math.h"
#include "CommonConfig.h"
/*********************************************CONSTANT AND MACRO DEFINITIONS USING #DEFINE******************************************/

/***********************************************ENUMERATIONS AND STRUCTURES AND TYPEDEFS*********************************************/

/************************************************FUNCTION PROTOTYPES WITH GLOBAL SCOPE***********************************************/
void initWNG();
void ranf(float *rNmbs);         //Genera Floats random entre 0 y 1,
void whiteNoiseGen(float32_t *WGN,float32_t s); //Este usa una lookup table, escribe BLOCKSIZE elementos de ruido blanco g
#endif // WHITENOISEGEN_H_

