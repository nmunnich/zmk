/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>

struct zmk_device_state_changed {
    uint8_t state_id;
    bool state;
    bool locked;
    int64_t timestamp;
};

ZMK_EVENT_DECLARE(zmk_device_state_changed);

static inline int raise_device_state_changed(uint8_t state_id, bool state, bool locked) {
    return raise_zmk_device_state_changed((struct zmk_device_state_changed){
        .state_id = state_id, .state = state, .locked = locked, .timestamp = k_uptime_get()});
}
