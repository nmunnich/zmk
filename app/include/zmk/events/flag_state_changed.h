/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>

struct zmk_flag_state_changed {
    uint8_t flag_id;
    bool new_state;
    bool locked;
    int64_t timestamp;
};

ZMK_EVENT_DECLARE(zmk_flag_state_changed);

static inline int raise_flag_state_changed(uint8_t flag_id, bool new_state, bool locked) {
    return raise_zmk_flag_state_changed((struct zmk_flag_state_changed){
        .flag_id = flag_id, .new_state = new_state, .locked = locked, .timestamp = k_uptime_get()});
}
