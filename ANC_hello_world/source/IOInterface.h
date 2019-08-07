/*****************************************************************************
  @file     IOInterface.h
  @brief    
  @author   Javier
 ******************************************************************************/

#ifndef IOINTERFACE_H_
#define IOINTERFACE_H_

/*******************************************************INCLUDE HEADER FILES********************************************************/
#include "arm_math.h"
#include "CommonConfig.h"
/*********************************************CONSTANT AND MACRO DEFINITIONS USING #DEFINE******************************************/

/***********************************************ENUMERATIONS AND STRUCTURES AND TYPEDEFS*********************************************/

/************************************************FUNCTION PROTOTYPES WITH GLOBAL SCOPE***********************************************/
void initIOInterface(); //Inits the driver
int newInputAvailable(); //Checks if there is new input. This layer does the upsampling part
void getInputs(InputMeasure *Inputs);
void pushOutput(unsigned int *outputs); //Pushes to an output queue, then this layer puts it in the DAC at the correct time.
int isProcessingTimeRisky(float32_t processingTime);
#endif // IOINTERFACE_H_

