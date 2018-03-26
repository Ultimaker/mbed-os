/* mbed Microcontroller Library
 * Copyright (c) 2006-2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stddef.h>
#include "us_ticker_api.h"
#include "PeripheralNames.h"
#include "hal_tick.h"

// A 16-bit timer is used
#if TIM_MST_16BIT

TIM_HandleTypeDef TimMasterHandle;

// Declare ticker as 16 bits - mbed layer takes care of mapping to 32 or 64
// bits
const ticker_info_t* us_ticker_get_info()
{
    static const ticker_info_t info = {
        1000000,
        16
    };
    return &info;
}

void us_ticker_init(void)
{
    /* NOTE: assuming thaiHAL tick has already been initialized! */
    us_ticker_disable_interrupt();
}

uint32_t us_ticker_read()
{
    return TIM_MST->CNT;
}

void us_ticker_set_interrupt(timestamp_t timestamp)
{
    // NOTE: This function must be called with interrupts disabled to keep our
    //       timer interrupt setup atomic
    // Set new output compare value
    __HAL_TIM_SET_COMPARE(&TimMasterHandle, TIM_CHANNEL_1, timestamp & 0xFFFF);
    // Ensure the compare event starts clear
    __HAL_TIM_CLEAR_FLAG(&TimMasterHandle, TIM_FLAG_CC1);
    // Enable IT
    __HAL_TIM_ENABLE_IT(&TimMasterHandle, TIM_IT_CC1);
}

void us_ticker_fire_interrupt(void)
{
    // Ensure the compare event starts clear
    __HAL_TIM_CLEAR_FLAG(&TimMasterHandle, TIM_FLAG_CC1);
    LL_TIM_GenerateEvent_CC1(TimMasterHandle.Instance);
    // Enable IT
    __HAL_TIM_ENABLE_IT(&TimMasterHandle, TIM_IT_CC1);

}

/* NOTE: must be called with interrupts disabled! */
void us_ticker_disable_interrupt(void)
{
    __HAL_TIM_DISABLE_IT(&TimMasterHandle, TIM_IT_CC1);
}

/* NOTE: must be called with interrupts disabled! */
void us_ticker_clear_interrupt(void)
{
    __HAL_TIM_CLEAR_FLAG(&TimMasterHandle, TIM_FLAG_CC1);
}

#endif // TIM_MST_16BIT
