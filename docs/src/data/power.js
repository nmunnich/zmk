/*
 * Copyright (c) 2021 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * This file holds all current measurements related to ZMK features and hardware
 * All current measurements are in micro amps. Measurements were taken on a Nordic Power Profiler Kit
 * The test device to get these values was three nice!nanos (nRF52840).
 */

import hardwareMetadata from "@site/src/data/hardware-metadata.json";

export const zmkBase = {
  hostConnection: 23, // How much current it takes to have an idle host connection
  standalone: {
    idle: 0, // No extra idle current
    typing: 315, // Current while holding down a key. Represents polling+BLE notification power
  },
  central: {
    idle: 490, // Idle current for connection to right half
    typing: 380, // Current while holding down a key. Represents polling+BLE notification power
  },
  peripheral: {
    idle: 20, // Idle current for connection to left half
    typing: 365, // Current while holding down a key. Represents polling+BLE notification power
  },
};

function mapPowerSupplyMetadataToRuntimeNames(powerSupply) {
  return {
    type: powerSupply.type,
    outputVoltage: powerSupply.output_voltage,
    quiescentMicroA: powerSupply.quiescent_micro_a,
    efficiency: powerSupply.efficiency,
  };
}

function powerProfileFromMetadata(metadataItem, profile, index) {
  return {
    id: `${metadataItem.id}::${index}`,
    name: `${metadataItem.name}${profile.suffix ? ` ${profile.suffix}` : ""}`,
    powerSupply: mapPowerSupplyMetadataToRuntimeNames(profile.power_supply),
    otherQuiescentMicroA: profile.other_quiescent_micro_a ?? 0,
  };
}

const powerProfiles = hardwareMetadata.flatMap((metadataItem) => {
  if (metadataItem.type !== "board" || !metadataItem.power) {
    return [];
  }

  const profiles = Array.isArray(metadataItem.power)
    ? metadataItem.power
    : [metadataItem.power];

  return profiles.map((profile, index) =>
    powerProfileFromMetadata(metadataItem, profile, index)
  );
});

export const zmkBoards = Object.fromEntries(
  powerProfiles.map((profile) => [profile.id, profile])
);

/**
 * ZMK board power measurements
 *
 * Board and shield power usage is sourced from the hardware metadata files.
 * Power supply can be an LDO or switching.
 */

export const underglowPower = {
  firmware: 60, // ZMK power usage while underglow feature is turned on (SPIM mostly)
  ledOn: 20000, // Estimated power consumption of a WS2812B at 100% (can be anywhere from 10mA to 30mA)
  ledOff: 460, // Quiescent current of a WS2812B
};

export const backlightLEDs = {
  White: 3.2,
  Blue: 3.0,
  Green: 2.2,
  Yellow: 2.1,
  Red: 1.8,
};

export const backlightPower = {
  pwmPower: 510, // Estimated power consumption of PWM module
};

export const displayPower = {
  // Based on GoodDisplay's 1.02in epaper
  EPAPER: {
    activePercent: 0.05, // Estimated one refresh per minute taking three seconds
    active: 1500, // Power draw during refresh
    sleep: 5, // Idle power draw of an epaper
  },
  // 128x32 SSD1306
  OLED: {
    activePercent: 0.5, // Estimated sleeping half the time (based on idle)
    active: 10000, // Estimated power draw when about half the pixels are on
    sleep: 7, // Deep sleep power draw (display off)
  },
  // Based on the nice!view using Sharp's LS011B7DH01
  NICEVIEW: {
    activePercent: 0.01, // Estimated two refreshes per second taking five milliseconds each
    active: 1425, // Power draw during refresh (225uA display + 1200uA SPIM)
    sleep: 1, // Idle power draw of the display
  },
};
