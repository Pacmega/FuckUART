void receiving()
{
  if (DetectedFallingEdge())
  {
    if (SampleCounter == sampleAmount + 1 && !TakenAllSamples)
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

boolean DetectedFallingEdge()
{
  if (!FallingEdgeDetected && !digitalRead(recvPin))
  {
    FallingEdgeDetected = true;
    DataArray[0] = zerobyte;
    DataArrayCounter++;
    return true;
  }
  return FallingEdgeDetected;
}

void TakeSample()
{
  if (readRecvPin())
  {
    SampleArray[SampleCounter] = 1;
  }
  else
  {
    SampleArray[SampleCounter] = 0;
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

void FixTheShit()
{
  byte savebyte = DataArray[0];
  for (int i = 0; i < MaxArrayLength; i++)
  {
    DataArray[i] = DataArray[i + 1];
  }
  DataArray[11] = savebyte;
}

unsigned char readRecvPin() // Returns either 0 (false) or 1 (true)
{
  return ((PIND & B00001000) >> 3);
}