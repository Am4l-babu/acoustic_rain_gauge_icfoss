int loudness;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    loudness = analogRead(A0);
    for(count = 0; count < 60 ; count++)
    {
   delay(1000);
    }
    Serial.println(loudness);
    delay(200);
}
