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
 *  File            : cdd_interface.c
 *  Description     : 复杂驱动接口函数
 *  Call            : 029-89186510
 ******************************************************************************/
/******************************************************************************
 * Include files
 ******************************************************************************/
#include "includes.h"
#include "etpu_start.h"
#include "eTPUChCfg.h"
#include "etpu_eng_pos_auto.h"
#include "cdd_interface.h"




/*parameters for pwm & engine speed output interface*/
uint32_t PWM_m_i_InitIndex = PWM_INITINDEX_IMV;    /*channel index of pwm output init interface*/
float PWM_m_ti_InitCycle = PWM_INIT_CYCLE_SAFE;    /*cycle of pwm output init interface*/
uint32_t PWM_m_pc_InitDuty = PWM_INIT_DUTY_SAFE;    /*dyty of pwm output init interface*/

uint32_t PWM_m_i_UpdateIndex = PWM_UPDATEINDEX_IMV;    /*channel index of pwm output update interface*/
float PWM_m_ti_UpdateCycle = PWM_UPDATE_CYCLE_SAFE;    /*cycle of pwm output update interface*/
uint32_t PWM_m_pc_UpdateDuty = PWM_UPDATE_DUTY_SAFE;    /*dyty of pwm output update interface*/
uint32_t PWM_m_pc_UpdateMode = PWM_UPDATEMODE_NEXT;    /*update mode of pwm output update interface*/

uint32_t ENG_m_n_InitSpd = ENG_INIT_SPD_SAFE;
uint32_t ENG_m_n_UpdateSpd = ENG_UPDATE_SPD_SAFE;

/*parameters for freq input interface*/
uint8_t FRQ_m_i_Index = FRQ_INDEX_FAN;
uint32_t FRQ_m_u_Result = FRQ_SAFE;

/*fuel inject parameter*/
uint8_t INJ_m_i_Index = INJ_INDEX_1;
float INJ_m_phi_Parameter[10] = {0};

/*crank & cam sync interface*/
/*status of crank, see etpu_crank_auto.h for more information*/
uint8_t EPS_m_st_CrkSync = FS_ETPU_CRANK_SEEK;
/*status of cam, see etpu_cam_auto.h for more information*/
uint8_t EPS_m_st_CamSync = FS_ETPU_CAM_FIRST_EDGE;
/*speed of crank*/
uint32_t EPS_m_n_Crank = 0;
/*speed of cam*/
uint32_t EPS_m_n_Cam = 0;
/*average speed of crank*/
uint32_t EPS_m_n_CrankAvg = 0;
/*average speed of cam*/
uint32_t EPS_m_n_CamAvg = 0;
/*speed of engine*/
uint32_t EPS_m_n_Engine = 0;
/*average speed of engine*/
uint32_t EPS_m_n_EngineAvg = 0;
/*status of engine*/
uint32_t EPS_m_st_Status = FS_ETPU_ENG_POS_SEEK;
/*status of senser*/
uint32_t EPS_m_st_SenSta = EPS_SEN_STA_CRANK;
/*status of sync_bit*/
uint16_t EPS_m_st_Error = EPS_OK;


