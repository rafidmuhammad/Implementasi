#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <definitions.h>
#include <TimerOne.h>
#include <SubPrograms.h>
#include "ACS712.h"

// Global Variables
//----------------------------------------------------------------------------------------------------------------------
//  Sensor
float TeganganPanel = 0;
float ArusPanel = 0;
float DayaPanel = 0;
float DayaPanelOld = 0;
float TeganganPanelOld = 0;
float TeganganBaterai = 0;
float ArusBaterai = 0;
// SOC
float SOC = 0;
// pwm cuk dan boost
float pwm_converter = 0; // dalam persen
// STATE
short state_charging;
char state_baterai;
// Timer counter
volatile unsigned short counter_20hz = 0;
volatile unsigned short counter_2hz = 0;
volatile unsigned short counter_1hz = 0;
// Time remaining
float waktu = 0; // dalam jam
//
unsigned long previousMillis = 0;
// ACS Sensor
ACS712 ACSPanel(A2, 5.0, 1023.0, 100);
ACS712 ACSBaterai(A6, 5.0, 1023.0, 66);
//----------------------------------------------------------------------------------------------------------------------

// OLED Display
//-----------------------------------------------------------------------------------------------------------------------
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

static const float PROGMEM LUT_SOC[101] = {10, 10.308, 10.592, 10.852, 11.091, 11.308, 11.507, 11.688, 11.852, 12.000, 12.134, 12.254, 12.362, 12.459, 12.545, 12.621, 12.688, 12.747, 12.799, 12.844, 12.883, 12.917, 12.945, 12.969, 12.990, 13.006, 13.020, 13.031, 13.040, 13.047, 13.053, 13.057, 13.060, 13.062, 13.063, 13.064, 13.065, 13.066, 13.066, 13.067, 13.068, 13.069, 13.071, 13.073, 13.075, 13.078, 13.081, 13.084, 13.088, 13.093, 13.098, 13.103, 13.108, 13.114, 13.120, 13.127, 13.133, 13.140, 13.146, 13.153, 13.159, 13.166, 13.172, 13.178, 13.184, 13.189, 13.195, 13.200, 13.204, 13.208, 13.212, 13.215, 13.218, 13.221, 13.223, 13.225, 13.227, 13.229, 13.230, 13.231, 13.233, 13.234, 13.236, 13.238, 13.241, 13.244, 13.248, 13.254, 13.260, 13.268, 13.278, 13.289, 13.303, 13.319, 13.338, 13.361, 13.386, 13.415, 13.449, 13.486, 13.529};

static const unsigned char PROGMEM solar[] =
    {0b11111111, 0b11111111,
     0b10000001, 0b10000001,
     0b10000001, 0b10000001,
     0b10000001, 0b10000001,
     0b11111111, 0b11111111,
     0b10000001, 0b10000001,
     0b10000001, 0b10000001,
     0b10000001, 0b10000001,
     0b11111111, 0b11111111,
     0b10000001, 0b10000001,
     0b10000001, 0b10000001,
     0b10000001, 0b10000001,
     0b11111111, 0b11111111,
     0b10000001, 0b10000001,
     0b10000001, 0b10000001,
     0b10000001, 0b10000001,
     0b11111111, 0b11111111};

static const unsigned char PROGMEM battery[] =
    {0b00000111, 0b11100000,
     0b00000111, 0b11100000,
     0b00011111, 0b11111000,
     0b00010000, 0b00001000,
     0b00010000, 0b00011000,
     0b00010110, 0b00101000,
     0b00010110, 0b01001000,
     0b00010000, 0b10001000,
     0b00010001, 0b00001000,
     0b00010010, 0b00001000,
     0b00010100, 0b11001000,
     0b00011000, 0b11001000,
     0b00010000, 0b00001000,
     0b00010000, 0b00001000,
     0b00010000, 0b00001000,
     0b00010000, 0b00001000,
     0b00011111, 0b11111000};

static const unsigned char PROGMEM TimeLeft[] =
    {0b01111111, 0b11111110,
     0b01000000, 0b00000010,
     0b01000000, 0b00000010,
     0b00100000, 0b00000100,
     0b00011111, 0b11111000,
     0b00001111, 0b11110000,
     0b00000111, 0b11100000,
     0b00000011, 0b11000000,
     0b00000001, 0b10000000,
     0b00000010, 0b11000000,
     0b00000100, 0b10100000,
     0b00001000, 0b10010000,
     0b00010000, 0b10001000,
     0b00100001, 0b10000100,
     0b01000011, 0b11000010,
     0b01111111, 0b11111110,
     0b01111111, 0b11111110};

