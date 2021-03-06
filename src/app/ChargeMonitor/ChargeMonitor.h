#ifndef CHARGE_MONITOR_H
#define CHARGE_MONITOR_H

#include <stdbool.h>
#include "BatteryModel.h"

/**
 * Charging has a simple two way communication interface.
 * The charge enable line is asserted by the BMS and must be high for charging to commence.
 * The charger available line is pulled to 0V when the charger is connected to the BMS, and remains at Vdd otherwise.
 */

/**
 * Start off the charge enable line in a safe state.
 */
void ChargeMonitor_init(void);

/**
 * Request charging if the charger is connected.
 * Check the average cell voltage in the battery pack.
 * Once it crosses the fully charged threshold, request to stop charging.
 * It's the PackMonitor responsibility to prevent overcharging.
 */
void ChargeMonitor_1Hz(BatteryModel_t* bm);

/**
 * Return true if the charger is connected, false otherwise.
 */
bool ChargeMonitor_charger_available(void);

/**
 * Return true if charging has been requested, false otherwise.
 */
bool ChargeMonitor_is_charging(void);

/**
 * Return true if charging has stopped to allow balancing, false otherwise.
 */
bool ChargeMonitor_is_balancing_allowed(void);

#endif // CHARGE_MONITOR_H