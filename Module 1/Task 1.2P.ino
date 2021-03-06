void setup()
{
	pinMode(2, INPUT);
	pinMode(13, OUTPUT);
	Serial.begin(9600);
  
  	//Interrupt when the input changes
  	attachInterrupt(digitalPinToInterrupt(2), motion, CHANGE);
}


void loop()
{
  	delay(100);
}
                    

void motion()
{
  	if (digitalRead(2) == HIGH)
  	{
    	Serial.println("Motion Detected");;
     	digitalWrite(13, HIGH);
    }
    

  	else 
    {
    	Serial.println("No Motion Detected");
     	digitalWrite(13, LOW);
    }
}