/**
 * @brief Menampilkan string dengan alignment tengah vertikal maupun horizontal
 *
 * @param text menerima string
 */
void oledDisplayCenter(String text)
{
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  oled.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);

  // display on horizontal and vertical center
  oled.clearDisplay(); // clear display
  oled.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
  oled.println(text); // text to display
  oled.display();
}

/**
 * @brief inisialisasi layar OLED
 *
 */
void OledInit()
{
  oled.setCursor(0, 0);
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(2);
  oledDisplayCenter("SolPow");
  delay(1000);
  oled.setTextSize(1);
}

/**
 * @brief menampilkan data ke layar OLED beserta simbol yang terkait
 *
 * @param DayaPanel menerima masukan alamat dari DayaPanel untuk ditampilkan
 * @param SOC menerima masukan alamat dari SOC untuk ditampilkan
 * @param waktu menerima masukan alamat dari waktu charging/discharging untuk ditampilkan
 */
void OledDisplayData(float *DayaPanel, float *SOC, float *waktu)
{
  oled.clearDisplay();
  // daya panel
  oled.drawBitmap(0, 0, solar, 16, 17, WHITE);
  oled.setCursor(43, 5);
  oled.print(*DayaPanel);
  oled.print(" ");
  oled.print("Watt");
  // SOC baterai
  oled.drawBitmap(0, 20, battery, 16, 17, WHITE);
  oled.setCursor(43, 25);
  if (TeganganBaterai >= 10.0)
  {
    oled.print((int)*SOC);
  }
  else if (TeganganBaterai < 10.0)
  {
    oled.print("--");
  }
  oled.print(" ");
  oled.print("%");
  // time remaining
  oled.drawBitmap(0, 40, TimeLeft, 16, 17, WHITE);
  oled.setCursor(43, 45);
  if (TeganganBaterai >= 10)
  {
    if (ArusBaterai > 0)
    {
      *waktu = (KapasitasBaterai* *SOC/100.0) / ArusBaterai;
      oled.print(*waktu, 1);
      oled.print(" Chr.");
    }
    else if (ArusBaterai < 0)
    {
      *waktu = (KapasitasBaterai* *SOC/100.0) / abs(ArusBaterai);
      oled.print(*waktu, 1);
      oled.print(" Dis.");
    }
  }
  else if (TeganganBaterai < 10)
  {
    oled.print("--");
  }
  oled.print(" ");
  oled.print("Hr");
  oled.display();
}
//-----------------------------------------------------------------------------------------------------------------------

/**
 * @brief Melakukan pembacaan nilai analog untuk suatu kanal tertentu dengan simple averaging sebanyak 10 nilai
 *
 * @param kanal
 * @return int
 */
int SensorRead(int kanal)
{
  int jumlah = 0;
  int temp;
  int i;

  for (i = 0; i < 10; i++)
  {
    temp = analogRead(kanal);
    jumlah += temp;
    delayMicroseconds(50);
  }
  return (jumlah / 10);
}

/**
 * @brief memberikan keluaran nilai pwm
 *
 * @param kanal
 * @param pwm
 */
void set_pwm_duty(int kanal, float *pwm)
{

  if (*pwm > PWM_MAX)
  {
    *pwm = PWM_MAX;
  }
  else if (*pwm < PWM_MIN)
  {
    *pwm = PWM_MIN;
  }
  if (*pwm < PWM_MAX)
  {
    Timer1.pwm(kanal, (PWM_TOP * (long)*pwm / 100), 100); // use Timer1 routine to set pwm duty cycle at 100uS period
  }
  else if (*pwm == PWM_MAX)
  {                                         // if pwm set to 100% it will be on full but we have
    Timer1.pwm(kanal, (PWM_TOP - 1), 1000); // keep switching so set duty cycle at 99.9% and slow down to 1000uS period
  }
}

/**
 * @brief ISR yang akan melakukan pembacaan arus tiap 0.05 s; pembacaan tegangan panel tiap 0.5 s; pembaruan daya panel dan algortima MPPT tiap 1 s
 *
 */
