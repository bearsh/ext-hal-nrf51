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

#ifndef ANALOGIN_H_
#define ANALOGIN_H_

#include "analogin_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*adc_irq_handler)(uint32_t id);

void analogin_config(analogin_t *obj, uint8_t res, uint8_t inpsel, uint8_t refsel, uint8_t extrefsel);
void analogin_enable(analogin_t *obj);
void analogin_disable(analogin_t *obj);
void analogin_start(analogin_t *obj);
int analogin_busy(analogin_t *obj);
uint16_t analogin_read_result_u16(analogin_t *obj);
void analogin_irq_set(analogin_t *obj, uint32_t id, uint32_t enable);
void analogin_irq_set_handler(analogin_t *obj, adc_irq_handler handler);
void analogin_irq_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ANALOGIN_H_ */
