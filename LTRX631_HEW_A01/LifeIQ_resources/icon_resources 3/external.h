/*****************************************************************************
 * FileName:        external.h
 * Processor:       PIC32MX
 * Compiler:        MPLAB C32/XC32 (see release notes for tested revision)
 * Linker:          MPLAB LINK32/XC32
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright(c) 2012 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * AUTO-GENERATED CODE:  Graphics Resource Converter version: 3.28.15
 *****************************************************************************/

#ifndef EXTERNAL_H_FILE
#define EXTERNAL_H_FILE



/*****************************************************************************
 * This is a unique marker to verify the external memory data
 * The marker is a total of 8 bytes in length starting at the location GRC_CRC32_EXTERNAL_ADDR.
 * The first four bytes, starting at GRC_CRC32_EXTERNAL_ADDR, will always equal 'M', 'C', 'H', 'P'.
 * The next four byte is the generated 32-bit CRC.  The application can compare the value read from the
 * external memory to the value of GRC_CRC32_EXTERNAL_MARKER to verify that the data is valid.
 *****************************************************************************/
#define GRC_CRC32_EXTERNAL_MARKER 0xCC06238Bul
#define GRC_CRC32_EXTERNAL_ADDR 0x003F3DD0ul

/*****************************************************************************
 * SECTION:  BITMAPS
 *****************************************************************************/