/*interface of pwm output init*/
/*IRS_ECU_CDD_PWM.1*/
int32_t ETPU_PWM_Init(void){
    int32_t err_code = 0;
    
    if((PWM_m_i_InitIndex != PWM_INITINDEX_IMV)&&
        (PWM_m_i_InitIndex != PWM_INITINDEX_UTHCV)&&
        (PWM_m_i_InitIndex != PWM_INITINDEX_FAN)){
            ETPU_PWM_Init_exception();
            return PWM_INIT_RET_ERR_INDEX;
        }

    if((PWM_m_ti_InitCycle < PWM_INIT_CYCLE_MIN)||
        (PWM_m_ti_InitCycle > PWM_INIT_CYCLE_MAX)){
            ETPU_PWM_Init_exception();
            return PWM_INIT_RET_ERR_CYCLE;
    }

    if((PWM_m_pc_InitDuty < PWM_INIT_DUTY_MIN)||
        (PWM_m_pc_InitDuty > PWM_INIT_DUTY_MAX)){
            ETPU_PWM_Init_exception();
            return PWM_INIT_RET_ERR_DUTY;
    }

    switch(PWM_m_i_InitIndex){
        case(PWM_INITINDEX_IMV):
            err_code = EtpuPwmInit(ETPU_APP_CHAN_PUMP_EXE,    /*A26*/
                                                    FS_ETPU_PRIORITY_LOW,
                                                    (uint32_t)(1/(PWM_m_ti_InitCycle/1000)),    /*freq, Hz*/
                                                    (uint16_t)(PWM_m_pc_InitDuty*100),    /*duty, %*/
                                                    FS_ETPU_PWM_ACTIVEHIGH, 
                                                    FS_ETPU_TCR1,
                                                    etpu_a_tcr1_freq);
            break;
        case(PWM_INITINDEX_UTHCV):            
            err_code = EtpuPwmInit(ETPU_APP_CHAN_VALVE,    /*A25*/
                                                    FS_ETPU_PRIORITY_LOW,
                                                    (uint32_t)(1/(PWM_m_ti_InitCycle/1000)),    /*freq, Hz*/
                                                    (uint16_t)(PWM_m_pc_InitDuty*100),    /*duty, %*/
                                                    FS_ETPU_PWM_ACTIVEHIGH, 
                                                    FS_ETPU_TCR1,
                                                    etpu_a_tcr1_freq);
            break;
        case(PWM_INITINDEX_FAN):            
            err_code = EtpuPwmInit(ETPU_APP_CHAN_FAN_CLUTH,    /*A28*/
                                                    FS_ETPU_PRIORITY_LOW,
                                                    (uint32_t)(1/(PWM_m_ti_InitCycle/1000)),    /*freq, Hz*/
                                                    (uint16_t)(PWM_m_pc_InitDuty*100),    /*duty, %*/
                                                    FS_ETPU_PWM_ACTIVEHIGH, 
                                                    FS_ETPU_TCR1,
                                                    etpu_a_tcr1_freq);
            break;
        default:
            break;
    }
    return err_code;
}

void ETPU_PWM_Init_exception(void){
    int32_t err_code_A25 = 0;
    int32_t err_code_A26 = 0;
    int32_t err_code_A28 = 0;
    
    err_code_A25 = EtpuPwmInit(ETPU_APP_CHAN_VALVE,    /*A25*/
                                                    FS_ETPU_PRIORITY_LOW,
                                                    (uint32_t)(1/(PWM_INIT_CYCLE_SAFE/1000)),    /*freq, Hz*/
                                                    (uint16_t)(PWM_INIT_DUTY_SAFE*100),    /*duty, %*/
                                                     FS_ETPU_PWM_ACTIVEHIGH, 
                                                     FS_ETPU_TCR1,
                                                     etpu_a_tcr1_freq);
    err_code_A26 = EtpuPwmInit(ETPU_APP_CHAN_PUMP_EXE,    /*A26*/
                                                     FS_ETPU_PRIORITY_LOW,
                                                     (uint32_t)(1/(PWM_INIT_CYCLE_SAFE/1000)),    /*freq, Hz*/
                                                     (uint16_t)(PWM_INIT_DUTY_SAFE*100),    /*duty, %*/
                                                     FS_ETPU_PWM_ACTIVEHIGH, 
                                                     FS_ETPU_TCR1,
                                                     etpu_a_tcr1_freq);
    err_code_A28 = EtpuPwmInit(ETPU_APP_CHAN_FAN_CLUTH,    /*A28*/
                                                     FS_ETPU_PRIORITY_LOW,
                                                     (uint32_t)(1/(PWM_INIT_CYCLE_SAFE/1000)),    /*freq, Hz*/
                                                     (uint16_t)(PWM_INIT_DUTY_SAFE*100),    /*duty, %*/
                                                     FS_ETPU_PWM_ACTIVEHIGH, 
                                                     FS_ETPU_TCR1,
                                                     etpu_a_tcr1_freq);
}

