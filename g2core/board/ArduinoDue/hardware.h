/*
 * hardware.h - system hardware configuration
 * For: /board/ArduinoDue
 * THIS FILE IS HARDWARE PLATFORM SPECIFIC - ARM version
 *
 * This file is part of the g2core project
 *
 * Copyright (c) 2013 - 2018 Alden S. Hart, Jr.
 * Copyright (c) 2013 - 2018 Robert Giseburt
 *
 * This file ("the software") is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2 as published by the
 * Free Software Foundation. You should have received a copy of the GNU General Public
 * License, version 2 along with the software.  If not, see <http://www.gnu.org/licenses/> .
 *
 * As a special exception, you may use this file as part of a software library without
 * restriction. Specifically, if other files instantiate templates or use macros or
 * inline functions from this file, or you compile this file and link it with  other
 * files to produce an executable, this file does not by itself cause the resulting
 * executable to be covered by the GNU General Public License. This exception does not
 * however invalidate any other reasons why the executable file might be covered by the
 * GNU General Public License.
 *
 * THE SOFTWARE IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT WITHOUT ANY
 * WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "config.h"
#include "settings.h"
#include "error.h"

#include "MotateUtilities.h" // for HOT_FUNC and HOT_DATA

#ifndef HARDWARE_H_ONCE
#define HARDWARE_H_ONCE

/*--- Hardware platform enumerations ---*/

#define G2CORE_HARDWARE_PLATFORM    "ArduinoDue"
#define G2CORE_HARDWARE_VERSION     "na"

/***** Motors & PWM channels supported by this hardware *****/
// These must be defines (not enums) so expressions like this:
//  #if (MOTORS >= 6)  will work

#ifndef HAS_LASER
#define HAS_LASER 0
#endif

#ifndef HAS_PRESSURE
#define HAS_PRESSURE 0
#endif

#if HAS_LASER
#define MOTORS 5                    // number of motors + one "laser" motor (used for pulsing the laser in sync)
#else
#define MOTORS 4                    // number of motors supported the hardware
#endif
#define PWMS 2                      // number of PWM channels supported the hardware
#define AXES 6                      // axes to support -- must be 6 or 9

/*************************
 * Global System Defines *
 *************************/

#define MILLISECONDS_PER_TICK 1     // MS for system tick (systick * N)
#define SYS_ID_LEN 40               // total length including dashes and NUL

/*************************
 * Motate Setup          *
 *************************/

#include "MotatePins.h"
#include "MotateTimers.h"           // for TimerChanel<> and related...
#include "MotateUtilities.h"           // for TimerChanel<> and related...

using Motate::TimerChannel;

using Motate::pin_number;
using Motate::Pin;
using Motate::PWMOutputPin;
using Motate::OutputPin;

/************************************************************************************
 **** ARM SAM3X8E SPECIFIC HARDWARE *************************************************
 ************************************************************************************/

/**** Resource Assignment via Motate ****
 *
 * This section defines resource usage for pins, timers, PWM channels, communications
 * and other resources. Please refer to /motate/utility/SamPins.h, SamTimers.h and
 * other files for pinouts and other configuration details.
 *
 * Commenting out or #ifdef'ing out definitions below will cause the compiler to
 * drop references to these resources from the compiled code. This will reduce
 * compiled code size and runtime CPU cycles. E.g. if you are compiling for a 3 motor,
 * XYZ axis config commenting out the higher motors and axes here will remove them
 * from later code (using the motate .isNull() test).
 */

/* Interrupt usage and priority
 *
 * The following interrupts are defined w/indicated priorities
 *
 *	 0	DDA_TIMER (3) for step pulse generation
 *	 1	DWELL_TIMER (4) for dwell timing
 *	 2	LOADER software generated interrupt (STIR / SGI)
 *	 3	Serial read character interrupt
 *	 4	EXEC software generated interrupt (STIR / SGI)
 *	 5	Serial write character interrupt
 */

/**** Stepper DDA and dwell timer settings ****/

