/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

/**
 * @brief A layer ID is a stable identifier to refer to a layer, regardless of ordering.
 */
typedef uint8_t zmk_keymap_layer_id_t;

/**
 * @brief A way to return/reference a missing or invalid layer ID
 */
#define ZMK_KEYMAP_LAYER_ID_INVAL UINT8_MAX

/**
 * @brief A layer index is a identifier to refer layer at a particular position in the order.
 */

typedef uint8_t zmk_keymap_layer_index_t;

typedef uint32_t zmk_keymap_layers_state_t;

zmk_keymap_layer_id_t zmk_keymap_layer_default(void);
zmk_keymap_layers_state_t zmk_keymap_layer_state(void);
zmk_keymap_layers_state_t zmk_keymap_layer_locks(void);

bool zmk_device_state_active_with_state(zmk_keymap_layer_id_t state,
                                        zmk_keymap_layers_state_t state_to_test);

bool zmk_device_state_active(zmk_keymap_layer_id_t state);

bool zmk_device_state_locked(zmk_keymap_layer_id_t state);

int zmk_device_state_activate(zmk_keymap_layer_id_t state, bool locking);

int zmk_device_state_deactivate(zmk_keymap_layer_id_t state, bool locking);

int zmk_device_state_toggle(zmk_keymap_layer_id_t state, bool locking);

int zmk_keymap_layer_to(zmk_keymap_layer_id_t state, bool locking);