uint8_t val;

void setup()
{
  Serial.begin(9600);
  val = 0;
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB
  }
}

void loop()
{
  while(Serial.availableForWrite() < 8);
  val++;
  Serial.print(val);
  Serial.write(" ");
  val++;
  Serial.print(val);
  Serial.write("\n");
  //delay(10);
}

