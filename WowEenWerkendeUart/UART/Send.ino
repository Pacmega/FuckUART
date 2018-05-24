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
  // After the start bit and the 8 data bits the parity, if any,
  // is placed at position 9 in the array.

  int ones = countOnes(byteToSend);

  if (parityMode == oddParityMode)
  {
    if (ones % 2 == 0)
    {
      serializedByte[parityLocation] = parityOn;
    }
    else
    {
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
      serializedByte[parityLocation] = parityOff;
    }
  }
}

void serialize(unsigned char byteToSend)
{
  //serializedByte = NULL;
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
  // for (int i = 0; i < 12; i++)
  // {
  //   Serial.print(serializedByte[i]);
  // }
  // Serial.println(" ");
  doneENCRYPT = true;
}


void sending()
{
  if (doneENCRYPT)
  {
    if (sendone == SendOneInNine)
    {
      if (ArrayPosition != (sizeOfSerializedByte - 1))
      {
        if (serializedByte[ArrayPosition] == 0)
        {
          PORTB &= B11011111; // Pin 13 set to LOW
        }
        else
        {
          PORTB |= B00100000; // Pin 13 set to HIGH
        }
        ArrayPosition++;
        sendone = 0;
      }
      else
      {
        ArrayPosition = 0;
        doneENCRYPT = false;
       
      }
    }
    else
    {
      sendone = sendone + 1;
    }
  }
  else
  {
    PORTB |= B00100000; // When idle, keep pin 13 high.
  }
}

