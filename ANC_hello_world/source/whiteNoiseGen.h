/*****************************************************************************
  @file     boxmuller.h
  @brief    
  @author   Javier
 ******************************************************************************/

#ifndef WHITENOISEGEN_H_
#define WHITENOISEGEN_H_

/*******************************************************INCLUDE HEADER FILES********************************************************/
#include "arm_math.h"
/*********************************************CONSTANT AND MACRO DEFINITIONS USING #DEFINE******************************************/

/***********************************************ENUMERATIONS AND STRUCTURES AND TYPEDEFS*********************************************/

/************************************************FUNCTION PROTOTYPES WITH GLOBAL SCOPE***********************************************/
void initWNG();
float ranf();         //Float random entre 0 y 1
float box_muller(float m, float s); //Este era un método para generar WGN pero tardaba en computar
float32_t whiteNoiseGen(float32_t s); //Este usa una lookup table.
#endif // WHITENOISEGEN_H_

