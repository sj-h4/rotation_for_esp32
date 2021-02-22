hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t timerCounter = 0;

volatile unsigned int nw = 0;
volatile unsigned int od = 0;
volatile unsigned int cnt = 0;
volatile double cnt_t = 0;
volatile double rpm = 0;

//スリット数
volatile unsigned int slit_num = 36;

//インターバル（単位はms）
volatile unsigned int interval = 500;

//校正用の時速
//volatile double sp = 0;

void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  timerCounter++;
  //lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  //xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
}

void int_cnt() {
  cnt++;
}

void cul() {
  nw = cnt;
  cnt_t = (nw + od) / 2;
  od = nw;
  cnt = 0;
}

void setup() {
  Serial.begin(115200);
  //while(!Serial);
  pinMode(33, INPUT);
  Serial.println("RMP");
  attachInterrupt(33, int_cnt, RISING);
  timer = timerBegin(0, 80, true);
  
  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);
  
  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, interval * 100, true);
  
  // Start an alarm
  timerAlarmEnable(timer);

}

void loop() {
  if (timerCounter > 0) {
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    timerCounter = 0;
    portEXIT_CRITICAL(&timerMux);

    cul();
    
    rpm = cnt_t * 1000 * 60 / slit_num / interval;
    Serial.print("cnt_t = ");
    Serial.print(cnt_t);
    Serial.print("  RPM = ");
    Serial.println(rpm);
    //Serial.print(rpm * 2105 * 60 / 1000000);
  }
}
