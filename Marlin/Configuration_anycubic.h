/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once
/*
*
*       Anycubic 4Max Pro Specific
*
*/
#ifdef EXTENSIBLE_UI

  // Enable Anycubic TFT
  #define ANYCUBIC_SOFT_TFT_MODEL

  #ifdef ANYCUBIC_SOFT_TFT_MODEL
    #define ANYCUBIC_TFT_DEBUG
    #define ANYCUBIC_GENERIC_SERIAL
    #define ANYCUBIC_POWERDOWN
    //#define ANYCUBIC_FILAMENT_RUNOUT

    // Play stock music
    //#define ANYCUBIC_MUSIC

    // Play a (non-earpiercing) startup chime on startup/serial connection
    // of the Trigorilla board
    //#define ANYCUBIC_STARTUP_CHIME

    //
    // Short 2KHz beep when endstops are hit
    //
    //#define ANYCUBIC_ENDSTOP_BEEP

    #ifdef ANYCUBIC_GENERIC_SERIAL
      #if MOTHERBOARD == BOARD_TRIGORILLA_14
        #define INTERNAL_SERIAL_PORT 3
      #elif MOTHERBOARD == BOARD_BIGTREE_SKR_V1_3
        #undef SERIAL_PORT_2
      #endif
    #else
      #define ANYCUBIC_TFT_SERIAL
    #endif

  #endif
#endif

#if MOTHERBOARD == BOARD_BIGTREE_SKR_V1_3
  // 4Max Pro Specific Pin Definitions

  #define I2C_SCL         P0_01      // E1_STEP_PIN / SCL / UART_RXD3 Direct Connect to MCU, like AUX PINS
  #define I2C_SDA         P0_00      // E1_DIR_PIN / SDA / UART_TXD3 Direct Connect to MCU, like AUX PINS

  //#define E1_ENABLE_PIN   P0_10      // Direct Connect to MCU, like AUX PINS

  
/**
 *               _____                                              _____
 *           NC | · · | GND                                     5V | · · | GND
 *        RESET | · · | 1.31 (SD_DETECT)             (LCD_D7) 1.23 | · · | 1.22 (LCD_D6)
 *  (MOSI) 0.18 | · · | 3.25 (BTN_EN2)               (LCD_D5) 1.21 | · · | 1.20 (LCD_D4)
 * (SD_SS) 0.16 | · · | 3.26 (BTN_EN1)               (LCD_RS) 1.19 | · · | 1.18 (LCD_EN)
 *   (SCK) 0.15 | · · | 0.17 (MISO)                 (BTN_ENC) 0.28 | · · | 1.30 (BEEPER)
 *               -----                                              -----
 *               EXP2                                               EXP1
 */

  //#define TEMP_CHAMBER_PIN    P0_25_A2     // P0_25_A2   // A2 (T2) - (69) - TEMP_1_PIN
  #define CASE_LIGHT_PIN      P2_04        // Enclosure Light Pin (TRIGORILLA_HEATER_1_PIN 45)

  #ifdef USE_EXTERNAL_FAN_MOSFET
    #define CONTROLLER_FAN_PIN            7   // Stepper / Mother Board Cooling Fan
    #define ANYCUBIC_E0_AUTO_FAN_PIN      44  // Hotend Cooling Fan
  #else
    #define ANYCUBIC_E0_AUTO_FAN_PIN      -1  // Disable Auto Hotend fan
  #endif

  #define EXT_SCK_PIN          P0_15
  #define EXT_MISO_PIN         P0_17
  #define EXT_MOSI_PIN         P0_18
  #define EXT_CS               P0_18

  #define DIRECT_AIN      P1_30_A4   // (37) not 5V tolerant

  //
  // SD Support
  //
  //#define ONBOARD_SD_CS_PIN  P0_06   // Chip select for "System" SD card
  //#define SD_DETECT_PIN    P0_27
  //#define SCK_PIN          P0_07
  //#define MISO_PIN         P0_08
  //#define MOSI_PIN         P0_09
  //#define SS_PIN           ONBOARD_SD_CS_PIN

  #define POWER_OFF_PIN 	      P1_23 // One cable goes from power module to D21 pin and nearby 12V and GND pins
  //#define  KILL_PIN             P1_21
#endif

