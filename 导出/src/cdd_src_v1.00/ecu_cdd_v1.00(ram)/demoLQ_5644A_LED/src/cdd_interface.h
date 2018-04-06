/*******************************************************************************
 * 版权 2015-2018 中国航空工业集团第六三一所第十八研究室
 *
 * 对本文件的拷贝、发布、修改或者其他任何用途必须得到中国航空工业集团第六三一所
 * 的书面协议许可。
 *
 * Copyrights (2015-2018) ACTRI
 * All Right Reserved
 *******************************************************************************

 *******************************************************************************
 * Revision History
 *------------------------------------------------------------------------------
 * 
 *******************************************************************************

 *******************************************************************************
 * content
 *------------------------------------------------------------------------------
 *  File            : cdd_interface.h
 *  Description     : 复杂驱动接口函数头文件
 *  Call            : 029-89186510
 ******************************************************************************/

#ifndef CDD_INTERFACE_H_
#define CDD_INTERFACE_H_
#ifdef __cplusplus
extern "C" {
#endif


/*macro definition of pwm output init interface*/
#define PWM_INITINDEX_IMV 1    /*pwm output channel for imv, a26 of etpu*/
#define PWM_INITINDEX_UTHCV 2    /*pwm output channel for UTHCV, a25 of etpu*/
#define PWM_INITINDEX_FAN 3    /*pwm output channel for fan, a28 of etpu*/

#define PWM_INIT_RET_OK 0    /*result of pwm output init interface, ok*/
#define PWM_INIT_RET_ERR_INDEX 1    /*result of pwm output init interface, channel index err*/
#define PWM_INIT_RET_ERR_CYCLE 2    /*result of pwm output init interface, output cycle err*/
#define PWM_INIT_RET_ERR_DUTY 3    /*result of pwm output init interface, output duty err*/

#define PWM_INIT_CYCLE_MIN 2.5
#define PWM_INIT_CYCLE_MAX 62.5
#define PWM_INIT_DUTY_MIN 0
#define PWM_INIT_DUTY_MAX 100
#define PWM_INIT_CYCLE_SAFE 62.5
#define PWM_INIT_DUTY_SAFE 95

/*macro definition of pwm output update interface*/
#define PWM_UPDATEINDEX_IMV 1    /*pwm output channel for imv, a26 of etpu*/
#define PWM_UPDATEINDEX_UTHCV 2    /*pwm output channel for UTHCV, a25 of etpu*/
#define PWM_UPDATEINDEX_FAN 3    /*pwm output channel for fan, a28 of etpu*/

#define PWM_UPDATEMODE_IMME 1    /*update mode: imediately*/
#define PWM_UPDATEMODE_NEXT 2    /*update mode: next*/

#define PWM_UPDATE_RET_OK 0    /*result of pwm output update interface, ok*/
#define PWM_UPDATE_RET_ERR_INDEX 1    /*result of pwm output update interface, channel index err*/
#define PWM_UPDATE_RET_ERR_CYCLE 2    /*result of pwm output update interface, output cycle err*/
#define PWM_UPDATE_RET_ERR_DUTY 3    /*result of pwm output update interface, output duty err*/
#define PWM_UPDATE_RET_ERR_MODE 4    /*result of pwm output update interface, update mode err*/

#define PWM_UPDATE_CYCLE_MIN 2.5
#define PWM_UPDATE_CYCLE_MAX 62.5
#define PWM_UPDATE_DUTY_MIN 0
#define PWM_UPDATE_DUTY_MAX 100
#define PWM_UPDATE_CYCLE_SAFE 62.5
#define PWM_UPDATE_DUTY_SAFE 95

/*macro definition of engine speed output init interface*/
#define ENG_INIT_RET_OK 0   /*result of engine speed output init interface, ok*/
#define ENG_INIT_RET_ERR_SPD 1  /*result of engine speed output init interface, engine speed err*/

#define ENG_INIT_SPD_MIN 50
#define ENG_INIT_SPD_MAX 4000
#define ENG_INIT_SPD_SAFE 50

/*macro definition of engine speed output update interface*/
#define ENG_UPDATE_RET_OK  0/*result of engine speed output update interface, ok*/
#define ENG_UPDATE_RET_ERR_SPD 1  /*result of engine speed output update interface, engine speed err*/

#define ENG_UPDATE_SPD_MIN 50
#define ENG_UPDATE_SPD_MAX 4000
#define ENG_UPDATE_SPD_SAFE 50

/*macro definition of freq input interface*/
#define FRQ_INDEX_FAN 1    /*freq input channel for fan, A19 of etpu*/
#define FRQ_INDEX_CARSPD 2    /*freq inputchannel for car speed, A13 of etpu*/
#define FRQ_INDEX_ENGSPD 3    /*freq input channel for engine speed, A21 of etpu*/

#define FRQ_SAFE 0

/*macro definition of fuel inject interface*/
/*array index of parameter*/
#define INJ_PRE_2_ANGLE          0   /*angle of pre_inject 2*/
#define INJ_PRE_2_DURATION   1    /*time duration of pre_inject 2*/
#define INJ_PRE_1_ANGLE         2   /*angle of pre_inject 1*/
#define INJ_PRE_1_DURATION   3    /*time duration of pre_inject 1*/
#define INJ_MAIN_ANGLE           4    /*angle of main_inject*/
#define INJ_MAIN_DURATION     5    /*time duration of main_inject*/
#define INJ_RE_1_ANGLE            6    /*angle of re_inject 1*/
#define INJ_RE_1_DURATION      7   /*time duration of re_inject 1*/
#define INJ_RE_2_ANGLE            8    /*angle of re_inject 2*/
#define INJ_RE_2_DURATION      9   /*time duration of re_inject 2*/
/*index of inject unit*/
#define INJ_INDEX_1 1   /*inject unit #1*/
#define INJ_INDEX_2 2   /*inject unit #2*/
#define INJ_INDEX_3 3   /*inject unit #3*/
#define INJ_INDEX_4 4   /*inject unit #4*/
#define INJ_INDEX_5 5   /*inject unit #5*/
#define INJ_INDEX_6 6   /*inject unit #6*/

/*status of senser*/
#define EPS_SEN_STA_CRANK 0
#define EPS_SEN_STA_CAM 1
#define EPS_SEN_STA_ERR 2
/*status of sync_bit*/
#define EPS_OK 0
#define EPS_ERR_CRANK_NOPULSE 1
#define EPS_ERR_CAM_NOPULSE 2
#define EPS_ERR_CRANK_NOSYNC 3
#define EPS_ERR_CAM_NOSYNC 4
#define EPS_ERR_BOTH 5

extern uint32_t etpu_a_tcr1_freq;
extern uint32_t PWM_m_i_InitIndex;
extern float PWM_m_ti_InitCycle;
extern uint32_t PWM_m_pc_InitDuty;
extern uint32_t PWM_m_i_UpdateIndex;
extern float PWM_m_ti_UpdateCycle;
extern uint32_t PWM_m_pc_UpdateDuty;
extern uint32_t PWM_m_pc_UpdateMode;
extern uint32_t ENG_m_n_InitSpd;
extern uint32_t ENG_m_n_UpdateSpd;
extern uint8_t FRQ_m_i_Index;
extern uint32_t FRQ_m_u_Result;
extern uint8_t INJ_m_i_Index;
extern float INJ_m_phi_Parameter[10];
extern uint8_t EPS_m_st_CrkSync;
extern uint8_t EPS_m_st_CamSync;
extern uint32_t EPS_m_n_Crank;
extern uint32_t EPS_m_n_Cam;
extern uint32_t EPS_m_n_CrankAvg;
extern uint32_t EPS_m_n_CamAvg;
extern uint32_t EPS_m_n_Engine;
extern uint32_t EPS_m_n_EngineAvg;
extern uint32_t EPS_m_st_Status;
extern uint32_t EPS_m_st_SenSta;
extern uint16_t EPS_m_st_Error;

void ETPU_PWM_Init_exception(void);
int32_t ETPU_PWM_Init(void);
int32_t ETPU_PWM_Update(void);
int32_t ETPU_SPD_Init(void);
void ETPU_SPD_Init_exception(void);
int32_t ETPU_SPD_Update(void);
void ETPU_FRQ_Get (void);
void ETPU_Fuel_Set (void);
void EPS_GetCrankSta(void);
void EPS_GetCamSta(void);
void EPS_GetCrankSpd(void);
void EPS_GetCamSpd(void);
void EPS_GetCrankAndCamAvgSpd(void);
void EPS_GetEngSpdAndAvgSpd(void);
void EPS_GetEngSta(void);
void EPS_GetSenSta(void);


#ifdef __cplusplus
}
#endif
#endif  /* end cdd_interface.h*/