/*interface of pwm output update*/
/*IRS_ECU_CDD_PWM.2*/
int32_t ETPU_PWM_Update(void){
    int32_t err_code = 0;
        
    if((PWM_m_i_UpdateIndex != PWM_UPDATEINDEX_IMV)&&
      (PWM_m_i_UpdateIndex != PWM_UPDATEINDEX_UTHCV)&&
      (PWM_m_i_UpdateIndex != PWM_UPDATEINDEX_FAN)){
        return PWM_UPDATE_RET_ERR_INDEX;
    }
    
    if((PWM_m_ti_UpdateCycle < PWM_UPDATE_CYCLE_MIN)||
      (PWM_m_ti_UpdateCycle > PWM_UPDATE_CYCLE_MAX)){
        return PWM_UPDATE_RET_ERR_CYCLE;
    }
    
    if((PWM_m_pc_UpdateDuty < PWM_UPDATE_DUTY_MIN)||
      (PWM_m_pc_UpdateDuty > PWM_UPDATE_DUTY_MAX)){
        return PWM_UPDATE_RET_ERR_DUTY;
    }
   
    if(PWM_m_pc_UpdateMode == PWM_UPDATEMODE_IMME){    
/*update imediately*/
        switch(PWM_m_i_UpdateIndex){
            case(PWM_UPDATEINDEX_IMV):
                err_code = EtpuPwmUpdateImmed(ETPU_APP_CHAN_PUMP_EXE,    /*A26*/
                                                                        (uint32_t)(1/(PWM_m_ti_UpdateCycle/1000)),    /*freq, Hz*/
                                                                        (uint16_t)(PWM_m_pc_UpdateDuty*100),    /*duty, %*/
                                                                        etpu_a_tcr1_freq);
                break;
            case(PWM_UPDATEINDEX_UTHCV):            
                err_code = EtpuPwmUpdateImmed(ETPU_APP_CHAN_VALVE,    /*A25*/
                                                                        (uint32_t)(1/(PWM_m_ti_UpdateCycle/1000)),    /*freq, Hz*/
                                                                        (uint16_t)(PWM_m_pc_UpdateDuty*100),    /*duty, %*/
                                                                        etpu_a_tcr1_freq);
                break;
            case(PWM_UPDATEINDEX_FAN):            
                err_code = EtpuPwmUpdateImmed(ETPU_APP_CHAN_FAN_CLUTH,    /*A28*/
                                                                        (uint32_t)(1/(PWM_m_ti_UpdateCycle/1000)),    /*freq, Hz*/
                                                                        (uint16_t)(PWM_m_pc_UpdateDuty*100),    /*duty, %*/
                                                                        etpu_a_tcr1_freq);

                break;
            default:
                break;
        }
    }else if(PWM_m_pc_UpdateMode == PWM_UPDATEMODE_NEXT){
/*update next cycle*/
        switch(PWM_m_i_UpdateIndex){
            case(PWM_UPDATEINDEX_IMV):
                err_code = EtpuPwmUpdateCohere(ETPU_APP_CHAN_PUMP_EXE,    /*A26*/
                                                                                (uint32_t)(1/(PWM_m_ti_UpdateCycle/1000)),    /*freq, Hz*/
                                                                                (uint16_t)(PWM_m_pc_UpdateDuty*100),    /*duty, %*/
                                                                                etpu_a_tcr1_freq);
                break;
            case(PWM_UPDATEINDEX_UTHCV):            
                err_code = EtpuPwmUpdateCohere(ETPU_APP_CHAN_VALVE,    /*A25*/
                                                                                (uint32_t)(1/(PWM_m_ti_UpdateCycle/1000)),    /*freq, Hz*/
                                                                                (uint16_t)(PWM_m_pc_UpdateDuty*100),    /*duty, %*/
                                                                                etpu_a_tcr1_freq);
                break;
            case(PWM_UPDATEINDEX_FAN):            
                err_code = EtpuPwmUpdateCohere(ETPU_APP_CHAN_FAN_CLUTH,    /*A28*/
                                                                                (uint32_t)(1/(PWM_m_ti_UpdateCycle/1000)),    /*freq, Hz*/
                                                                                (uint16_t)(PWM_m_pc_UpdateDuty*100),    /*duty, %*/
                                                                                etpu_a_tcr1_freq);       
                break;
            default:
                break;
        }
    }else{
        return PWM_UPDATE_RET_ERR_MODE;
    }
    return err_code;
}