#if MOTHERBOARD == BOARD_TRIGORILLA_14
  // 4Max Pro Specific Pin Definitions

  // Limit Switches and UART3 shares pins
  // Z+  Z- Y+   Y-  X+  X-
  // D19 D18 D15 D14 D2  D3
  // GND GND GND GND GND GND
  // 5V  5V  5V  5V  5V  5V

  #define X_MIN_PIN            3
  #define Y_MIN_PIN           19
  #define Z_MIN_PIN           18

  //#define X_MAX_PIN           -1 //This pin is used as Z_TEST
  //#define Y_MAX_PIN           -1
  #define Z_MAX_PIN           -1 // This pin is used for consumable detection

  // Temp Sensors
  // GND      GND      GND      GND
  // A12(D66) A15(D69) A14(D68) A13(D67)

  #define TEMP_0_PIN          13   // Analog Input
  #define TEMP_1_PIN          14   // Analog Input
  #define TEMP_2_PIN          15   // Analog Input
  #define TEMP_3_PIN          12   // Analog Input

  #define TEMP_BED_PIN        14   // Analog Input
  #define TEMP_CHAMBER_PIN    15   // Analog Input

  // Heat Bed  Heater 1  Heater 0
  // D8        D45       D10
  #define HEATER_1_PIN        -1
  #define CASE_LIGHT_PIN      45 // Enclosure Light Pin (TRIGORILLA_HEATER_1_PIN 45)

  // Fan0 Fan1 Fan2
  // D9   D7   D44
  #define CONTROLLER_FAN_PIN    7 //Stepper / Mother Board Cooling Fan
  #define V5_COOLING_PIN       44 //Hotend Cooling Fan

  // ISP
  // RST D52 D50
  // GND D51 5V

  // SD Card
  // D53 D51 GND 5V D52 D50 D49 GND
  #define SDPOWER_PIN         -1
  #define SDSS                53 // SD Card Chil Select pin / SD Card CS Pin for SIDO communication
  //#define SCK_PIN           52
  //#define MISO_PIN          50
  //#define MOSI_PIN          51
  //#define SS_PIN            53

  #define POWER_OFF_PIN 	    21 // One cable goes from power module to D21 pin and nearby 12V and GND pins

  #define FIL_RUNOUT_PIN         33  // Cable goes from flat cable hub board
  #define PS_ON_PIN              -1 // 12 NOT USE

  #if defined(ANYCUBIC_SOFT_TFT_MODEL)
    #define  BEEPER_PIN             31
    #define  SD_DETECT_PIN          49
    #define  KILL_PIN               41
  #endif

  // EXP1
  // D37  D35
  // D17  D16
  // D23  D25
  // D27  D29
  // GND  5V

  // EXP2
  // D50  D52
  // D31  D53
  // D33  D51
  // D49  D41
  // GND  RST

  // IIC
  // 12V GND D21 D20 GND 5V

  // UART3 and Limit Switches shares pins
  // GND D15 D14 V5

  // TFT Cable Pinout
  // YELLOW GREY BLUE RED GREEN
  // GND    D15  D14  RST 5V

  // HUB (need to rewrite)
  // BROWN YELLOW GREEN BLUE RED      BLACK
  // D19   D18    D2    D3   A13(D67) GND

  // AUX
  // D42 GND 5V
  // D43 GND 5V

  // Servo
  // 5V  5V  5V  5V  5V
  // GND GND GND GND GND
  // D11 D4  D6  D5  D12

  /*
  PIN                   Port          Bindings in Marlin
  D2 (PWM, INT4)        E4            <unused/unknown>
  D3 (PWM, INT5)        E5            X_MIN_PIN
  D4 (PWM)              G5            SERVO1_PIN
  D5 (PWM, AIN1)        E3            SERVO0_PIN
  D6 (PWM)              H3            SERVO3_PIN
  D7 (PWM)              H4            CONTROLLER_FAN_PIN
  D8 (PWM)              H5            HEATER_BED_PIN
  D9 (PWM)              H6            FAN_PIN
  D10 (PWM, PCINT4)     B4            HEATER_0_PIN
  D11 (PWM, PCINT5)     B5            SERVO2_PIN
  D12 (PWM, PCINT6)     B6            <unused/unknown>
  D14 (TX3 / PCINT10)   J1            TFT
  D15 (RX3 / PCINT9)    J0            TFT
  D16 (TX2)             H1            <unused/unknown>
  D17 (RX2)             H0            <unused/unknown>
  D18 (TX1 / INT3)      D3            Z_MIN_PIN
  D19 (RX1 / INT2)      D2            Y_MIN_PIN
  D20 (SDA / INT1)      D1            <unused/unknown>
  D21 (SCL / INT0)      D0            POWER_OFF_PIN
  D23                   A1            <unused/unknown>
  D25                   A3            <unused/unknown>
  D27                   A5            <unused/unknown>
  D29                   A7            <unused/unknown>
  D31                   C6            BEEPER_PIN
  D33                   C4            FIL_RUNOUT_PIN
  D35                   C2            <unused/unknown>
  D37                   C0            <unused/unknown>
  D41                   G0            KILL_PIN
  D42                   L7            <unused/unknown>
  D43                   L6            <unused/unknown>
  D44 (PWM)             L5            FAN2_PIN, E0_AUTO_FAN_PIN
  D45 (PWM)             L4            HEATER_1_PIN / CASE_LIGHT_PIN
  D49                   L0            SD_DETECT_PIN
  D50 (MISO / PCINT3)   B3            MISO_PIN
  D51 (MOSI / PCINT2)   B2            MOSI_PIN
  D52 (SCK / PCINT1)    B1            SCK_PIN
  D53 (SS / PCINT0)     B0            SS_PIN, SDSS
  D66 — A12 (PCINT20)   K4            TEMP_2_PIN
  D67 — A13 (PCINT21)   K5            TEMP_0_PIN
  D68 — A14 (PCINT22)   K6            TEMP_BED_PIN
  D69 — A15 (PCINT23)   K7            TEMP_1_PIN
  */
#endif