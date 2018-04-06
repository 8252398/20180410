/*******************************************************************************
 * 版权 2015-2016 中国航空工业集团第六三一所第十八研究室                        
 *                                                                              
 * 对本文件的拷贝、发布、修改或者其他任何用途必须得到中国航空工业集团第六三一所 
 * 的书面协议许可。                                                             
 *                                                                              
 * Copyrights (2015-2016) ACTRI                                                 
 * All Right Reserved                                                           
 *******************************************************************************
 
 *******************************************************************************
 * Revision History                                                             
 *------------------------------------------------------------------------------
 * 2017-12-07, gbo, created                                                     
 *******************************************************************************
 
 *******************************************************************************
 * content                                                                      
 *------------------------------------------------------------------------------
 *  File            : etpuc_set.c                                               
 *  Description     : ETPU函数集                                            
 *  Call            : 029-89186510                                              
 ******************************************************************************/
 
/* Use a 6k memory starting at 0 */
#pragma memory ROM[(10 * (1024))] @ 0x0400;

/* Put entry table at the beginning of memory so that it is not
   dependent on memory size. */
#pragma entryaddr 0x0000;

int24 Global_Error = 0xFF;
unsigned int8 gEngPosSyncState = 0;
unsigned int8 gCamState = 0;
unsigned int24 gCamEdgeAngle = 0;
unsigned int24 gCamCount = 0;

union Link_t {
	int32 Chans;
	int8  Chan[3];
};

#define GLOBAL_ERROR_FUNC
void Global_Error_Func();

#define REGISTER_PASSING

#ifndef REGISTER_PASSING
void Link4( union Link_t Link  );
#else
void Link4( );
#endif

register_p7_0 p7_0;
register_p15_8 p15_8;
register_p23_16 p23_16;
register_p31_24 p31_24;

/*************************/
/* Include the functions */
/*************************/
/* PWM (Pulse Width Modulation) function */
#define PWM_FUNCTION_NUMBER 0
#include "etpuc_pwm.c"

/* FIN */
#define FIN_FUNCTION_NUMBER 1
#include "etpuc_fin.c"

/* Camshaft detect */
#define CAM_FUNCTION_NUMBER 2
#include "etpuc_cam2.c"

#define CRANK_FUNCTION_NUMBER 3
#include "etpuc_crank2.c"

#define TOOTHGEN_FUNCTION_NUMBER 4
#include "etpuc_toothgen2.c"

#define FUEL_FUNCTION_NUMBER 5
#include "etpuc_fuel2.c"

#define RAIL_FUNCTION_NUMBER 6
#include "etpuc_rail.c"

#define FUEL_ENABLE_FUNCTION_NUMBER 7
#include "etpuc_fuelenable.c"

void Global_Error_Func()
{
	int24 error_temp;

	if (LinkServiceRequest == 1) error_temp=1;
	else
		error_temp = 0;

	if (MatchALatch == 1) error_temp++;
	error_temp <<= 1;

	if (MatchBLatch == 1) error_temp++;
	error_temp <<= 1;

	if (TransitionALatch == 1) error_temp++;
	error_temp <<= 1;

	if (TransitionBLatch == 1) error_temp++;
	error_temp <<= 8;

	Global_Error = error_temp + chan;

	ClearAllLatches();
}

#ifndef REGISTER_PASSING
void Link4(union Link_t Link ){

	link = Link.Chan[0];
	link = Link.Chan[1];
	link = Link.Chan[2];
	link = Link.Chan[3];
}
#else
void Link4( ){

	link = p7_0;
	link = p15_8;
	link = p23_16;
	link = p31_24;
}
#endif

/* output eTPU code image and information for CPU */
#pragma write h, (::ETPUfilename (etpu_set.h));
#pragma write h, (/*******************************************************************************);
#pragma write h, ( * 版权 2015-2016 中国航空工业集团第六三一所第十八研究室                        );
#pragma write h, ( *                                                                              );
#pragma write h, ( * 对本文件的拷贝、发布、修改或者其他任何用途必须得到中国航空工业集团第六三一所 );
#pragma write h, ( * 的书面协议许可。                                                             );
#pragma write h, ( *                                                                              );
#pragma write h, ( * Copyrights (2015-2016) ACTRI                                                 );
#pragma write h, ( * All Right Reserved                                                           );
#pragma write h, ( *******************************************************************************);
#pragma write h, (                                                                                );
#pragma write h, ( *******************************************************************************);
#pragma write h, ( * Revision History                                                             );
#pragma write h, ( *------------------------------------------------------------------------------);
#pragma write h, ( * 2017-12-07, gbo, created                                                     );
#pragma write h, ( *******************************************************************************);
#pragma write h, ( );
#pragma write h, ( *******************************************************************************);
#pragma write h, ( * content                                                                      );
#pragma write h, ( *------------------------------------------------------------------------------);
#pragma write h, ( *  File            : etpuc_set.h                                               );
#pragma write h, ( *  Description     : 函数集头文件                                              );
#pragma write h, ( *  Call            : 029-89186510                                              );
#pragma write h, ( *  该文件由ETPU自动生成，请勿修改！！！                                        );
#pragma write h, ( ******************************************************************************/);
#pragma write h, (#ifndef _ETPU_SET_H_ );
#pragma write h, (#define _ETPU_SET_H_ );
#pragma write h, ( );
#pragma write h, (/* eTPU standard function set1 */ );
#pragma write h, (#define FS_ETPU_ENTRY_TABLE 0x0000);
#pragma write h, ( );
#pragma write h, (#define FS_ETPU_MISC ::ETPUmisc);
#pragma write h, ( );
#pragma write h, (const uint32_t etpu_globals[] = { ::ETPUglobalimage }; );
#pragma write h, ();
#pragma write h, (const uint32_t etpu_code[] = { ::ETPUcode32 }; );
#pragma write h, (/* List of functions: );
#pragma write h, (::ETPUnames);
#pragma write h, (*/);
#pragma write h, ( );
#pragma write h, (#endif /* _ETPU_SET_H_ */ );
#pragma write h, ( );
