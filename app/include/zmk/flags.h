/*
 * Copyright (c) 2025 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/sys/util.h>
#include <zephyr/devicetree.h>

/**
 * @brief A layer ID is a stable identifier to refer to a layer, regardless of ordering.
 */
typedef uint8_t zmk_flag_id_t;

/**
 * @brief A way to return/reference a missing or invalid layer ID
 */
#define ZMK_FLAG_ID_INVAL UINT8_MAX

/**
 * @brief A bitfield representing the state of all layers.
 */
typedef uint32_t zmk_flag_state_t;

#define ZMK_BOTTOM_LAYER_INDEX 0

#define ZMK_KEYMAP_LAYERS_FOREACH(_fn)                                                             \
    COND_CODE_1(IS_ENABLED(CONFIG_ZMK_STUDIO), (DT_FOREACH_CHILD(DT_INST(0, zmk_keymap), _fn)),    \
                (DT_FOREACH_CHILD_STATUS_OKAY(DT_INST(0, zmk_keymap), _fn)))

#define ZMK_KEYMAP_LAYERS_FOREACH_SEP(_fn, _sep)                                                   \
    COND_CODE_1(IS_ENABLED(CONFIG_ZMK_STUDIO),                                                     \
                (DT_FOREACH_CHILD_SEP(DT_INST(0, zmk_keymap), _fn, _sep)),                         \
                (DT_FOREACH_CHILD_STATUS_OKAY_SEP(DT_INST(0, zmk_keymap), _fn, _sep)))

#define ZMK_LAYER_CHILD_LEN_PLUS_ONE(node) 1 +

#define ZMK_KEYMAP_LAYERS_LEN (ZMK_KEYMAP_LAYERS_FOREACH(ZMK_LAYER_CHILD_LEN_PLUS_ONE) 0)

zmk_flag_state_t zmk_flag_states(void);
zmk_flag_state_t zmk_flag_locks(void);

bool zmk_flag_active_in_states(zmk_flag_id_t layer, zmk_flag_state_t state_to_test);
int zmk_flag_activate(zmk_flag_id_t layer, bool locking);
int zmk_flag_deactivate(zmk_flag_id_t layer, bool locking);
int zmk_flag_toggle_state(zmk_flag_id_t layer, bool locking);
bool zmk_flag_is_active(zmk_flag_id_t layer);
bool zmk_flag_is_locked(zmk_flag_id_t layer);