/*interface of engine speed output init*/
/*IRS_ECU_CDD_RS.1*/
int32_t ETPU_SPD_Init(void){
    int32_t err_code = 0;
       
    if((ENG_m_n_InitSpd > ENG_INIT_SPD_MAX)||
    (ENG_m_n_InitSpd < ENG_INIT_SPD_MIN)){
        ETPU_SPD_Init_exception();
        return ENG_INIT_RET_ERR_SPD;
    }

    err_code = EtpuPwmInit(ETPU_APP_CHAN_ENG_SPEED,    /*A2*/
                                            FS_ETPU_PRIORITY_LOW,
                                            ENG_m_n_InitSpd,    /*freq, Hz*/
                                            5000,    /*duty, %*/
                                            FS_ETPU_PWM_ACTIVEHIGH, 
                                            FS_ETPU_TCR1,
                                            etpu_a_tcr1_freq);
    return err_code; 
}

void ETPU_SPD_Init_exception(void){
    int32_t err_code = 0;
    
    err_code = EtpuPwmInit(ETPU_APP_CHAN_ENG_SPEED,    /*A2*/
                                            FS_ETPU_PRIORITY_LOW,
                                            ENG_INIT_SPD_SAFE,    /*freq, Hz*/
                                            5000,    /*duty, %*/
                                            FS_ETPU_PWM_ACTIVEHIGH, 
                                            FS_ETPU_TCR1,
                                            etpu_a_tcr1_freq);
}

/*interface of engine speed output update*/
/*IRS_ECU_CDD_RS.2*/
int32_t ETPU_SPD_Update(void){
    int32_t err_code = 0;
       
    if((ENG_m_n_UpdateSpd > ENG_UPDATE_SPD_MAX)||
    (ENG_m_n_UpdateSpd < ENG_UPDATE_SPD_MIN)){
        return ENG_UPDATE_RET_ERR_SPD;
    }

    err_code = EtpuPwmUpdateImmed(ETPU_APP_CHAN_ENG_SPEED,    /*A2*/
                                                              ENG_m_n_UpdateSpd,    /*freq, Hz*/
                                                              5000,    /*duty, %*/
                                                              etpu_a_tcr1_freq);
    return err_code; 
}

/*interface of freq input*/
/*IRS_ECU_CDD_FREQ.1*/
void ETPU_FRQ_Get(void){
    if((FRQ_m_i_Index != FRQ_INDEX_FAN)&&
        (FRQ_m_i_Index != FRQ_INDEX_CARSPD)&&
        (FRQ_m_i_Index != FRQ_INDEX_ENGSPD)){
            FRQ_m_u_Result = FRQ_SAFE;
    }

    switch(FRQ_m_i_Index){
        case(FRQ_INDEX_FAN):
            FRQ_m_u_Result = EtpuFinGetResult(ETPU_APP_CHAN_FAN_SPEED,etpu_a_tcr1_freq);
            break;
        case(FRQ_INDEX_CARSPD):
            FRQ_m_u_Result = EtpuFinGetResult(ETPU_APP_CHAN_MEG_SPEED,etpu_a_tcr1_freq);
            break;
        case(FRQ_INDEX_FAN):
            FRQ_m_u_Result = EtpuFinGetResult(ETPU_APP_CHAN_RECORDER,etpu_a_tcr1_freq);
            break;
        default:
            break;
    }
}

/*interface of fuel inject*/
/*IRS_ECU_CDD_INJ.1*/
void ETPU_Fuel_Set (void){
    uint32_t para[10] = {0};
    uint8_t index[6] = {ETPU_APP_CHAN_FUEL1,ETPU_APP_CHAN_FUEL2,
                                    ETPU_APP_CHAN_FUEL3,ETPU_APP_CHAN_FUEL4,
                                    ETPU_APP_CHAN_FUEL5,ETPU_APP_CHAN_FUEL6};

    if((INJ_m_i_Index < INJ_INDEX_1)||(INJ_m_i_Index > INJ_INDEX_6)){
        return;
    }

    para[INJ_PRE_2_ANGLE] = DEG2TCR2(INJ_m_phi_Parameter[INJ_PRE_2_ANGLE]/64);
    para[INJ_PRE_2_DURATION] = USEC2TCR1(INJ_m_phi_Parameter[INJ_PRE_2_DURATION]);
    para[INJ_PRE_1_ANGLE] = DEG2TCR2(INJ_m_phi_Parameter[INJ_PRE_1_ANGLE]/64);
    para[INJ_PRE_1_DURATION] = USEC2TCR1(INJ_m_phi_Parameter[INJ_PRE_1_DURATION]);
    para[INJ_MAIN_ANGLE] = DEG2TCR2(INJ_m_phi_Parameter[INJ_MAIN_ANGLE]/64);
    para[INJ_MAIN_DURATION] = USEC2TCR1(INJ_m_phi_Parameter[INJ_MAIN_DURATION]);
    para[INJ_RE_1_ANGLE] = DEG2TCR2(INJ_m_phi_Parameter[INJ_RE_1_ANGLE]/64);
    para[INJ_RE_1_DURATION] = USEC2TCR1(INJ_m_phi_Parameter[INJ_RE_1_DURATION]);
    para[INJ_RE_2_ANGLE] = DEG2TCR2(INJ_m_phi_Parameter[INJ_RE_2_ANGLE]/64);
    para[INJ_RE_2_DURATION] = USEC2TCR1(INJ_m_phi_Parameter[INJ_RE_2_DURATION]);

    EtpuFuelSetInjTime(index[INJ_m_i_Index -1], INJ_m_i_Index, para);    
}

