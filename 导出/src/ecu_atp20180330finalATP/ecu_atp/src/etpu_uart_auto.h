
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
 *  File            : etpuc_uart_auto.h                                         
 *  Description     : UART头文件                                         
 *  Call            : 029-89186510                                             
 *  该文件由ETPU自动生成，请勿修改！！！                                       
 ******************************************************************************/
#ifndef _ETPU_UART_AUTO_H_
#define _ETPU_UART_AUTO_H_

/* 函数相关 */
#define FS_ETPU_UART_FUNCTION_NUMBER 4 
#define FS_ETPU_UART_TABLE_SELECT 1 
#define FS_ETPU_UART_NUM_PARMS 0x0018 

/* 主服务请求 */
#define FS_ETPU_UART_TX_INIT 4 
#define FS_ETPU_UART_RX_INIT 7 

/* 停止位配置 */
#define FS_ETPU_UART_STOPBITS_1 0 
#define FS_ETPU_UART_STOPBITS_05 1 
#define FS_ETPU_UART_STOPBITS_15 2 
#define FS_ETPU_UART_STOPBITS_2 3 

/* 校验位配置 */
#define FS_ETPU_UART_NO_PARITY 0 
#define FS_ETPU_UART_EVEN_PARITY (0  + 2 )
#define FS_ETPU_UART_ODD_PARITY (1  + 2 )

/* 控制寄存器掩码 */
/* 发送使能标志，0表示使能 */
#define FS_ETPU_UART_TXRE_MASK 1 
/* 发送完成标志，1表示发送完成 */
#define FS_ETPU_UART_TXOV_MASK 2 
/* 接收帧错误标志，1表示接收帧错误 */
#define FS_ETPU_UART_RXFE_MASK 4 
/* 接收校验错标志，1表示接收校验错 */
#define FS_ETPU_UART_RXPE_MASK 8 
/* 接收完成标志，1表示接收完成，数据已准备好 */
#define FS_ETPU_UART_RXDR_MASK 16 

/* 参数偏移 */
#define FS_ETPU_UART_BAUD_RATE_OFFSET  0x0001
#define FS_ETPU_UART_CTRL_REG_OFFSET  0x0000
#define FS_ETPU_UART_DATA_REG_OFFSET  0x0005
#define FS_ETPU_UART_DATA_BITS_OFFSET  0x0004
#define FS_ETPU_UART_STOP_BITS_OFFSET  0x0008

#endif

