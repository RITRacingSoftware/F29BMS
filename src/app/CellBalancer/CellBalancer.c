#include "common_macros.h"

#include "CellBalancer.h"
#include "ChargeMonitor.h"
#include "FaultManager.h"

// used to keep track of drain status.
static BatteryModel_t prev_model;

bool reading_allowed = false;

void CellBalancer_init(void)
{
    // no cells should be draining on startup
    for (int i = 0; i < NUM_SERIES_CELLS; i++)
    {
        prev_model.cells[i].is_draining = false;
    }
}

void CellBalancer_stage_cell_draining(BatteryModel_t* bm)
{
    // keep track of per iteration drain failure detection
    bool drain_failure = false;

    for (int i = 0; i < NUM_SERIES_CELLS; i++)
    {
        // check if this cell should be draining and isnt
        if (bm->cells[i].is_draining != prev_model.cells[i].is_draining)
        {
            FaultManager_set_fault_active(FaultCode_DRAIN_FAILURE, &i);
            drain_failure = true;
        }
        
        if (ChargeMonitor_is_balancing_allowed())
        {
            float cell_diff_V = bm->cells[i].voltage - bm->smallest_V;
          // bm->cells[i].is_draining = true; 
            //start some cells draining
            if (FLOAT_GT(cell_diff_V, DIFF_CORRECTION_THRESHOLD_V, VOLTAGE_TOLERANCE))
            {
                // cell is falling behind, request drain
                bm->cells[i].is_draining = true;
            }
            else if (FLOAT_GT_EQ(bm->cells[i].voltage, CHARGED_CELL_V, VOLTAGE_TOLERANCE))
            {
                // charging is supposed to happen, but this cell needs to be drained first
                bm->cells[i].is_draining = true;
            }
            else
            {
                // stop some cells draining
                if (bm->cells[i].is_draining)
                {
                    if (FLOAT_LT_EQ(bm->cells[i].voltage, CHARGED_CELL_V - BALANCING_HISTERESIS_V, VOLTAGE_TOLERANCE))
                    {
                        bm->cells[i].is_draining = false;
                    }
                }
                else
                {
                    bm->cells[i].is_draining = false;      
                }
            }
            //bm->cells[i].is_draining = true;
            reading_allowed = false;
        }
        else
        {
            bm->cells[i].is_draining = false;
            reading_allowed = true;
        }
    }

    if (drain_failure == false)
    {
        FaultManager_clear_fault(FaultCode_DRAIN_FAILURE);
    }

    // save off the new expected drain states
    prev_model = *bm;
}

bool CellBalancer_reading_allowed(void)
{
    return reading_allowed;
}