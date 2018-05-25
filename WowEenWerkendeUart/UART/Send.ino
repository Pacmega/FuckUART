void sending()
{
  if (sendingData)
  {
    if (currentBit < (sizeOfSerializedByte - 1))
    {
      if (sendone == SendOneInNine)
      {
        if (serializedByte[currentBit] == HIGH)
        {
          PORTB |= B00100000; // Pin 13 set to HIGH
        }
        else
        {
          PORTB &= B11011111; // Pin 13 set to LOW
        }
        currentBit++;
        sendone = 0;
      }
      else
      {
        sendone++;
      }
    }
    else
    {
      currentBit = 0;
      sendingData = false;
    }
  }
  else
  {
    PORTB |= B00100000; // When idle, keep pin 13 high.
  }
}

int countOnes(unsigned char itemToCount)
{
  int ones;
  for (int i = 0; i < 8; ++i)
  {
    if ((itemToCount >> i) & 0b1 == 1)
    {
      ones++;
    }
  }
  return ones;
}

void addStopBits(int firstStopbit)
{
  serializedByte[firstStopbit] = stopBit;

  if (stopBits == oneStopbit)
  {
    serializedByte[firstStopbit + 1] = disabledBit;
  }
  else 
  {
    serializedByte[firstStopbit + 1] = stopBit;
  }
}

void addParity(unsigned char byteToSend)
{
  // After the start bit and the 8 data bits the parity, if any, is placed at position 9 in the array. (defined in UART.ino)
  // This function is only called when there is a parity mode enabled, so we don't need to consider noParityMode.

  int ones = countOnes(byteToSend);

  if (parityMode == oddParityMode)
  {
    if (ones % 2 == 0)
    {
      // Even number of data 1's
      serializedByte[parityLocation] = parityOn;
    }
    else
    {
      // Odd number of data 1's
      serializedByte[parityLocation] = parityOff;
    }
  }
  else
  {
    // Only remaining option is even parity
    if (ones % 2 == 1)
    {
      // Odd number of data 1's
      serializedByte[parityLocation] = parityOn;
    }
    else
    {
      // Even number of data 1's
      serializedByte[parityLocation] = parityOff;
    }
  }
}

void serialize(unsigned char byteToSend)
{
  serializedByte[0] = startBit;

  for (int i = 0; i < 8; ++i)
  {
    serializedByte[i + 1] = (byteToSend >> i) & 0b1;
  }
  if (parityMode == noParityMode)
  {
    addStopBits(parityLocation);
  }
  else
  {
    addParity(byteToSend);
    addStopBits(10);
  }
  sendingData = true;
}