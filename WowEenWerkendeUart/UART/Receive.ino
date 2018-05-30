void receiving()
{
  if (receivingData)
  {
    if (sampleCounter == sampleAmount + 1 && !takenAllSamples)
    {
      sampleCounter = 0;
      takenAllSamples = true;
    }
    else if (!takenAllSamples)
    {
      sampleArray[sampleCounter] = readRecvPin();
      sampleCounter++;
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
    dataArray[0] = 0;
    dataArrayCounter++;
  }
}

void takeSample()
{
  if (readRecvPin() == HIGH)
  {
    sampleArray[sampleCounter] = 1;
  }
  else
  {
    sampleArray[sampleCounter] = 0;
  }
}

unsigned char checkMajority()
{
  if (sampleArray[3] + sampleArray[4] + sampleArray[5] >= 2 )
    return 1;
  return 0;
}

// Processes the data that's being read on the recvPin 
void processRead()
{
  if (dataArrayCounter == maxArrayLength && takenAllSamples)
  {
    changingBitOrder(); 
    Serial.print(convertToChar());
    dataArrayCounter = 0;
    takenAllSamples = false;
    receivingData = false;
  }
  else if (takenAllSamples)
  {
    dataArray[dataArrayCounter] = checkMajority();
    dataArrayCounter++;
    takenAllSamples = false;
  }
}

char convertToChar()
{
  char receivedChar = '\0'; // \0 is 0x00 in the ascii table.
  for (int i = 0; i < sampleAmount; i++)
  {
    receivedChar |= (dataArray[sampleAmount - i] << (7-i));
  }
  return receivedChar;
}
 
// For some strange reason, our first bit is always placed in the position of the last...
void changingBitOrder()
{
  byte savebyte = dataArray[0];
  for (int i = 0; i < maxArrayLength; i++)
  {
    dataArray[i] = dataArray[i + 1];
  }
  dataArray[11] = savebyte;
}

unsigned char readRecvPin() // Returns either 0 or 1
{
  return ((PIND & B00001000) >> 3);
}