/*interface of crank & cam sync*/
/*IRS_ECU_CDD_SYN.2~11*/
void EPS_GetCrankSta(void){
    EPS_m_st_CrkSync = EtpuEngPosGetCrankState(ETPU_APP_CHAN_CRANK);    
}

void EPS_GetCamSta(void){
    EPS_m_st_CamSync = EtpuEngPosGetCamState();
}

void EPS_GetCrankSpd(void){
    EPS_m_n_Crank = EtpuEngPosGetCrankSpeed(ETPU_APP_CHAN_CRANK, etpu_a_tcr1_freq);
}

void EPS_GetCamSpd(void){
    EPS_m_n_Cam = EtpuEngPosGetCamSpeed(ETPU_APP_CHAN_CAM, etpu_a_tcr1_freq);
}

void EPS_GetCrankAndCamAvgSpd(void){
    aveSpeedCompt();
    EPS_m_n_CrankAvg = aveCrankSpd;
    EPS_m_n_CamAvg = aveCamSpd;    
}

void EPS_GetEngSpdAndAvgSpd(void){
/*TODO:
crank ok: engine speed and engine avg_speed equals crank speed and avg_speed.
crank err & cam ok: engine speed and engine avg_speed equals cam speed and avg_speed.
crank err & cam err: engine speed and engine avg_speed equals 0.
*/
    if(1){  /*crank ok*/
        EPS_m_n_Engine = EPS_m_n_Crank;
        EPS_m_n_EngineAvg = EPS_m_n_CrankAvg;
    }else if(0){    /*crank err & cam ok*/
        EPS_m_n_Engine = EPS_m_n_Cam;
        EPS_m_n_EngineAvg = EPS_m_n_CamAvg;
    }else{  /*both err*/
        EPS_m_n_Engine = 0;
        EPS_m_n_EngineAvg = 0;
    }
}

/*
sync status of engine, include 5 statemachine, see etpu_crank_auto.h for more information
1) 发动机同步初始化
FS_ETPU_ENG_POS_SEEK 
2) 发动机首次半同步，此时已验证了缺齿，但还没有凸轮信号
FS_ETPU_ENG_POS_FIRST_HALF_SYNC 
3) 发动机预同步，此时在当前曲轴圈中，检测到了凸轮信号，
还需进一步确认当前曲轴是第1圈还是第2圈
FS_ETPU_ENG_POS_PRE_FULL_SYNC
4) 发动机半同步，正常运行后，若某次未在期望的位置检测到凸轮信号后进入 
FS_ETPU_ENG_POS_HALF_SYNC 
5) 发动机同步
FS_ETPU_ENG_POS_FULL_SYNC
*/
void EPS_GetEngSta(void){
    EPS_m_st_Status = EtpuEngPosGetSyncState(); 
}

void EPS_GetSenSta(void){
    /*TODO:
    crank ok: senser status is crank senser.
    crank err & cam ok: senser status is cam senser.
    crank err & cam err: senser status is err
    */
    if(1){  /*crank ok*/
        EPS_m_st_SenSta = EPS_SEN_STA_CRANK;
    }else if(0){    /*crank err & cam ok*/
        EPS_m_st_SenSta = EPS_SEN_STA_CAM;
    }else{  /*both err*/
        EPS_m_st_SenSta = EPS_SEN_STA_ERR;
    }
}

void EPS_Bit (void){
    EPS_m_st_Error = EPS_OK;
}

