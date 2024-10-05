// PROJECT  : charlieplexClock
// PURPOSE  : Create a clock using CharliePlexing technique with LED display and to act as a timer
// AUTHOR   : Chance Hattrick
// COURSE   : ICS3U-E
// DATE     : 2024 08 31
// MCU      : 328P (Nano)
// STATUS   : Working
// REFERENCE: http://darcy.rsgc.on.ca/ACES/TEI4M/Tasks.html#CharlieClock  

// Define the pinouts for the 12 CharliePlex Pins 
#define P1 A3
#define P2 A2
#define P3 A1
#define P4 A0
#define P5 12
#define P6 11
#define P7 8
#define P8 7
#define P9 6
#define P10 5
#define P11 4
#define P12 3

// Define the pinouts for the Timer Inputs
#define TimerInit A4   // Timer initialization input pin
#define TimerHour 10   // Input pin for setting hours
#define TimerMin 9     // Input pin for setting minutes
#define TimerSec 2     // Input pin for setting seconds

// Pack pins into an array for easier manipulation
uint8_t pins[] = { P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12 };
#define numPins sizeof(pins)

// Variables for tracking time (seconds, minutes, hours)
uint8_t sec = 0;
uint8_t min = 0;
uint8_t hour = 0;

// Variables for tracking the timer countdown values
uint8_t Timsec = 0;
uint8_t Timemin = 0;
uint8_t Timhour = 0;
uint8_t startTimsec = 0;
uint8_t startTimemin = 0;
uint8_t startTimhour = 0;
uint8_t TimerOn = 0;   // Flag to check if timer is running

// Define a structure for each LED with its anode and cathode pins
struct LED {
  uint8_t anode;
  uint8_t cathode;
};

// Arrays to represent the LED positions for hours, minutes, and seconds
// Each pair (anode, cathode) defines an LED
LED hours[] = {
  { P9, P1 }, { P10, P1 }, { P11, P1 }, { P12, P1 }, { P12, P11 }, { P5, P1 },
  { P4, P1 }, { P3, P1 }, { P2, P1 }, { P6, P1 }, { P7, P1 }, { P8, P1 }
};

LED minutes[] = {
  {P8,P2}, {P9,P2}, {P10,P2}, {P11,P2}, {P12,P2}, {P1,P12}, {P2,P12}, {P3,P12}, {P4,P12}, {P5,P12}, {P6,P12}, 
  {P7,P12}, {P8,P12}, {P9,P12}, {P10,P12}, {P11,P12}, {P10,P11}, {P9,P11}, {P8,P11}, {P7,P11}, {P6,P11}, {P5,P11},
  {P4,P11}, {P3,P11}, {P2,P11}, {P1,P11}, {P12,P10}, {P11,P10}, {P9,P10}, {P8,P10}, {P7,P10}, {P6,P10}, {P5,P10}, 
  {P4,P10}, {P3,P10}, {P2,P10}, {P1,P10}, {P7,P4}, {P6,P4}, {P5,P4}, {P3,P4}, {P2,P4}, {P1,P4},
  {P1,P3}, {P2,P3}, {P4,P3}, {P5,P3}, {P6,P3}, {P7,P3}, {P8,P3}, {P9,P3}, {P10,P3}, {P11,P3}, 
  {P12,P3}, {P1,P2}, {P3,P2}, {P4,P2}, {P5,P2}, {P6,P2}, {P7,P2}
};

LED seconds[] = {
  { P1, P5 }, { P2, P5 }, { P3, P5 }, { P4, P5 }, { P6, P5 }, { P7, P5 }, { P8, P5 }, { P9, P5 }, { P10, P5 }, { P11, P5 }, { P12, P5 },
  { P8, P4 }, { P9, P4 }, { P10, P4 }, { P11, P4 }, { P12, P4 }, { P12, P9 }, { P11, P9 }, { P10, P9 }, { P8, P9 }, { P7, P9 }, { P6, P9 }, { P5, P9 },
  { P4, P9 }, { P3, P9 }, { P2, P9 }, { P1, P9 }, { P12, P8 }, { P11, P8 }, { P10, P8 }, { P9, P8 }, { P7, P8 }, { P6, P8 }, { P5, P8 }, { P4, P8 }, { P3, P8 },
  { P2, P8 }, { P1, P8 }, { P12, P7 },  { P11, P7 }, { P10, P7 }, { P9, P7 }, { P8, P7 },  { P6, P7 }, { P5, P7 }, { P4, P7 }, { P3, P7 }, { P2, P7 }, 
  { P1, P7 }, { P1, P6 }, { P2, P6 }, { P3, P6 }, { P4, P6 }, { P5, P6 },  { P7, P6 }, { P8, P6 },  { P9, P6 }, { P10, P6 },  { P11, P6 }, { P12, P6 }
};

