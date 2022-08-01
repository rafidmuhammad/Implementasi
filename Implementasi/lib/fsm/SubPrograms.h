#include <definitions.h>

extern void ChargingFSM(short *state, float *DayaPanel, float *DayaPanel_Old, float *SOC, float *TeganganPanel, float *TeganganPanel_Old, float *duty);
extern void fsm_init(short *charging_state);

extern void NetSOC(float *Arus, float *CurrentSOC);
