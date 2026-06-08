int loudness;
int loud;
void setup()
{
    Serial.begin(9600);
}

void loop()
{
   loudness=0;
    for(int count = 0; count < 60 ; count++)
    {


       loud= analogRead(A0);
       Serial.println(loud);
       loudness=loudness+loud;
        Serial.println("Loudness=");
        Serial.print(loud);
   delay(1000);
    }
   int l=loudness/60;
   Serial.println("l = ");
    Serial.print(l);
    delay(1000);
}
