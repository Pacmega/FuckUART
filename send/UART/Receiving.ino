// heck

bool CheckForStartbit()
{
  if(Read() == 0)
    return true;
    // Startingbit has been found

  return false;
}

char Read()
{
  // TODO: sample 5x; check majority; save bit to buffer
  char sampleCheck = 0;
  for(int j = 0; j < 5; j++) // Sampling
  {
    sampleCheck += readBit();
    // delay a fifth of 1/bitrate
  }

  if(sampleCheck > 2)
    return 1; // the majority is 1, so return a 1
  else
    return 0; // thee majority is 0, so return a 0
  
}

bool checkParity()
{
  // 1. Read + count values of databits
  // 2. Read value of parity bit
  // 3. Check if parity is correct and return true(correct)/false(incorrect)
  int parityChecker = 0;
  for(int i = 1; i <= 8; i++)
  {
    if(receivedByte[i] == 1)
    {
      parityChecker++;
    }
  }
  if(parityMode == evenParityMode)
  {
    if((parityChecker + receivedByte[9])%2 == 0)
      return true;
      // parity is even and dividing by 2 returned 0, so parity is correct
    else
      return false;
      // parity is even and dividing by 2 did not return 0, parity incorrect
  }
  else
  {
    if((parityChecker + receivedByte[9])%2 == 1)
      return true;
      // parity is odd and dividing by 2 returned 1, so parity is correct
    else
      return false;
      // parity is odd and dividing by 2 did not return 1, parity incorrect
  }
}

void removeStopbit()
{
  
}

unsigned char Deserialize()
{
  for(int i = 1; i < 9; i++)
  {
    
  }
}

void writeToBus()
{
  
}

char readBit()
{
  // TODO: Replace magic number by _BV(ALIASofNR)
  return ((PORTD & B00001000) >> 3); // reads register and shifts the bit to the right to return a 1 or 0
}