void Sampling(void)
{
  counter_20hz++;
  counter_2hz++;
  counter_1hz++;
  if (counter_20hz == 500)
  {
    // ISR at 10Hz
    ArusPanel = ACSPanel.mA_DC() / 1000.0;     // 20 A
    ArusBaterai = ACSBaterai.mA_DC() / 1000.0; // masuk
    NetSOC(&ArusBaterai, &SOC);
    counter_20hz = 0;
  }
  if (counter_2hz == 5000)
  {
    // ISR at 2 Hz
    TeganganPanel = ((SensorRead(KanalTeganganPanelPositif) * 5.0 / 1023.0) * 11) - 1.45; // (42900 / 3900)   Resistansi 3.9K dan 39K
    counter_2hz = 0;
  }
  if (counter_1hz == 10000)
  {
    // ISR at 1 Hz
    DayaPanel = ArusPanel * TeganganPanel;
    ChargingFSM(&state_charging, &DayaPanel, &DayaPanelOld, &SOC, &TeganganPanel, &TeganganPanelOld, &pwm_converter);
    set_pwm_duty(PWMConverter, &pwm_converter);
    counter_1hz = 0;
  }
}

/**
 * @brief melakukan inisialisasi SOC baterai berdasarkan tegangan yang terbaca saat Open Circuit
 *
 * @param Tegangan_Baterai menerima masukan alamat tegangan baterai
 * @param SOC menerima masukan alamat SOC
 */
void SOC_init(float *Tegangan_Baterai, float *SOC)
{
  for (int i = 0; i < 101; i++)
  {
    if ((*Tegangan_Baterai >= pgm_read_float_near(LUT_SOC + i)) && (*Tegangan_Baterai <= pgm_read_float_near(LUT_SOC + (i + 1))))
    {
      *SOC = i + 2;
      break;
    }
    else
    {
      *SOC = 0;
    }
  }
}

void setup()
{
  Serial.begin(9600);
  //Sementara
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  digitalWrite(11,HIGH);
  digitalWrite(12,LOW);
  //
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  digitalWrite(7,HIGH);
  digitalWrite(6,LOW);
  // timer
  Timer1.initialize(100); // inisialisasi timer 10KHz
  Timer1.pwm(PWMConverter, 0);
  Timer1.attachInterrupt(Sampling);
  // ACS midpoint
  ACSPanel.autoMidPoint();
  ACSBaterai.autoMidPoint();
  // Initial SOC
  // putusin relay
  TeganganBaterai = ((SensorRead(KanalTeganganBaterai) * 5.0 / 1023.0) * 22700.0 / 4700.0)+0.36; // (22700 / 4700) Resistansi 18k dan 4k7
  // sambung relay
  state_baterai = Sambung;
  //  fsm
  fsm_init(&state_charging);
  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    while (true)
      ;
  }
  delay(2000); // wait for initializing
  OledInit();
  SOC_init(&TeganganBaterai, &SOC);
}

/**
 * @brief menyambungkan beban ketika SOC baterai sudah berada di atas 10% dan memutus ketika SOC dibawah 5%
 *
 */
void load_switch()
{
  if (SOC >= 10)
  {
    digitalWrite(Relay1, LOW);
  }
  else if (SOC <= 5)
  {
    digitalWrite(Relay1, HIGH);
  }
}

void print_data()
{

  Serial.print("Tegangan Panel = ");
  Serial.print(TeganganPanel);
  Serial.print(" ");

  Serial.print("Arus Panel = ");
  Serial.print(ArusPanel);
  Serial.print(" ");

  Serial.print("Daya Panel = ");
  Serial.print(DayaPanel);
  Serial.print(" ");

  Serial.print("Tegangan Baterai = ");
  Serial.print(TeganganBaterai);
  Serial.print(" ");

  Serial.print("Arus Baterai  = ");
  Serial.print(ArusBaterai);
  Serial.print(" ");

  Serial.print("state = ");
  Serial.print(state_charging);
  Serial.print(" ");

  Serial.print("PWM Converter (%) = ");
  Serial.print(pwm_converter);
  Serial.println(" ");
}


void State_baterai(char *state)
{
  switch (*state)
  {
  case Sambung:
    if (TeganganBaterai < 10)
    {
      // putusin relay
      *state = Putus;
    }
    break;
  case Putus:
    if (TeganganBaterai > 10)
    {
      SOC_init(&TeganganBaterai, &SOC);
      // sambungkan relay
      *state = Sambung;
    }
    break;
  default:
    break;
  }
}

void loop()
{

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 3000)
  {
    TeganganBaterai = ((SensorRead(KanalTeganganBaterai) * 5.0 / 1023.0) * 22700.0 / 4700.0)+0.36; // (22700 / 4700) Resistansi 18k dan 4k7
    State_baterai(&state_baterai);
    OledDisplayData(&DayaPanel, &SOC, &waktu);
    load_switch();
    previousMillis = currentMillis;
  }

  print_data();
}
