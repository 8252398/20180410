
/*******************************************************************************
 * ��Ȩ 2015-2016 �й����չ�ҵ���ŵ�����һ����ʮ���о���                       
 *                                                                             
 * �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ��й����չ�ҵ���ŵ�����һ��
 * ������Э����ɡ�                                                            
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
 *  Description     : UARTͷ�ļ�                                         
 *  Call            : 029-89186510                                             
 *  ���ļ���ETPU�Զ����ɣ������޸ģ�����                                       
 ******************************************************************************/
#ifndef _ETPU_UART_AUTO_H_
#define _ETPU_UART_AUTO_H_

/* ������� */
#define FS_ETPU_UART_FUNCTION_NUMBER 4 
#define FS_ETPU_UART_TABLE_SELECT 1 
#define FS_ETPU_UART_NUM_PARMS 0x0018 

/* ���������� */
#define FS_ETPU_UART_TX_INIT 4 
#define FS_ETPU_UART_RX_INIT 7 

/* ֹͣλ���� */
#define FS_ETPU_UART_STOPBITS_1 0 
#define FS_ETPU_UART_STOPBITS_05 1 
#define FS_ETPU_UART_STOPBITS_15 2 
#define FS_ETPU_UART_STOPBITS_2 3 

/* У��λ���� */
#define FS_ETPU_UART_NO_PARITY 0 
#define FS_ETPU_UART_EVEN_PARITY (0  + 2 )
#define FS_ETPU_UART_ODD_PARITY (1  + 2 )

/* ���ƼĴ������� */
/* ����ʹ�ܱ�־��0��ʾʹ�� */
#define FS_ETPU_UART_TXRE_MASK 1 
/* ������ɱ�־��1��ʾ������� */
#define FS_ETPU_UART_TXOV_MASK 2 
/* ����֡�����־��1��ʾ����֡���� */
#define FS_ETPU_UART_RXFE_MASK 4 
/* ����У����־��1��ʾ����У��� */
#define FS_ETPU_UART_RXPE_MASK 8 
/* ������ɱ�־��1��ʾ������ɣ�������׼���� */
#define FS_ETPU_UART_RXDR_MASK 16 

/* ����ƫ�� */
#define FS_ETPU_UART_BAUD_RATE_OFFSET  0x0001
#define FS_ETPU_UART_CTRL_REG_OFFSET  0x0000
#define FS_ETPU_UART_DATA_REG_OFFSET  0x0005
#define FS_ETPU_UART_DATA_BITS_OFFSET  0x0004
#define FS_ETPU_UART_STOP_BITS_OFFSET  0x0008

#endif

