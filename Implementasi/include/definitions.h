// State FSM
#define ON 101
#define Charging 102
#define OFF 103
//state Baterai
#define Sambung 0
#define Putus 1

// PIN MAP
//  A0 Tegangan Panel Positif
//  A2 Arus Panel
//  A4 SDA PCF8574
//  A5 SCL PCF8574
//  A6 Arus Baterai
//  A7 Tegangan Baterai

#define KanalTeganganPanelPositif A0
#define KanalArusPanel A2
#define KanalTeganganBaterai A7
#define KanalArusBaterai A6
#define PWMConverter 9
#define Relay1 8 // beban
//#define Relay2 7 // sebelum baterai
//
#define PWM_MAX 90
#define PWM_MIN 0
#define PWM_TOP 1023
//
#define LVD 11.2                 // Volt
#define RCV 12                   // volt
#define TeganganBateraiTinggi 12 // Volt
// OLED
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
// Baterai
#define KapasitasBaterai 60.0 // dalam Ah
