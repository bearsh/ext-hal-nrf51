/*
 * Copyright (c) 2016, Martin Gysel, me@bearsh.org
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ext-hal-nrf51/analogin.h"
#include "cmsis.h"
#include "compiler-polyfill/attributes.h"

static uint32_t channel_ids[6] = {0};
static adc_irq_handler irq_handler = 0;

static uint32_t channelmap(uint32_t channel) {
	switch (channel) {
	case 1:   return 0;
	case 2:   return 2;
	case 4:   return 3;
	case 8:   return 4;
	case 16:  return 5;
	case 32:  return 6;
	case 64:  return 7;
	case 128: return 8;
	default: return -1;
	}
}


#ifdef __cplusplus
extern "C" {
#endif
void ADC_IRQHandler(void) {
	uint32_t pin = channelmap((NRF_ADC->CONFIG & ADC_CONFIG_PSEL_Msk) >> ADC_CONFIG_PSEL_Pos);

	NRF_ADC->EVENTS_END = 0;

	if (channel_ids[pin] && irq_handler) {
		irq_handler(channel_ids[pin]);
	}
}
#ifdef __cplusplus
}
#endif

void analogin_irq_init(void) {
    NVIC_SetPriority(ADC_IRQn, 3);
    NVIC_EnableIRQ(ADC_IRQn);
}

void analogin_config(analogin_t __unused *obj, uint8_t res, uint8_t inpsel, uint8_t refsel, uint8_t extrefsel)
{
	uint8_t enabled = 0;
	if (NRF_ADC->ENABLE == ADC_ENABLE_ENABLE_Enabled) {
		NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Disabled;
		enabled = 1;
	}
	NRF_ADC->CONFIG &= ~(ADC_CONFIG_RES_Msk | ADC_CONFIG_INPSEL_Msk | ADC_CONFIG_REFSEL_Msk | ADC_CONFIG_EXTREFSEL_Msk);
	NRF_ADC->CONFIG |= (res<<ADC_CONFIG_REFSEL_Pos) | (inpsel<<ADC_CONFIG_INPSEL_Pos) |
			(refsel<<ADC_CONFIG_REFSEL_Pos) | (extrefsel<<ADC_CONFIG_EXTREFSEL_Pos);
	if (enabled) {
		NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;
	}
}

void analogin_enable(analogin_t __unused *obj)
{
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;
}

void analogin_disable(analogin_t __unused *obj)
{
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Disabled;
}

void analogin_start(analogin_t *obj)
{
    NRF_ADC->CONFIG     &= ~ADC_CONFIG_PSEL_Msk;
    NRF_ADC->CONFIG     |= obj->adc_pin << ADC_CONFIG_PSEL_Pos;
    NRF_ADC->TASKS_START = 1;
}

int analogin_busy(analogin_t __unused *obj)
{
    return ((NRF_ADC->BUSY & ADC_BUSY_BUSY_Msk) >> ADC_BUSY_BUSY_Pos) == ADC_BUSY_BUSY_Busy;
}

uint16_t analogin_read_result_u16(analogin_t __unused *obj)
{
	return (uint16_t)NRF_ADC->RESULT; // 10 bit
}

void analogin_irq_set(analogin_t* obj, uint32_t id, uint32_t enable)
{
	static uint32_t irq_enabled = 0;
	uint32_t ch_id = channelmap(obj->adc_pin);

	if (enable) {
		irq_enabled |= obj->adc_pin;
		channel_ids[ch_id] = id;
	} else {
		irq_enabled &= ~obj->adc_pin;
		channel_ids[ch_id] = 0;
	}
	if (irq_enabled) {
		NRF_ADC->INTENSET = 1;
	} else {
		NRF_ADC->INTENCLR = 1;
	}
}

void analogin_irq_set_handler(analogin_t __unused *obj, adc_irq_handler handler)
{
	irq_handler = handler;
}
