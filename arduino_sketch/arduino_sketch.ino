int val;
int LED = 13;

 int direction  = 1;
 int speed  = 2;
 int distance  = 3;

static int k = 0;

void setup()
{
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
}

void loop()
{
  
      
      if (Serial.available()) {
        val = Serial.read();
        // При символе "W" включаем светодиод
        if (val == 'w') {
            digitalWrite(LED, HIGH);
        }
        // При символе "S" выключаем светодиод
        if (val == 's') {
            digitalWrite(LED, LOW);
        }
       }
    
    Serial.write(direction);
    Serial.write(15 + ++k);

    Serial.write(speed);
    Serial.write(55 + ++k);    

    Serial.write(distance);
    Serial.write(117 + ++k * 2);

    delay(300);
}