/*********************************
 * Bitmap Structure
 * Label: NORF
 * Description:  32x32 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL NORF;
#define NORF_WIDTH     (32)
#define NORF_HEIGHT    (32)
#define NORF_SIZE      (4102)
/*********************************
 * Bitmap Structure
 * Label: USB_
 * Description:  40x24 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL USB_;
#define USB__WIDTH     (40)
#define USB__HEIGHT    (24)
#define USB__SIZE      (3846)
/*********************************
 * Bitmap Structure
 * Label: WIIQ
 * Description:  32x32 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL WIIQ;
#define WIIQ_WIDTH     (32)
#define WIIQ_HEIGHT    (32)
#define WIIQ_SIZE      (4102)
/*********************************
 * Bitmap Structure
 * Label: background_avail
 * Description:  480x272 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL background_avail;
#define background_avail_WIDTH     (480)
#define background_avail_HEIGHT    (272)
#define background_avail_SIZE      (522246)
/*********************************
 * Bitmap Structure
 * Label: background_charge_equal
 * Description:  480x272 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL background_charge_equal;
#define background_charge_equal_WIDTH     (480)
#define background_charge_equal_HEIGHT    (272)
#define background_charge_equal_SIZE      (522246)
/*********************************
 * Bitmap Structure
 * Label: background_defaut
 * Description:  480x272 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL background_defaut;
#define background_defaut_WIDTH     (480)
#define background_defaut_HEIGHT    (272)
#define background_defaut_SIZE      (522246)
/*********************************
 * Bitmap Structure
 * Label: background_submenu
 * Description:  480x272 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL background_submenu;
#define background_submenu_WIDTH     (480)
#define background_submenu_HEIGHT    (272)
#define background_submenu_SIZE      (522246)
/*********************************
 * Bitmap Structure
 * Label: background_wait_menu_password
 * Description:  480x272 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL background_wait_menu_password;
#define background_wait_menu_password_WIDTH     (480)
#define background_wait_menu_password_HEIGHT    (272)
#define background_wait_menu_password_SIZE      (522246)
/*********************************
 * Bitmap Structure
 * Label: bal
 * Description:  32x24 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL bal;
#define bal_WIDTH     (32)
#define bal_HEIGHT    (24)
#define bal_SIZE      (3078)
/*********************************
 * Bitmap Structure
 * Label: bat_temp
 * Description:  64x120 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL bat_temp;
#define bat_temp_WIDTH     (64)
#define bat_temp_HEIGHT    (120)
#define bat_temp_SIZE      (30726)
/*********************************
 * Bitmap Structure
 * Label: bullet_1
 * Description:  16x16 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL bullet_1;
#define bullet_1_WIDTH     (16)
#define bullet_1_HEIGHT    (16)
#define bullet_1_SIZE      (1030)
/*********************************
 * Bitmap Structure
 * Label: bullet_2
 * Description:  16x16 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL bullet_2;
#define bullet_2_WIDTH     (16)
#define bullet_2_HEIGHT    (16)
#define bullet_2_SIZE      (1030)
/*********************************
 * Bitmap Structure
 * Label: cable
 * Description:  88x64 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL cable;
#define cable_WIDTH     (88)
#define cable_HEIGHT    (64)
#define cable_SIZE      (22534)
/*********************************
 * Bitmap Structure
 * Label: charger_icon
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL charger_icon;
#define charger_icon_WIDTH     (40)
#define charger_icon_HEIGHT    (40)
#define charger_icon_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: charger_icon_select
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL charger_icon_select;
#define charger_icon_select_WIDTH     (40)
#define charger_icon_select_HEIGHT    (40)
#define charger_icon_select_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: clock
 * Description:  78x86 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL clock;
#define clock_WIDTH     (78)
#define clock_HEIGHT    (86)
#define clock_SIZE      (26838)
/*********************************
 * Bitmap Structure
 * Label: enforcerimpaq
 * Description:  208x79 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL enforcerimpaq;
#define enforcerimpaq_WIDTH     (208)
#define enforcerimpaq_HEIGHT    (79)
#define enforcerimpaq_SIZE      (65734)
/*********************************
 * Bitmap Structure
 * Label: equal
 * Description:  48x32 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL equal;
#define equal_WIDTH     (48)
#define equal_HEIGHT    (32)
#define equal_SIZE      (6150)
/*********************************
 * Bitmap Structure
 * Label: equal_warning
 * Description:  32x24 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL equal_warning;
#define equal_warning_WIDTH     (32)
#define equal_warning_HEIGHT    (24)
#define equal_warning_SIZE      (3078)
/*********************************
 * Bitmap Structure
 * Label: express
 * Description:  208x79 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL express;
#define express_WIDTH     (208)
#define express_HEIGHT    (79)
#define express_SIZE      (65734)
/*********************************
 * Bitmap Structure
 * Label: icon_USB
 * Description:  128x128 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL icon_USB;
#define icon_USB_WIDTH     (128)
#define icon_USB_HEIGHT    (128)
#define icon_USB_SIZE      (65542)
/*********************************
 * Bitmap Structure
 * Label: icon_charger
 * Description:  128x128 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL icon_charger;
#define icon_charger_WIDTH     (128)
#define icon_charger_HEIGHT    (128)
#define icon_charger_SIZE      (65542)
/*********************************
 * Bitmap Structure
 * Label: icon_equal
 * Description:  32x24 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL icon_equal;
#define icon_equal_WIDTH     (32)
#define icon_equal_HEIGHT    (24)
#define icon_equal_SIZE      (3078)
/*********************************
 * Bitmap Structure
 * Label: icon_exit
 * Description:  128x128 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL icon_exit;
#define icon_exit_WIDTH     (128)
#define icon_exit_HEIGHT    (128)
#define icon_exit_SIZE      (65542)
/*********************************
 * Bitmap Structure
 * Label: icon_logs
 * Description:  128x128 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL icon_logs;
#define icon_logs_WIDTH     (128)
#define icon_logs_HEIGHT    (128)
#define icon_logs_SIZE      (65542)
/*********************************
 * Bitmap Structure
 * Label: icon_password
 * Description:  128x128 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL icon_password;
#define icon_password_WIDTH     (128)
#define icon_password_HEIGHT    (128)
#define icon_password_SIZE      (65542)
/*********************************
 * Bitmap Structure
 * Label: icon_settings
 * Description:  128x128 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL icon_settings;
#define icon_settings_WIDTH     (128)
#define icon_settings_HEIGHT    (128)
#define icon_settings_SIZE      (65542)
/*********************************
 * Bitmap Structure
 * Label: jauge_DF4
 * Description:  60x184 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL jauge_DF4;
#define jauge_DF4_WIDTH     (60)
#define jauge_DF4_HEIGHT    (184)
#define jauge_DF4_SIZE      (44166)
/*********************************
 * Bitmap Structure
 * Label: jauge_avail
 * Description:  60x184 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL jauge_avail;
#define jauge_avail_WIDTH     (60)
#define jauge_avail_HEIGHT    (184)
#define jauge_avail_SIZE      (44166)
/*********************************
 * Bitmap Structure
 * Label: jauge_charge
 * Description:  60x184 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL jauge_charge;
#define jauge_charge_WIDTH     (60)
#define jauge_charge_HEIGHT    (184)
#define jauge_charge_SIZE      (44166)
/*********************************
 * Bitmap Structure
 * Label: jauge_left
 * Description:  60x184 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL jauge_left;
#define jauge_left_WIDTH     (60)
#define jauge_left_HEIGHT    (184)
#define jauge_left_SIZE      (44166)
/*********************************
 * Bitmap Structure
 * Label: jauge_right
 * Description:  60x184 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL jauge_right;
#define jauge_right_WIDTH     (60)
#define jauge_right_HEIGHT    (184)
#define jauge_right_SIZE      (44166)
/*********************************
 * Bitmap Structure
 * Label: jauge_time
 * Description:  60x184 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL jauge_time;
#define jauge_time_WIDTH     (60)
#define jauge_time_HEIGHT    (184)
#define jauge_time_SIZE      (44166)
/*********************************
 * Bitmap Structure
 * Label: lan_green
 * Description:  24x24 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL lan_green;
#define lan_green_WIDTH     (24)
#define lan_green_HEIGHT    (24)
#define lan_green_SIZE      (2310)
/*********************************
 * Bitmap Structure
 * Label: lan_red
 * Description:  24x24 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL lan_red;
#define lan_red_WIDTH     (24)
#define lan_red_HEIGHT    (24)
#define lan_red_SIZE      (2310)
/*********************************
 * Bitmap Structure
 * Label: lan_white
 * Description:  24x24 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL lan_white;
#define lan_white_WIDTH     (24)
#define lan_white_HEIGHT    (24)
#define lan_white_SIZE      (2310)
/*********************************
 * Bitmap Structure
 * Label: lifeIQModular
 * Description:  208x79 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL lifeIQModular;
#define lifeIQModular_WIDTH     (208)
#define lifeIQModular_HEIGHT    (79)
#define lifeIQModular_SIZE      (65734)
/*********************************
 * Bitmap Structure
 * Label: lifetechmod1c
 * Description:  208x79 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL lifetechmod1c;
#define lifetechmod1c_WIDTH     (208)
#define lifetechmod1c_HEIGHT    (79)
#define lifetechmod1c_SIZE      (65734)
/*********************************
 * Bitmap Structure
 * Label: loading
 * Description:  48x56 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL loading;
#define loading_WIDTH     (48)
#define loading_HEIGHT    (56)
#define loading_SIZE      (10758)
/*********************************
 * Bitmap Structure
 * Label: logs_icon
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL logs_icon;
#define logs_icon_WIDTH     (40)
#define logs_icon_HEIGHT    (40)
#define logs_icon_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: logs_icon_select
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL logs_icon_select;
#define logs_icon_select_WIDTH     (40)
#define logs_icon_select_HEIGHT    (40)
#define logs_icon_select_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: menu_header
 * Description:  480x44 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL menu_header;
#define menu_header_WIDTH     (480)
#define menu_header_HEIGHT    (44)
#define menu_header_SIZE      (84486)
/*********************************
 * Bitmap Structure
 * Label: module
 * Description:  16x32 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL module;
#define module_WIDTH     (16)
#define module_HEIGHT    (32)
#define module_SIZE      (2054)
/*********************************
 * Bitmap Structure
 * Label: module_DF
 * Description:  16x32 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL module_DF;
#define module_DF_WIDTH     (16)
#define module_DF_HEIGHT    (32)
#define module_DF_SIZE      (2054)
/*********************************
 * Bitmap Structure
 * Label: module_vide
 * Description:  16x32 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL module_vide;
#define module_vide_WIDTH     (16)
#define module_vide_HEIGHT    (32)
#define module_vide_SIZE      (2054)
/*********************************
 * Bitmap Structure
 * Label: nexsys
 * Description:  208x79 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL nexsys;
#define nexsys_WIDTH     (208)
#define nexsys_HEIGHT    (79)
#define nexsys_SIZE      (65734)
/*********************************
 * Bitmap Structure
 * Label: password
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL password;
#define password_WIDTH     (40)
#define password_HEIGHT    (40)
#define password_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: password_icon
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL password_icon;
#define password_icon_WIDTH     (40)
#define password_icon_HEIGHT    (40)
#define password_icon_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: password_icon_select
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL password_icon_select;
#define password_icon_select_WIDTH     (40)
#define password_icon_select_HEIGHT    (40)
#define password_icon_select_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: password_select
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL password_select;
#define password_select_WIDTH     (40)
#define password_select_HEIGHT    (40)
#define password_select_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: return_icon
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL return_icon;
#define return_icon_WIDTH     (40)
#define return_icon_HEIGHT    (40)
#define return_icon_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: return_icon_select
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL return_icon_select;
#define return_icon_select_WIDTH     (40)
#define return_icon_select_HEIGHT    (40)
#define return_icon_select_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: settings_icon
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL settings_icon;
#define settings_icon_WIDTH     (40)
#define settings_icon_HEIGHT    (40)
#define settings_icon_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: settings_icon_select
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL settings_icon_select;
#define settings_icon_select_WIDTH     (40)
#define settings_icon_select_HEIGHT    (40)
#define settings_icon_select_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: temp_avail
 * Description:  24x48 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL temp_avail;
#define temp_avail_WIDTH     (24)
#define temp_avail_HEIGHT    (48)
#define temp_avail_SIZE      (4614)
/*********************************
 * Bitmap Structure
 * Label: temp_charge
 * Description:  24x48 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL temp_charge;
#define temp_charge_WIDTH     (24)
#define temp_charge_HEIGHT    (48)
#define temp_charge_SIZE      (4614)
/*********************************
 * Bitmap Structure
 * Label: timer
 * Description:  48x56 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL timer;
#define timer_WIDTH     (48)
#define timer_HEIGHT    (56)
#define timer_SIZE      (10758)
/*********************************
 * Bitmap Structure
 * Label: title_menu_back
 * Description:  152x32 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL title_menu_back;
#define title_menu_back_WIDTH     (152)
#define title_menu_back_HEIGHT    (32)
#define title_menu_back_SIZE      (19462)
/*********************************
 * Bitmap Structure
 * Label: usb_icon
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL usb_icon;
#define usb_icon_WIDTH     (40)
#define usb_icon_HEIGHT    (40)
#define usb_icon_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: usb_icon_select
 * Description:  40x40 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL usb_icon_select;
#define usb_icon_select_WIDTH     (40)
#define usb_icon_select_HEIGHT    (40)
#define usb_icon_select_SIZE      (6406)
/*********************************
 * Bitmap Structure
 * Label: warning
 * Description:  24x21 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL warning;
#define warning_WIDTH     (24)
#define warning_HEIGHT    (21)
#define warning_SIZE      (2022)
/*********************************
 * Bitmap Structure
 * Label: warning_AH_back
 * Description:  96x32 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL warning_AH_back;
#define warning_AH_back_WIDTH     (96)
#define warning_AH_back_HEIGHT    (32)
#define warning_AH_back_SIZE      (12294)
/*********************************
 * Bitmap Structure
 * Label: warning_charge
 * Description:  24x24 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL warning_charge;
#define warning_charge_WIDTH     (24)
#define warning_charge_HEIGHT    (24)
#define warning_charge_SIZE      (2310)
/*********************************
 * Bitmap Structure
 * Label: warning_default
 * Description:  128x112 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL warning_default;
#define warning_default_WIDTH     (128)
#define warning_default_HEIGHT    (112)
#define warning_default_SIZE      (57350)
/*********************************
 * Bitmap Structure
 * Label: warning_defaut
 * Description:  136x112 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL warning_defaut;
#define warning_defaut_WIDTH     (136)
#define warning_defaut_HEIGHT    (112)
#define warning_defaut_SIZE      (60934)
/*********************************
 * Bitmap Structure
 * Label: water
 * Description:  32x24 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL water;
#define water_WIDTH     (32)
#define water_HEIGHT    (24)
#define water_SIZE      (3078)
/*********************************
 * Bitmap Structure
 * Label: water_level
 * Description:  120x112 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL water_level;
#define water_level_WIDTH     (120)
#define water_level_HEIGHT    (112)
#define water_level_SIZE      (53766)
/*********************************
 * Bitmap Structure
 * Label: wifi_green
 * Description:  24x24 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL wifi_green;
#define wifi_green_WIDTH     (24)
#define wifi_green_HEIGHT    (24)
#define wifi_green_SIZE      (2310)
/*********************************
 * Bitmap Structure
 * Label: wifi_red
 * Description:  24x24 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL wifi_red;
#define wifi_red_WIDTH     (24)
#define wifi_red_HEIGHT    (24)
#define wifi_red_SIZE      (2310)
/*********************************
 * Bitmap Structure
 * Label: wifi_white
 * Description:  24x24 pixels, 24-bits per pixel
 ***********************************/
extern const IMAGE_EXTERNAL wifi_white;
#define wifi_white_WIDTH     (24)
#define wifi_white_HEIGHT    (24)
#define wifi_white_SIZE      (2310)
#endif

