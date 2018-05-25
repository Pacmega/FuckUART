void receiving()
{
  if (receivingData)
  {
    if (SampleCounter == sampleAmount + 1 && !TakenAllSamples)
    {
      SampleCounter = 0;
      TakenAllSamples = true;
    }
    else if (!TakenAllSamples)
    {
      takeSample();
      SampleCounter++;
    }
  }
  else
  {
    checkForFallingEdge();
  }
}

void checkForFallingEdge()
{
  if (readRecvPin() == LOW)
  {
    receivingData = true;
    DataArray[0] = 0;
    DataArrayCounter++;
  }
}

void takeSample()
{
  if (readRecvPin() == HIGH)
  {
    SampleArray[SampleCounter] = 1;
  }
  else
  {
    SampleArray[SampleCounter] = 0;
  }
}

unsigned char calculateSampleResult()
{
  // BAS - If it works, replace this by our own majority
  if ((SampleArray[0] + SampleArray[1] + SampleArray[2] + SampleArray[3] + SampleArray[4] + SampleArray[5] + SampleArray[6] + SampleArray[7] + SampleArray[8]) <= 5 )
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

char convertToChar()
{
  char receivedChar = '\0';
  for (int i = 0; i < sampleAmount; i++)
  {
    receivedChar |= (DataArray[sampleAmount - i] << (7-i));
  }
  return receivedChar;
}

void sampling()
{
  if (TakenAllSamples)
  {
    if (DataArrayCounter == MaxArrayLength)
    {
      FixTheShit(); // BAS - Needs to be implemented somewhere else
      Serial.print(convertToChar());
      DataArrayCounter = 0;
      TakenAllSamples = false;
      receivingData = false;
    }
    else
    {
      DataArray[DataArrayCounter] = calculateSampleResult();
      DataArrayCounter++;
      TakenAllSamples = false;
    }
  }
}

void FixTheShit()
{
  // BAS - Move this to a function that doesn't show this fix, but too obvious
  byte savebyte = DataArray[0];
  for (int i = 0; i < MaxArrayLength; i++)
  {
    DataArray[i] = DataArray[i + 1];
  }
  DataArray[11] = savebyte;
}

unsigned char readRecvPin() // Returns either 0 or 1
{
  return ((PIND & B00001000) >> 3);
}