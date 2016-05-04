int startTime, waitTime, hubsanWait,finishTime;

long time1 = 5000;
long time2 = 6000;
long time3 = 10000;
long time4 = 15000;
long time5 = 18000;
void loop() {
  Serial.begin(115200);
  A7105_Setup(); //A7105_Reset();
  //Serial.println("Preinit");
  initialize();

  startTime = millis();
  while (1) {
    hubsanWait = hubsan_cb();
    delayMicroseconds(hubsanWait);
    pointToPoint(time1, time2, 200, 220, 128, 150);
    pointToPoint(time2, time3, 250, 220, 128, 150);
    pointToPoint(time3, time4, 40, 150, 128, 150);
    pointToPoint(time4, time5, 0, 150, 128, 150);
  }
}

void pointToPoint(long start_millis, long end_millis, char throttle_val, char roll_val, char pitch_val, char yaw_val)
{
  if(((millis()- startTime)> start_millis) && (((millis()- startTime)< end_millis)))
  {
    throttle = throttle_val;
    rudder = roll_val;
    elevator = pitch_val;
    aileron = yaw_val;
  }
}
