#include <eTPUc_common.h>
#include <etpuc.h>

#ifdef RAIL_FUNCTION_NUMBER
#pragma ETPU_function RailPress, alternate @ RAIL_FUNCTION_NUMBER;
#else
#pragma ETPU_function RailPress, alternate;
#endif

#define RAIL_INIT 7

void RailPress(void)
{
    if (hsr == RAIL_INIT)
    {
        DisableMatchesInThread();
        OnTransA(NoDetect);
        OnTransB(NoDetect);
        ClearLSRLatch();
        ClearTransLatch();
        ClearMatchAEvent();
        ClearMatchBEvent();
        
        SetChannelMode(sm_st);
        EnableOutputBuffer();
        SetPinLow();
        EnableEventHandling();
    }
    else if (lsr == 1)
    {
        ClearLSRLatch();
        SetChannelInterrupt();
    }
    else
    {
#ifdef GLOBAL_ERROR_FUNC
        Global_Error_Func();
#else
        ClearAllLatches();
#endif
    }
}

/* Information exported to Host CPU program */
#pragma write h, (::ETPUfilename (etpu_rail_auto.h));
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
#pragma write h, ( *  File            : etpuc_rail_auto.h                                          );
#pragma write h, ( *  Description     : 轨压中断头文件                                      );
#pragma write h, ( *  Call            : 029-89186510                                              );
#pragma write h, ( *  该文件由ETPU自动生成，请勿修改！！！                                        );
#pragma write h, ( ******************************************************************************/);
#pragma write h, (#ifndef _ETPU_RAIL_AUTO_H_ );
#pragma write h, (#define _ETPU_RAIL_AUTO_H_ );
#pragma write h, ( );
#pragma write h, (/* Function Configuration Information */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_RAIL_FUNCTION_NUMBER) RAIL_FUNCTION_NUMBER );
#pragma write h, (::ETPUliteral(#define FS_ETPU_RAIL_TABLE_SELECT) ::ETPUentrytype(RailPress) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_RAIL_NUM_PARMS) ::ETPUram(RailPress) );
#pragma write h, ( );
#pragma write h, (/* Host Service Request Definitions */);
#pragma write h, (/* 喷油初始化 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_RAIL_INIT) RAIL_INIT );
#pragma write h, (#endif /* _ETPU_PWM_AUTO_H_ */);
#pragma write h, ( );