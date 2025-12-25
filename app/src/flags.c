/*
 * Copyright (c) 2025 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zmk/flags.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <zmk/event_manager.h>
#include <zmk/events/flag_state_changed.h>

#include <drivers/behavior.h>
#include <zephyr/sys/util.h>
#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/stdlib.h>
#include <zmk/behavior.h>
#include <zmk/physical_layouts.h>
#include <zmk/matrix.h>
#include <zmk/sensors.h>
#include <zmk/virtual_key_position.h>

#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/sensor_event.h>

static zmk_flag_state_t _zmk_flag_locks = 0;
static zmk_flag_state_t _zmk_flag_states = 0;

zmk_flag_state_t zmk_flag_states(void) { return _zmk_flag_states; }

zmk_flag_state_t zmk_flag_locks(void) { return _zmk_flag_locks; }

bool zmk_flag_active_in_states(zmk_flag_id_t flag, zmk_flag_state_t state_to_test) {
    return (state_to_test & (BIT(flag))) == (BIT(flag));
};

bool zmk_flag_is_active(zmk_flag_id_t flag) {
    return zmk_flag_active_in_states(flag, _zmk_flag_states);
};

bool zmk_flag_is_locked(zmk_flag_id_t flag) {
    return zmk_flag_active_in_states(flag, _zmk_flag_locks);
}

static inline int set_flag_state(zmk_flag_id_t flag, bool state, bool locking) {
    int ret = 0;
    if (flag >= ZMK_KEYMAP_LAYERS_LEN) {
        return -EINVAL;
    }

    // Non-forcing disables should not change a locked active flag
    if (!locking && !state && (_zmk_flag_locks & BIT(flag))) {
        return ret;
    }

    zmk_flag_state_t old_state = _zmk_flag_states;
    zmk_flag_state_t old_locks = _zmk_flag_locks;
    WRITE_BIT(_zmk_flag_states, flag, state);
    if (locking) {
        WRITE_BIT(_zmk_flag_locks, flag, state);
    }
    // Don't send flag changes unless there was an actual change
    if (old_state != _zmk_flag_states || old_locks != _zmk_flag_locks) {
        LOG_DBG("layer_changed: flag %d flag %d locked %d", flag, state, locking);

        ret = raise_flag_state_changed(flag, state, locking);
        if (ret < 0) {
            LOG_WRN("Failed to raise flag flag changed (%d)", ret);
        }
    }

    return ret;
}

int zmk_flag_activate(zmk_flag_id_t flag, bool locking) {
    return set_flag_state(flag, true, locking);
};

int zmk_flag_deactivate(zmk_flag_id_t flag, bool locking) {
    return set_flag_state(flag, false, locking);
};

int zmk_flag_toggle_state(zmk_flag_id_t flag, bool locking) {
    if (zmk_flag_is_active(flag) && (!locking || zmk_flag_is_locked(flag))) {
        return zmk_flag_deactivate(flag, locking);
    }

    return zmk_flag_activate(flag, locking);
};
