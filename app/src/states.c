/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/keymap.h>
#include <zmk/states.h>
#include <zmk/events/device_state_changed.h>

zmk_keymap_layers_state_t _zmk_keymap_layer_locks = 0;
zmk_keymap_layers_state_t _zmk_keymap_layer_state = 0;
zmk_keymap_layer_id_t _zmk_keymap_layer_default = 0;

#define ASSERT_LAYER_VAL(_layer, _fail_ret)                                                        \
    if ((_layer) >= ZMK_KEYMAP_LAYERS_LEN) {                                                       \
        return (_fail_ret);                                                                        \
    }

static inline int set_layer_state(zmk_keymap_layer_id_t state_id, bool state, bool locking) {
    int ret = 0;
    if (state_id >= ZMK_KEYMAP_LAYERS_LEN) {
        return -EINVAL;
    }

    // Default layer should *always* remain active
    if (state_id == _zmk_keymap_layer_default && !state) {
        return 0;
    }

    // Non-forcing disables should not change a locked active layer
    if (!locking && !state && (_zmk_keymap_layer_locks & BIT(state_id))) {
        return ret;
    }

    zmk_keymap_layers_state_t old_state = _zmk_keymap_layer_state;
    zmk_keymap_layers_state_t old_locks = _zmk_keymap_layer_locks;
    WRITE_BIT(_zmk_keymap_layer_state, state_id, state);
    if (locking) {
        WRITE_BIT(_zmk_keymap_layer_locks, state_id, state);
    }
    // Don't send state changes unless there was an actual change
    if (old_state != _zmk_keymap_layer_state || old_locks != _zmk_keymap_layer_locks) {
        LOG_DBG("layer_changed: layer %d state %d locked %d", state_id, state, locking);

        ret = raise_device_state_changed(state_id, state, locking);
        if (ret < 0) {
            LOG_WRN("Failed to raise device state changed (%d)", ret);
        }
    }

    return ret;
}

zmk_keymap_layer_id_t zmk_keymap_layer_default(void) { return _zmk_keymap_layer_default; }

zmk_keymap_layers_state_t zmk_keymap_layer_state(void) { return _zmk_keymap_layer_state; }

zmk_keymap_layers_state_t zmk_keymap_layer_locks(void) { return _zmk_keymap_layer_locks; }

bool zmk_device_state_active_with_state(zmk_keymap_layer_id_t state,
                                        zmk_keymap_layers_state_t state_to_test) {
    // The default layer is assumed to be ALWAYS ACTIVE so we include an || here to ensure nobody
    // breaks up that assumption by accident
    return (state_to_test & (BIT(state))) == (BIT(state)) || state == _zmk_keymap_layer_default;
};

bool zmk_device_state_active(zmk_keymap_layer_id_t state) {
    return zmk_device_state_active_with_state(state, _zmk_keymap_layer_state);
};

bool zmk_device_state_locked(zmk_keymap_layer_id_t state) {
    return zmk_device_state_active_with_state(state, _zmk_keymap_layer_locks);
}

int zmk_device_state_activate(zmk_keymap_layer_id_t state, bool locking) {
    return set_layer_state(state, true, locking);
};

int zmk_device_state_deactivate(zmk_keymap_layer_id_t state, bool locking) {
    return set_layer_state(state, false, locking);
};

int zmk_device_state_toggle(zmk_keymap_layer_id_t state, bool locking) {
    if (zmk_device_state_active(state) && (!locking || zmk_device_state_locked(state))) {
        return zmk_device_state_deactivate(state, locking);
    }

    return zmk_device_state_activate(state, locking);
};

int zmk_keymap_layer_to(zmk_keymap_layer_id_t state, bool locking) {
    for (int i = ZMK_KEYMAP_LAYERS_LEN - 1; i >= 0; i--) {
        zmk_device_state_deactivate(i, locking);
    }

    zmk_device_state_activate(state, locking);

    return 0;
}
