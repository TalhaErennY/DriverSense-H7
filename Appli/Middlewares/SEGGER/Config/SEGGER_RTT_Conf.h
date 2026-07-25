/*********************************************************************
*                   (c) SEGGER Microcontroller GmbH                  *
*                        The Embedded Experts                        *
*                           www.segger.com                           *
**********************************************************************
*                                                                    *
*        SEGGER RTT * Real Time Transfer for embedded targets        *
*                  https://github.com/SEGGERMicro/RTT                *
*                                                                    *
**********************************************************************

---------------------------END-OF-HEADER------------------------------
Purpose : User configuration file for RTT.
          For available configuration,
          refer to SEGGER_RTT_ConfDefaults.h.

----------------------------------------------------------------------
*/

#ifndef SEGGER_RTT_CONF_H
#define SEGGER_RTT_CONF_H


/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************/
/*
 * RTT up buffer:
 * Target -> Host
 */
#define SEGGER_RTT_MAX_NUM_UP_BUFFERS       (3)

/*
 * RTT down buffer:
 * Host -> Target
 */
#define SEGGER_RTT_MAX_NUM_DOWN_BUFFERS     (3)

/*
 * Channel 0 terminal output buffer.
 */
#define BUFFER_SIZE_UP                      (1024)

/*
 * Channel 0 terminal input buffer.
 */
#define BUFFER_SIZE_DOWN                    (16)

/*
 * RTT printf temp format buffer
 */
#define SEGGER_RTT_PRINTF_BUFFER_SIZE       (128)

/*
 * If RTT buffer overflow, do NOT block the RTOS application
 */
#define SEGGER_RTT_MODE_DEFAULT             SEGGER_RTT_MODE_NO_BLOCK_SKIP

/*
 * STM32H7 Cortex-M7 locking level for BASEPRI
 *
 * If in FreeRTOSConfig.h:
 * configPRIO_BITS = 4
 * configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY = 5
 *
 * then:
 * 5 << (8 - 4) = 0x50
 */
#define SEGGER_RTT_MAX_INTERRUPT_PRIORITY   (0x50)

#endif
/*************************** End of file ****************************/
