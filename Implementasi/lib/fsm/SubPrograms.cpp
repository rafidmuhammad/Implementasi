#include <definitions.h>
// FSM Baterai
//--------------------------------------------------------------------------------------------------
void fsm_init(short *charging_state)
{
    *charging_state = ON;
}

void ChargingFSM(short *state, float *DayaPanel, float *DayaPanel_Old, float *SOC, float *TeganganPanel, float *TeganganPanel_Old, float *duty)
{
    float delta = 0.1;
    int DayaPanelMinimum = 0;
    *DayaPanel *= -1;

    switch (*state)
    {
    case ON:
        if (*SOC < 99.0)
        {
            *state = Charging;
            *duty = 50; // starting duty cycle
        }
        else
        {
            *state = OFF;
        }

        break;
    case Charging:
        if (*SOC >= 99.0)
        {
            *state = OFF;
        }
        else if (*SOC < 99.0)
        {
            if ((*DayaPanel - *DayaPanel_Old) != 0)
            {
                if (*DayaPanel - *DayaPanel_Old > 0)
                {
                    if (*TeganganPanel - *TeganganPanel_Old > 0)
                    {
                        *duty -= delta;
                    }
                    else
                    {
                        *duty += delta;
                    }
                }
                else
                {
                    if (*TeganganPanel - *TeganganPanel_Old > 0)
                    {
                        *duty += delta;
                    }
                    else
                    {
                        *duty -= delta;
                    }
                }
            }
            *TeganganPanel_Old = *TeganganPanel;
            *DayaPanel_Old = *DayaPanel;
        }
        break;
    case OFF:
        // Matikan MOSFET / PWM converter  = 0
        *duty = 0;
        if (*DayaPanel > DayaPanelMinimum && *SOC < 95.0)
        {
            *state = ON;
        }
        break;
    default:
        // Matikan Mosfet / PWM = 0
        *duty = 0;
        break;
    }
}
//--------------------------------------------------------------------------------------------------

// NETSOC
//--------------------------------------------------------------------------------------------------
void NetSOC(float *Arus, float *CurrentSOC)
{
    float NetAh = (*Arus * 0.05) / 3600.0; // net coulomb dalam jam
    // 0.05 dari sampling frequency 20 Hz
    *CurrentSOC += ((NetAh / 60.0) * 100.0); // dalam persen
}
//--------------------------------------------------------------------------------------------------
