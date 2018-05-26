void receiving()
{
  if (receivingData) // Check if there is an anomily detected (data transfer)
  {
    if (sampleCounter == sampleAmount + 1 && !takenAllSamples) // If samplecounter has reached the end, set takenAllSamples to true
    {
      sampleCounter = 0;
      takenAllSamples = true;
    }
    else if (!takenAllSamples) // Continue sampling if samplecounter hasn't reached the end yet
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

// To check majority, we'll use 3 characters. Could be more, but this should be fine.
bool checkMajority(unsigned char a, unsigned char b, unsigned char c)
{
  if (a + b + c >= 2)
    return 1; // AKA true

  return 0; // AKA false
}

char convertToChar()
{
  // convert the array into a character
  char receivedChar = '\0'; // \0 is 0x00 in the ascii table.
  for (int i = 0; i < sampleAmount; i++)
  {
    receivedChar |= (dataArray[sampleAmount - i] << (7-i));
  }
  return receivedChar;
}

// Processes the data that's being read on the recvPin
void processRead()
{
    if (dataArrayCounter == maxArrayLength && takenAllSamples) // Print the character after the dataArray has been filled
    {
      Serial.print(convertToChar());
      dataArrayCounter = 0;
      takenAllSamples = false;
      receivingData = false;
    }
    else if (takenAllSamples) // Place the bit into the array after the majority has been checked
    {
      // Don't have to move bits around when you set them on the correct place.
      dataArray[maxArrayLength - dataArrayCounter] = checkMajority(sampleArray[3], sampleArray[4], sampleArray[5]);
      dataArrayCounter++;
      takenAllSamples = false;
    }
}

unsigned char readRecvPin() // Returns either 0 or 1
{
  return ((PIND & B00001000) >> 3);
}