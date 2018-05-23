boolean DetectedFallingEdge()
{
  if (!FallingEdgeDetected && digitalRead(ReceivePin) == LOW)
  {
    //Serial.println("detected");
    FallingEdgeDetected = true;
    DataArray[0] = zerobyte;
    DataArrayCounter++;
    //PrintChar = false;
    return true;
  }
  return FallingEdgeDetected;
}


void TakeSample()
{
  if (digitalRead(ReceivePin) == LOW)
  {
    SampleArray[SampleCounter] = zerobyte;
  }
  if (digitalRead(ReceivePin) == HIGH)
  {
    SampleArray[SampleCounter] = onebyte;
  }
}

byte CalculateSampleResult()
{
  if ((SampleArray[0] + SampleArray[1] + SampleArray[2] + SampleArray[3] + SampleArray[4] + SampleArray[5] + SampleArray[6] + SampleArray[7] + SampleArray[8]) <= 5 )
  {
        return zerobyte;
  }
  else
  {
        return onebyte;
  }
}

void ConvertToChar()
{
  char finalchar = B00000000;
  for (int i = 0; i < 8; i++)
  {
    finalchar = finalchar << 1;
    finalchar = finalchar | DataArray[8 - i];
  }
  Serial.print("char = ");
  Serial.println(finalchar);
}

void printBits(byte myByte){
 for(byte mask = 0x80; mask; mask >>= 1){
   if(mask  & myByte)
       Serial.print('1');
   else
       Serial.print('0');
 }
}

void sampleing()
{
  if (TakenAllSamples)
    {
      if (DataArrayCounter == MaxArrayLength)
      {
        FixTheShit();
        ConvertToChar();
        DataArrayCounter = 0;
        TakenAllSamples = false;
        for (int i = 0; i < MaxArrayLength; i++)
        {
          if (DataArray[i] == zerobyte)
          {
            Serial.print("0");
          }
          if (DataArray[i] == onebyte)
          {
            Serial.print("1");
          }
        }
        Serial.println(" ");
        FallingEdgeDetected = false;
      }
      else
      {
        DataArray[DataArrayCounter] = CalculateSampleResult();
        DataArrayCounter++;
        TakenAllSamples = false;
      }
    }
}

void Receiving()
{
    if (DetectedFallingEdge())
  {
    if (SampleCounter == MaxNumberOfSamples + 1 && !TakenAllSamples)
    {
      SampleCounter = 0;
      TakenAllSamples = true;
    }
    else
    {
      if (!TakenAllSamples)
      {
        TakeSample();
        SampleCounter++;
      }
    }
  }
}
void FixTheShit()
{
  byte savebyte = DataArray[0];
  for (int i = 0; i < MaxArrayLength; i++)
  {
    DataArray[i] = DataArray[i + 1];
  }
  DataArray[11] = savebyte;
}
