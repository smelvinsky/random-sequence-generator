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

int min = 1024;
int max = 0;
uint8_t val2;

void loop()
{
  //val = analogRead(A2);
  //Serial.write(val);
  //val = analogRead(A3);
  //Serial.write(val);
  val2 = analogRead(A2);
  if (val2 > max)
  {
    max = val2;
  }
  if (val2 < min)
  {
    min = val2;
  }
  Serial.print("Min: ");
  Serial.print(min);
  Serial.print("Max: ");
  Serial.print(max);
  Serial.print("Value: ");
  Serial.print(val2);
  Serial.print("\n");
}

