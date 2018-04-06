/*------------------------------------------------------------------------
   版权 2017-2027 中国航空工业集团第六三一所第十八研究室
   
   对本文件的拷贝、发布、修改或者其他任何用途必须得到
   中国航空工业集团第六三一所的书面协议许可
   
   File : GPIO.c
   
   Description : GPIO驱动
   
   
   Revision History:
   rev.0.01 2017-11-16,马振华,创建
------------------------------------------------------------------------*/

#include "mpc5644a.h"  
#include "typedefs.h"  
#include "gpio.h" 
#include "flexCAN.h"


GPINRESULT gpiResult;
GPOUTRESULT gpoResult;
GPOUTBITRESULT gpoBitResult;

/************************************************************************
* @brief 
* 		fs_gpio_read_data	离散量输入
* @param[in]  port:通道号
* @param[out] 无		  
* @return     无
************************************************************************/
uint8_t fs_gpio_read_data(uint16_t port)
{

	return( SIU.GPDI[port].R );
}

/************************************************************************
* @brief 
* 		fs_gpio_write_data	离散量输出
* @param[in]  port:通道号
*             value:离散量输出值
* @param[out] 无		  
* @return     无
************************************************************************/
void fs_gpio_write_data(uint16_t port, uint8_t value)
{

	SIU.GPDO[port].R = value;
}
 
/************************************************************************
* @brief 
* 		gpiConfig	离散量输入信号管脚配置
* @param[in]  port:通道号
*			  hys:Input hysteresis
*			  wpe:Weak pullup/down enable
*			  wps:Weak pullup/down select
* @param[out] 无		  
* @return     无
************************************************************************/
void gpiConfig(uint16_t port, uint16_t hys, uint16_t wpe, uint16_t wps)
{ 
	SIU.PCR[port].B.PA = 0;  /* GPIO */  
	SIU.PCR[port].B.IBE = 1; /* INPUT */
	SIU.PCR[port].B.OBE = 0; /* OUTPUT */
	SIU.PCR[port].B.DSC = 0; /* 00 10 pF drive strength */
	SIU.PCR[port].B.ODE = 0;
	SIU.PCR[port].B.HYS = hys; /* 0 Disable hysteresis for the pad,1 Enable hysteresis for the pad */
	SIU.PCR[port].B.SRC = 1; /*SRC=01 中等转换速率 */
	SIU.PCR[port].B.WPE = wpe; /* 0 Disable weak pull device for the pad,1 Enable weak pull device for the pad. */
	SIU.PCR[port].B.WPS = wps; /* 0 Pulldown is enabled for the pad,1 Pullup is enabled for the pad. */ 	
} 

/************************************************************************
* @brief 
* 		gpoConfig	离散量输出信号管脚配置
* @param[in]  port:通道号
*             dsc:Drive strength control
*			  ode:Open drain output enable
*			  src:Slew rate control
* @param[out] 无		  
* @return     无
************************************************************************/
void gpoConfig(uint16_t port, uint16_t dsc, uint16_t ode, uint16_t src)
{ 
	SIU.PCR[port].B.PA = 0;  /* GPIO */  
	SIU.PCR[port].B.IBE = 0; /* INPUT */
	SIU.PCR[port].B.OBE = 1; /* OUTPUT */
	SIU.PCR[port].B.DSC = dsc; /* 00 10 pF drive strength,01 20 pF drive strength,10 30 pF drive strength,11 50 pF drive strength */
	SIU.PCR[port].B.ODE = ode; /* 11 50 pF drive strength,1 Enable open drain for the pad. */
	SIU.PCR[port].B.HYS = 0; /* 0 Disable hysteresis for the pad,1 Enable hysteresis for the pad */
	SIU.PCR[port].B.SRC = src; /*SRC=01 中等转换速率 */
	SIU.PCR[port].B.WPE = 0; /* 0 Disable weak pull device for the pad,1 Enable weak pull device for the pad. */
	SIU.PCR[port].B.WPS = 0; /* 0 Pulldown is enabled for the pad,1 Pullup is enabled for the pad. */	
} 

