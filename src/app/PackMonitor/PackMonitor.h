#ifndef PACK_MONITOR_H
#define PACK_MONITOR_H

#include "f29BmsConfig.h"
#include "BatteryModel.h"
#include "TempModel.h"

/**
 * Checks battery cell and temperature readings for fault conditions and sets/clears faults. 
 * Implements a time hysteresis for each kind of error.
 * 
 * Some simple definitions:
 * 
 * Fault = an indication from the BMS that it is not operating nominally in a certain way.
 * A fault is the bit in the fault matrix and the alert message.
 * 
 * Fault Condition = a condition that will eventually cause the BMS to fault.
 * A fault condition is something in the physical world that is likely a result of sub-nominal BMS operation.
 * It is expected that a fault condition leads to a fault.
 * 
 * It's important to separate these two definitions when debugging or conversations become confusing AF.

/**
 * Reset all fault timers.
 */
void PackMonitor_init(void);

/**
 * Check for dangerous voltage differences, invalid voltages, etc. in the battery pack.
 * Activate faults if these error conditions are present.
 */
void PackMonitor_validate_battery_model_10Hz(BatteryModel_t* bm);

/**
 * Check for over/invalid temperature events. Activate faults if these conditions are present.
 */
void PackMonitor_validate_temp_model_10Hz(TempModel_t* tm);

#endif // PACK_MONITOR_H