// Calculate the total number of LEDs
uint8_t numLEDs = sizeof(minutes) / sizeof(LED);

// Simple variable for looping through LEDs
uint8_t i = 0;

// Function to turn on a specific hour LED
void turnOnHour1() {
  pinMode(P11, OUTPUT);
  pinMode(P12, OUTPUT);
  digitalWrite(P11, HIGH);  // Anode side of LED
  digitalWrite(P12, LOW);   // Cathode side of LED
}

// Initialize the clock
void setup() {
  Serial.begin(9600);
  pinMode(TimerHour, INPUT);
  pinMode(TimerMin, INPUT);
  pinMode(TimerSec, INPUT);
}

// Function to light up the correct hour LED
void gethour(uint8_t hour){
  uint8_t displayHour = hour;
  pinMode(hours[displayHour].anode,OUTPUT);
  pinMode(hours[displayHour].cathode,OUTPUT);
  digitalWrite(hours[displayHour].anode, HIGH);
  digitalWrite(hours[displayHour].cathode, LOW);
}

// Function to light up the correct minute LED
void getminute(uint8_t min){
  uint8_t current_min = min;
  pinMode(minutes[current_min].anode,OUTPUT);
  pinMode(minutes[current_min].cathode,OUTPUT);
  digitalWrite(minutes[current_min].anode, HIGH);
  digitalWrite(minutes[current_min].cathode, LOW);
}

// Function to light up the correct second LED
void getsecond(uint8_t sec){
  uint8_t current_sec = sec;
  pinMode(seconds[current_sec].anode,OUTPUT);
  pinMode(seconds[current_sec].cathode,OUTPUT);
  digitalWrite(seconds[current_sec].anode, HIGH);
  digitalWrite(seconds[current_sec].cathode, LOW);
} 

// Function to reset all pins, turning off all LEDs
void reset(){
  for (uint8_t pin = 0; pin < numPins; pin++)
   pinMode(pins[pin], INPUT);
}

// Function to update the clock display in sequence (hours, minutes, seconds)
void CharlieClock(){
  for (int refreshCount = 0; refreshCount < 200; refreshCount++) {
    gethour(hour);   // Display hour
    delay(2);
    reset();
    getminute(min);  // Display minute
    delay(2);
    reset();
    getsecond(sec);  // Display second
    delay(2);
    reset();
    delay(1);
  }

  // Update time after each display loop
  if(sec < 59){
    sec++;
  } else {
    sec = 0;
    if (min < 59) {
      min++;
    } else {
      min = 0;
      if (hour < 11) {
        hour++;
      } else {
        hour = 0;
      }
    }
  }
}

// Function to set up a countdown timer
void TimerSetup(){
  TimerOn = 1;  // Enable the timer mode

  // Check and increment the timer hour, minute, and second inputs
  if (digitalRead(TimerHour) == HIGH){
    Timhour = (Timhour + 1) % 12;
    delay(150);
  }
  if (digitalRead(TimerMin) == HIGH){
    Timemin = (Timemin + 1) % 60;
    delay(150);
  }
  if (digitalRead(TimerSec) == HIGH){
    Timsec = (Timsec + 1) % 60;
    delay(150);
  }
}

// Countdown mode for the timer
void TimerMode(){
  for (int refreshCount = 0; refreshCount < 200; refreshCount++) {
    gethour(Timhour);    // Display hour
    delay(2);
    reset();
    getminute(Timemin);  // Display minute
    delay(2);
    reset();
    getsecond(Timsec);   // Display second
    delay(2);
    reset();
    delay(1);
  }

  // Update the countdown time
  if (Timsec > 0){
    Timsec--;
  } else if (Timsec == 0) {
    Timsec = 59;
    if (Timemin > 0){
      Timemin--;
    } else if (Timemin == 0) {
      Timemin = 59;
      if (Timhour > 0){
        Timhour--;
      } else if (Timhour == 0) {
        TimerOn = 0;  // Stop the timer once it reaches zero
      }
    }
  }
}

// Main loop
void loop() {
  // If the timer input pin is set to initialize the timer
  if (digitalRead(TimerInit) == HIGH && TimerOn == 0){
    TimerSetup();  // Enter timer setup mode
  }

  // If the timer is active, run the countdown
  if (TimerOn == 1){
    TimerMode();
  } else {
    // Otherwise, run the clock
    CharlieClock();
  }
}