#define FREQUENCY_DDA       100000UL        // Hz step frequency. Interrupts actually fire at 2x (200 KHz)
#define FREQUENCY_DWELL     1000UL
#define MIN_SEGMENT_MS ((double)1.0) 

#define PLANNER_QUEUE_SIZE (48)
#define SECONDARY_QUEUE_SIZE (10)

/**** Motate Definitions ****/

// Timer definitions. See stepper.h and other headers for setup
typedef TimerChannel<3,0> dda_timer_type;       // stepper pulse generation in stepper.cpp
typedef TimerChannel<4,0> exec_timer_type;      // request exec timer in stepper.cpp
typedef TimerChannel<5,0> fwd_plan_timer_type;  // request exec timer in stepper.cpp

/**** SPI Setup ****/
#include "MotateSPI.h"
typedef Motate::SPIBus<Motate::kSPI_MISOPinNumber, Motate::kSPI_MOSIPinNumber, Motate::kSPI_SCKPinNumber> SPIBus_used_t;
extern SPIBus_used_t spiBus;

// Pin assignments

pin_number indicator_led_pin_num = Motate::kLED_USBRXPinNumber;
static PWMOutputPin<indicator_led_pin_num> IndicatorLed;

/**** Motate Global Pin Allocations ****/

//static OutputPin<kSocket1_SPISlaveSelectPinNumber> spi_ss1_pin;
//static OutputPin<kSocket2_SPISlaveSelectPinNumber> spi_ss2_pin;
//static OutputPin<kSocket3_SPISlaveSelectPinNumber> spi_ss3_pin;
//static OutputPin<kSocket4_SPISlaveSelectPinNumber> spi_ss4_pin;
//static OutputPin<kSocket5_SPISlaveSelectPinNumber> spi_ss5_pin;
//static OutputPin<kSocket6_SPISlaveSelectPinNumber> spi_ss6_pin;
static OutputPin<Motate::kKinen_SyncPinNumber> kinen_sync_pin;

static OutputPin<Motate::kGRBL_ResetPinNumber> grbl_reset_pin;
static OutputPin<Motate::kGRBL_FeedHoldPinNumber> grbl_feedhold_pin;
static OutputPin<Motate::kGRBL_CycleStartPinNumber> grbl_cycle_start_pin;

static OutputPin<Motate::kGRBL_CommonEnablePinNumber> motor_common_enable_pin;

// Input pins are defined in gpio.cpp

/********************************
 * Function Prototypes (Common) *
 ********************************/

const configSubtable *const getSysConfig_3();

void hardware_init(void);       // master hardware init
stat_t hardware_periodic();     // callback from the main loop (time sensitive)
void hw_hard_reset(void);
stat_t hw_flash(nvObj_t *nv);

stat_t hw_get_fb(nvObj_t *nv);
stat_t hw_get_fv(nvObj_t *nv);
stat_t hw_get_hp(nvObj_t *nv);
stat_t hw_get_hv(nvObj_t *nv);
stat_t hw_get_fbs(nvObj_t *nv);
stat_t hw_get_fbc(nvObj_t *nv);
stat_t hw_get_id(nvObj_t *nv);

#ifdef __TEXT_MODE

    void hw_print_fb(nvObj_t *nv);
    void hw_print_fv(nvObj_t *nv);
    void hw_print_fbs(nvObj_t *nv);
    void hw_print_fbc(nvObj_t *nv);
    void hw_print_hp(nvObj_t *nv);
    void hw_print_hv(nvObj_t *nv);
    void hw_print_id(nvObj_t *nv);

#else

    #define hw_print_fb tx_print_stub
    #define hw_print_fv tx_print_stub
    #define hw_print_fbs tx_print_stub
    #define hw_print_fbc tx_print_stub
    #define hw_print_hp tx_print_stub
    #define hw_print_hv tx_print_stub
    #define hw_print_id tx_print_stub

#endif // __TEXT_MODE

#endif	// end of include guard: HARDWARE_H_ONCE
