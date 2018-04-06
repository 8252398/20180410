/********************************************************************************/
/* FILE NAME: eTPU_util.h                           COPYRIGHT (c) MOTOROLA 2003 */
/* VERSION:  1.0                                    All Rights Reserved         */
/*                                                                              */
/* This file is maintained as part of the support for the eTPU C development    */
/* tools by Byte Craft Limited.                                                 */
/*                                                                              */
/* This code may be adapted for any purpose when used with the                  */
/* eTPU C Code Development System.  No warranty is implied or                   */
/* given as to their usability for any purpose. Portions of this                */
/* file is Copyright Motorola.                                                  */
/*                                                                              */
/*	(c) Copyright 2002-2004 Byte Craft Limited                                */
/*	421 King St.N., Waterloo, ON, Canada, N2J 4E4                             */
/*	VOICE: 1 (519) 888 6911                                                   */
/*	email: support@bytecraft.com                                              */
/*                                                                              */
/*	Walter Banks                                                              */
/* 	                                                                          */
/*                                                                              */
/* DESCRIPTION: This contains common utility functions or macros                */
/*==============================================================================*/
/* UPDATE HISTORY                                                               */
/* REV      AUTHOR       DATE        DESCRIPTION OF CHANGE                      */
/* ---      -------      --------    ---------------------                      */
/* 0.0      Ming Li      11/02/03    Initial creation                           */
/* 0.2      Walter Banks 19 Dec 03   Added Documentation on System requirements */
/* 0.3      Walter Banks  5 Jan 04   Added () around arguments to protect against
                                     two operators in a row  ie a-b if b is 
                                     defined as -5 would expand to a--b now is
                                     (a)-(-5)                                   */ 
/* 0.4      Walter Banks 23 Oct 04   Added  _coherentread 
                                     Added  _coherentwrite                      */
/* 0.5      Walter Banks 27 Oct 04   Fixed  _coherentread _coherentwrite 
                                            added NOP() to break possibility of
                                            wrong pairing                       */


/********************************************************************************/

#ifndef __ETPU_UTIL_H
#define __ETPU_UTIL_H 

/*--------------------------------------------------------------------+
|                    Required Application constants                   |
|      System Constant               Typical Value                    |
|---------------------------------------------------------------------|
|     TicksPerTooth                      100                          |
|     TeethPerCycle                       72                          | 
+--------------------------------------------------------------------*/


/*--------------------------------------------------------------------+
|                           Include Header Files                      |
+--------------------------------------------------------------------*/

/*--------------------------------------------------------------------+
|                           Macro Definition                          |
+--------------------------------------------------------------------*/
#define T_Modulus TicksPerTooth*TeethPerCycle
#define Add_Angle(b,c) ((((b)+(c))>T_Modulus)? ((b)+(c))-T_Modulus: ((b)+(c)))
#define Subtract_Angle(b,c) ((((b)-(c))<0)? ((b)-(c))+T_Modulus: ((b)-(c)))

#define _coherentread(a_source,a_dest,b_source,b_dest) \
   {register_diob diob; NOP(); diob = a_source; register_p p; \
    p = b_source;  b_dest = p; a_dest = diob;};

#define _coherentwrite(a_dest,a_source,b_dest,b_source)\
   {register_diob diob; diob = a_source; register_p p; \
    p = b_source; NOP(); a_dest = diob; b_dest = p; };


/*--------------------------------------------------------------------+
|                           Constants Definition                      |
+--------------------------------------------------------------------*/


#endif /* __ETPU_UTIL_H */

/*********************************************************************
 *
 * Copyright:
 *	MOTOROLA, INC. All Rights Reserved.
 *  You are hereby granted a copyright license to use, modify, and
 *  distribute the SOFTWARE so long as this entire notice is
 *  retained without alteration in any modified and/or redistributed
 *  versions, and that such modified versions are clearly identified
 *  as such. No licenses are granted by implication, estoppel or
 *  otherwise under any patents or trademarks of Motorola, Inc. This
 *  software is provided on an "AS IS" basis and without warranty.
 *
 *  To the maximum extent permitted by applicable law, MOTOROLA
 *  DISCLAIMS ALL WARRANTIES WHETHER EXPRESS OR IMPLIED, INCLUDING
 *  IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR
 *  PURPOSE AND ANY WARRANTY AGAINST INFRINGEMENT WITH REGARD TO THE
 *  SOFTWARE (INCLUDING ANY MODIFIED VERSIONS THEREOF) AND ANY
 *  ACCOMPANYING WRITTEN MATERIALS.
 *
 *  To the maximum extent permitted by applicable law, IN NO EVENT
 *  SHALL MOTOROLA BE LIABLE FOR ANY DAMAGES WHATSOEVER (INCLUDING
 *  WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS
 *  INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY
 *  LOSS) ARISING OF THE USE OR INABILITY TO USE THE SOFTWARE.
 *
 *  Motorola assumes no responsibility for the maintenance and support
 *  of this software
 ********************************************************************/

