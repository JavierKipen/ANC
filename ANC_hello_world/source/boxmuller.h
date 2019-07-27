/*****************************************************************************
  @file     boxmuller.h
  @brief    
  @author   Javier
 ******************************************************************************/

#ifndef BOXMULLER_H_
#define BOXMULLER_H_

/*******************************************************INCLUDE HEADER FILES********************************************************/
#include "arm_math.h"
/*********************************************CONSTANT AND MACRO DEFINITIONS USING #DEFINE******************************************/

/***********************************************ENUMERATIONS AND STRUCTURES AND TYPEDEFS*********************************************/

/************************************************FUNCTION PROTOTYPES WITH GLOBAL SCOPE***********************************************/
float ranf();         /* ranf() is uniform in 0..1 */
float box_muller(float m, float s);
float32_t whiteNoiseGen(float32_t s);
#endif // BOXMULLER_H_

