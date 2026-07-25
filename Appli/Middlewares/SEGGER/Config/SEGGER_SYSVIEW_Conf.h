/*********************************************************************
*                   (c) SEGGER Microcontroller GmbH                  *
*                        The Embedded Experts                        *
*                           www.segger.com                           *
**********************************************************************
*                                                                    *
*         SEGGER SystemView  * Real-time application analysis        *
*              https://github.com/SEGGERMicro/SystemView             *
*                                                                    *
**********************************************************************

---------------------------END-OF-HEADER------------------------------

Purpose : SEGGER SystemView configuration file.
          Set defines which deviate from the defaults (see SEGGER_SYSVIEW_ConfDefaults.h) here.          

Additional information:
  Required defines which must be set are:
    SEGGER_SYSVIEW_GET_TIMESTAMP
    SEGGER_SYSVIEW_GET_INTERRUPT_ID
  For known compilers and cores, these might be set to good defaults
  in SEGGER_SYSVIEW_ConfDefaults.h.
  
  SystemView needs a (nestable) locking mechanism.
  If not defined, the RTT locking mechanism is used,
  which then needs to be properly configured.
*/

#ifndef SEGGER_SYSVIEW_CONF_H
#define SEGGER_SYSVIEW_CONF_H

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************/
/*
 * SystemView RTT buffer.
 */
#define SEGGER_SYSVIEW_RTT_BUFFER_SIZE      (32 * 1024)

/*********************************************************************
*
*       Define: SEGGER_SYSVIEW_SECTION
*
*  Description
*    Section to place the SystemView RTT Buffer into.
*  Default
*    undefined: Do not place into a specific section.
*  Notes
*    If SEGGER_RTT_SECTION is defined, the default changes to use
*    this section for the SystemView RTT Buffer, too.
*/
#if !(defined SEGGER_SYSVIEW_SECTION) && (defined SEGGER_RTT_BUFFER_SECTION)
  #define SEGGER_SYSVIEW_SECTION                  SEGGER_RTT_BUFFER_SECTION
#endif


/*********************************************************************
* TODO: Add your defines here.                                       *
**********************************************************************/
/*
 * Timestamp source:
 * Cortex-M7 DWT cycle counter register.
 */
#define SEGGER_SYSVIEW_GET_TIMESTAMP()      (*(volatile unsigned int *)0xE0001004U)

/*
 * Active interrupt/exception ID:
 * SCB->ICSR, VECTACTIVE field
 */
#define SEGGER_SYSVIEW_GET_INTERRUPT_ID()   ((*(volatile unsigned int *)0xE000ED04U) & 0x1FFU)

/*
 * DWT->CYCCNT is a 32-bit counter.
 */
#define SEGGER_SYSVIEW_TIMESTAMP_BITS       32

#endif  // SEGGER_SYSVIEW_CONF_H

/*************************** End of file ****************************/
