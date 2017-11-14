uint8_t val;

void setup()
{
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  
  Serial.begin(9600);
  val = 0;
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB
  }
}

void loop()
{
  val = analogRead(A2);
  Serial.print(val);
  Serial.write(" ");
  val = analogRead(A3);
  Serial.print(val);
  Serial.write("\n");
}