/************************************************************************
* @brief 
* 		gkInConfig	所有离散量输入信号管脚配置
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void gkInConfig(void)
{
	uint16_t hys = 0, wpe = 0, wps = 0;
	
	/* 配置D/K离散量输入 */
	gpiConfig(77, hys, wpe, wps); /* 发动机制动开关GPIO[77] */
	gpiConfig(81, hys, wpe, wps); /* 调速器选择/最大车速限制开关 */
	gpiConfig(219, hys, wpe, wps); /* 远程油门使能开关 */
	gpiConfig(92, hys, wpe, wps); /* AC压力开关 */
	gpiConfig(76, hys, wpe, wps); /* 离合器开关 */
	gpiConfig(78, hys, wpe, wps); /* PTO使能开关 */
	gpiConfig(80, hys, wpe, wps); /* 油门互锁开关 */
	gpiConfig(82, hys, wpe, wps); /* 远程PTO使能开关 */
	gpiConfig(91, hys, wpe, wps); /* PTO+开关 */
	gpiConfig(98, hys, wpe, wps); /* 诊断开关 */
	gpiConfig(75, hys, wpe, wps); /* 行车制动开关 */
	gpiConfig(90, hys, wpe, wps); /* PTO-开关 GPIO[141] */
	gpiConfig(191, hys, wpe, wps); /* 报警指示灯 EMIOS12 */
	gpiConfig(190, hys, wpe, wps); /*  停机指示灯 EMIOS11 */
	gpiConfig(193, hys, wpe, wps); /*  “等待起动”指示灯 EMIOS14 */
	gpiConfig(192, hys, wpe, wps); /*  mil lamp指示灯 EMIOS13 */
	
	/* 配置24V/K离散量输入 */
	gpiConfig(202, hys, wpe, wps); /* 钥匙开关信号 EMIOS23 */
	gpiConfig(185, hys, wpe, wps); /* 启动马达锁定BIT EMIOS6 */
	gpiConfig(182, hys, wpe, wps); /* 进气加热器继电器_BIT EMIOS3 */
	gpiConfig(183, hys, wpe, wps); /* 排气制动输出_BIT EMIOS4 */	
	gpiConfig(110, hys, wpe, wps); /* UREA_LINE_HEATER_RELAY_BIT, GPIO[110] */	
}

/************************************************************************
* @brief 
* 		gkInTst	离散量输入信号测试
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void gkInTst(void)
{
	gpiResult.B.GPI0 = fs_gpio_read_data(77);  /* 发动机制动开关GPIO[77] */                      
	gpiResult.B.GPI1 = fs_gpio_read_data(81);  /* 调速器选择/最大车速限制开关 */                 
	gpiResult.B.GPI2 = fs_gpio_read_data(219);  /* 远程油门使能开关 */                            
	gpiResult.B.GPI3 = fs_gpio_read_data(92);  /* AC压力开关 */                                  
	gpiResult.B.GPI4 = fs_gpio_read_data(76);  /* 离合器开关 */                                  
	gpiResult.B.GPI5 = fs_gpio_read_data(78);  /* PTO使能开关 */                                 
	gpiResult.B.GPI6 = fs_gpio_read_data(80);  /* 油门互锁开关 */                                
	gpiResult.B.GPI7 = fs_gpio_read_data(82);  /* 远程PTO使能开关 */                             
	gpiResult.B.GPI8 = fs_gpio_read_data(91);  /* PTO+开关 */                                    
	gpiResult.B.GPI9 = fs_gpio_read_data(98);  /* 诊断开关 */                                    
	gpiResult.B.GPI10 = fs_gpio_read_data(75);  /* 行车制动开关 */                                
	gpiResult.B.GPI11 = fs_gpio_read_data(90);  /* PTO-开关 GPIO[141] */                          
	gpiResult.B.GPI12 = fs_gpio_read_data(202);  /* 钥匙开关信号 EMIOS23 */                                      
	
	CANATransmitMsg(1, 0x10, (uint8_t *)(&gpiResult), 4); 
	delay(5); /* 延迟5msCANB发送数据 */
	CANBTransmitMsg(1, 0x10, (uint8_t *)(&gpiResult), 4);	
}

