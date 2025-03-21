/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*=====================================================================================
 * Description:
 *   The Modbus parameter structures used to define Modbus instances that
 *   can be addressed by Modbus protocol. Define these structures per your needs in
 *   your application. Below is just an example of possible parameters.
 *====================================================================================*/
#ifndef _DEVICE_PARAMS
#define _DEVICE_PARAMS

#include <stdint.h>

// This file defines structure of modbus parameters which reflect correspond modbus address space
// for each modbus register type (coils, discreet inputs, holding registers, input registers)
#pragma pack(push, 1)
typedef struct {
    uint8_t discrete_input;
} discrete_reg_params_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t coils_port0;
    uint8_t coils_port1;
} coil_reg_params_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint16_t input_data0;
    uint16_t input_data1;
    uint16_t input_data2;
    uint16_t input_data3;
    // uint16_t data[150]; // 8 + 150 = 158
    uint16_t input_data4;
    uint16_t input_data5;
    uint16_t input_data6;
    uint16_t input_data7;
    // uint16_t data_block1[150];

    uint16_t input_temp_0;
    uint16_t input_temp_1;

    uint16_t input_voltage_0;
    uint16_t input_voltage_1;

    uint16_t input_current_0;
    uint16_t input_current_1;
} input_reg_params_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    float holding_data0;
    float holding_data1;
    float holding_data2;
    float holding_data3;
    uint16_t test_regs[150];
    float holding_data4;
    float holding_data5;
    float holding_data6;
    float holding_data7;
} holding_reg_params_t;
#pragma pack(pop)

// extern holding_reg_params_t holding_reg_params;
extern input_reg_params_t input_reg_params;
extern coil_reg_params_t coil_reg_params;
// extern discrete_reg_params_t discrete_reg_params;

#endif  // !defined(_DEVICE_PARAMS)