int loudness;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    loudness = analogRead(A0);
    Serial.println(loudness);
    delay(200);
}