/************************************************************************
* @brief 
* 		gkOutConfig	  所有离散量输出信号管脚测试
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void gkOutConfig(void)
{
	uint16_t dsc = 0, ode = 0, src = 1;
	
	/* 配置D/K离散量输出 */
	gpoConfig(187, dsc, ode, src); /* 报警指示灯  EMIOS8 */
	gpoConfig(186, dsc, ode, src); /* 停机指示灯  EMIOS7 */
	gpoConfig(189, dsc, ode, src); /* “等待起动”指示灯  EMIOS10 */
	gpoConfig(188, dsc, ode, src); /* mil lamp指示灯  EMIOS9 */
	
	/* 配置24V/K离散量输出 */
	gpoConfig(181, dsc, ode, src); /* 启动马达锁定  EMIOS2 */
	gpoConfig(179, dsc, ode, src); /* 进气加热器继电器  EMIOS0 */
	gpoConfig(180, dsc, ode, src); /* 排气制动输出  EMIOS1 */
	gpoConfig(143, dsc, ode, src); /* UREA_LINE_HEATER_RELAY  GPIO143 */
}

/************************************************************************
* @brief 
* 		gkOutTst	离散量输出信号测试
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void gkOutTst(void)
{  
	uint8_t i = 0;  
	uint8_t bitResult[8] = {0};
    
    for (i = 0; i < 8; i++)	
    {
        bitResult[i] = 0;   
    }
	
	gpoResult.B.GPO0 = 0;  /* 报警指示灯 EMIOS12 */                          
	gpoResult.B.GPO1 = 0;  /*  停机指示灯 EMIOS11 */                         
	gpoResult.B.GPO2 = 0;  /*  “等待起动”指示灯 EMIOS14 */                 
	gpoResult.B.GPO3 = 0;  /*  mil lamp指示灯 EMIOS13 */                     
	gpoResult.B.GPO4 = 0;  /* 启动马达锁定BIT EMIOS6 */                      
	gpoResult.B.GPO5 = 0;  /* 进气加热器继电器_BIT EMIOS3 */                 
	gpoResult.B.GPO6 = 0;  /* 排气制动输出_BIT EMIOS4 */	                    
	gpoResult.B.GPO7 = 0;  /* UREA_LINE_HEATER_RELAY_BIT, GPIO[110] */
	    	
	/* 首先设置所有离散量输出为1 */
	fs_gpio_write_data(187, 1);
	fs_gpio_write_data(186, 1);
	fs_gpio_write_data(189, 1);
	fs_gpio_write_data(188, 1);
	fs_gpio_write_data(181, 1);
	fs_gpio_write_data(179, 1);
	fs_gpio_write_data(180, 1);
	fs_gpio_write_data(143, 1);
	delay(10); /* 等待10ms回采 */ 
	
	/* 接着每10ms回采一次，共采集3次 */ 
	for (i = 0; i < 3; i++)
	{ 
	    gpoResult.B.GPO0 += fs_gpio_read_data(191);  /* 报警指示灯 EMIOS12 */                          
	    gpoResult.B.GPO1 += fs_gpio_read_data(190);  /*  停机指示灯 EMIOS11 */                         
	    gpoResult.B.GPO2 += fs_gpio_read_data(193);  /*  “等待起动”指示灯 EMIOS14 */                 
	    gpoResult.B.GPO3 += fs_gpio_read_data(192);  /*  mil lamp指示灯 EMIOS13 */                     
	    gpoResult.B.GPO4 += fs_gpio_read_data(185);  /* 启动马达锁定BIT EMIOS6 */                      
	    gpoResult.B.GPO5 += fs_gpio_read_data(182);  /* 进气加热器继电器_BIT EMIOS3 */                 
	    gpoResult.B.GPO6 += fs_gpio_read_data(183);  /* 排气制动输出_BIT EMIOS4 */	                    
	    gpoResult.B.GPO7 += fs_gpio_read_data(110);  /* UREA_LINE_HEATER_RELAY_BIT, GPIO[110] */
	    delay(10); /* 等待10ms */  
	}
	
	/* 最后判断若三次采集值均为1则该路离散量输出1测试合格；否则不合格 */	
	if (gpoResult.B.GPO0 == 0) /* D/K输出1时BIT反馈为0 */
	{
		bitResult[0] = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		bitResult[0] = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (gpoResult.B.GPO1 == 0) 
	{
		bitResult[1] = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		bitResult[1] = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (gpoResult.B.GPO2 == 0) 
	{
		bitResult[2] = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		bitResult[2] = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (gpoResult.B.GPO3 == 0) 
	{
		bitResult[3] = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		bitResult[3] = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (gpoResult.B.GPO4 == 3) /* 24V/K输出1时BIT反馈为0 */
	{
		bitResult[4] = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		bitResult[4] = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (gpoResult.B.GPO5 == 3) 
	{
		bitResult[5] = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		bitResult[5] = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (gpoResult.B.GPO6 == 3) 
	{
		bitResult[6] = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		bitResult[6] = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (gpoResult.B.GPO7 == 3) 
	{
		bitResult[7] = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		bitResult[7] = 0; /* 该路离散量输出1测试不合格 */
	}
    delay(1000); /* 保持高电平1s */

	
	/* 再次设置所有离散量输出为0 */
	fs_gpio_write_data(187, 0);
	fs_gpio_write_data(186, 0);
	fs_gpio_write_data(189, 0);
	fs_gpio_write_data(188, 0);
	fs_gpio_write_data(181, 0);
	fs_gpio_write_data(179, 0);
	fs_gpio_write_data(180, 0);
	fs_gpio_write_data(143, 0);  
	delay(10); /* 等待10ms回采 */  
	
	gpoResult.B.GPO0 = 0;  /* 报警指示灯 EMIOS12 */                          
	gpoResult.B.GPO1 = 0;  /*  停机指示灯 EMIOS11 */                         
	gpoResult.B.GPO2 = 0;  /*  “等待起动”指示灯 EMIOS14 */                 
	gpoResult.B.GPO3 = 0;  /*  mil lamp指示灯 EMIOS13 */                     
	gpoResult.B.GPO4 = 0;  /* 启动马达锁定BIT EMIOS6 */                      
	gpoResult.B.GPO5 = 0;  /* 进气加热器继电器_BIT EMIOS3 */                 
	gpoResult.B.GPO6 = 0;  /* 排气制动输出_BIT EMIOS4 */	                    
	gpoResult.B.GPO7 = 0;  /* UREA_LINE_HEATER_RELAY_BIT, GPIO[110] */
	
	/* 接着每10ms回采一次，共采集三次 */ 
	for (i = 0; i < 3; i++)
	{ 
	    gpoResult.B.GPO0 += fs_gpio_read_data(191);  /* 报警指示灯 EMIOS12 */                          
	    gpoResult.B.GPO1 += fs_gpio_read_data(190);  /*  停机指示灯 EMIOS11 */                         
	    gpoResult.B.GPO2 += fs_gpio_read_data(193);  /*  “等待起动”指示灯 EMIOS14 */                 
	    gpoResult.B.GPO3 += fs_gpio_read_data(192);  /*  mil lamp指示灯 EMIOS13 */                     
	    gpoResult.B.GPO4 += fs_gpio_read_data(185);  /* 启动马达锁定BIT EMIOS6 */                      
	    gpoResult.B.GPO5 += fs_gpio_read_data(182);  /* 进气加热器继电器_BIT EMIOS3 */                 
	    gpoResult.B.GPO6 += fs_gpio_read_data(183);  /* 排气制动输出_BIT EMIOS4 */	                    
	    gpoResult.B.GPO7 += fs_gpio_read_data(110);  /* UREA_LINE_HEATER_RELAY_BIT, GPIO[110] */
	    delay(10); /* 等待10ms */  
	}
	
	/* 最后判断若三次采集值均为0则该路离散量输出0测试合格；否则不合格 */
	if (gpoResult.B.GPO0 == 3) 
	{
		bitResult[0] += 2; /* 三次采集值均为0则该路离散量输出0测试合格 */
	}
	else
	{
		bitResult[0] += 0; /* 该路离散量输出0测试不合格 */
	}
	
	if (gpoResult.B.GPO1 == 3) 
	{
		bitResult[1] += 2; /* 三次采集值均为0则该路离散量输出0测试合格 */
	}
	else
	{
		bitResult[1] += 0; /* 该路离散量输出0测试不合格 */
	}
	
	if (gpoResult.B.GPO2 == 3) 
	{
		bitResult[2] += 2; /* 三次采集值均为0则该路离散量输出0测试合格 */
	}
	else
	{
		bitResult[2] += 0; /* 该路离散量输出0测试不合格 */
	}
	
	if (gpoResult.B.GPO3 == 3) 
	{
		bitResult[3] += 2; /* 三次采集值均为0则该路离散量输出0测试合格 */
	}
	else
	{
		bitResult[3] += 0; /* 该路离散量输出0测试不合格 */
	}
	
	if (gpoResult.B.GPO4 == 0) 
	{
		bitResult[4] += 2; /* 三次采集值均为0则该路离散量输出0测试合格 */
	}
	else
	{
		bitResult[4] += 0; /* 该路离散量输出0测试不合格 */
	}
	
	if (gpoResult.B.GPO5 == 0) 
	{
		bitResult[5] += 2; /* 三次采集值均为0则该路离散量输出0测试合格 */
	}
	else
	{
		bitResult[5] += 0; /* 该路离散量输出0测试不合格 */
	}
	
	if (gpoResult.B.GPO6 == 0) 
	{
		bitResult[6] += 2; /* 三次采集值均为0则该路离散量输出0测试合格 */
	}
	else
	{
		bitResult[6] += 0; /* 该路离散量输出0测试不合格 */
	}
	
	if (gpoResult.B.GPO7 == 0) 
	{
		bitResult[7] += 2; /* 三次采集值均为0则该路离散量输出0测试合格 */
	}
	else
	{
		bitResult[7] += 0; /* 该路离散量输出0测试不合格 */
	}  
	
	/* 当离散量输出1和0均测试合格则该路离散量输出测试合格；否则不合格 */
	if (bitResult[0] == 3) 
	{
		gpoBitResult.GPO0 = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		gpoBitResult.GPO0 = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (bitResult[1] == 3) 
	{
		gpoBitResult.GPO1 = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		gpoBitResult.GPO1 = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (bitResult[2] == 3) 
	{
		gpoBitResult.GPO2 = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		gpoBitResult.GPO2 = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (bitResult[3] == 3) 
	{
		gpoBitResult.GPO3 = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		gpoBitResult.GPO3 = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (bitResult[4] == 3) 
	{
		gpoBitResult.GPO4 = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		gpoBitResult.GPO4 = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (bitResult[5] == 3) 
	{
		gpoBitResult.GPO5 = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		gpoBitResult.GPO5 = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (bitResult[6] == 3) 
	{
		gpoBitResult.GPO6 = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		gpoBitResult.GPO6 = 0; /* 该路离散量输出1测试不合格 */
	}
	
	if (bitResult[7] == 3) 
	{
		gpoBitResult.GPO7 = 1; /* 三次采集值均为1则该路离散量输出1测试合格 */
	}
	else
	{
		gpoBitResult.GPO7 = 0; /* 该路离散量输出1测试不合格 */
	} 
	
	CANATransmitMsg(1, 0x50, (uint8_t *)(&gpoBitResult), 4);
	delay(5); /* 延迟5msCANB发送数据 */
	CANBTransmitMsg(1, 0x50, (uint8_t *)(&gpoBitResult), 4);
}

