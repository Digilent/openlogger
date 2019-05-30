/************************************************************************/
/*                                                                      */
/*    DDR.c                                                             */
/*                                                                      */
/*    Initializes the DDR memory as part of the CRT _on_reset()         */
/*                                                                      */
/*    This code was modified from the Harmony DDR Init code             */
/*    Originated by Microchip                                           */
/*                                                                      */
/************************************************************************/
/*    Author:     Keith Vogel                                           */
/*    Copyright 2016, Digilent Inc.                                     */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*    6/12/2018 (KeithV): Created                                       */
/************************************************************************/
#include <PinAssignments.h>

extern void InitDDR(void);
extern void InitSFRs(void);

void _on_reset(void)
{
    InitDDR();
    InitSFRs();
}  
