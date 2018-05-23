int countOnes(unsigned char itemToCount)
{
  int ones;
  for (int i = 0; i < 8; ++i)
  {
    if ((itemToCount >> i) & 1 == 1)
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
    if (ones % 2 == 1)
    {
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
  serializedByte[0] = startBit;

  for (int i = 0; i < 8; ++i)
  {
    serializedByte[i + 1] = (byteToSend >> i) & 0b1;
  }
  addParity(byteToSend);
  addStopBits(10);

  for (int i = 0; i < 12; i++)
  {
    Serial.print(serializedByte[i]);
  }
  Serial.println(" ");
  doneENCRYPT = true;
}

void Sending()
{
  if (doneENCRYPT)
  {
    if (sendone == SendOneInNine)
    {
      if (ArrayPosition != (sizeOfSerializedByte - 1))
      {
        if (serializedByte[ArrayPosition] == 0)
        {
          PORTD = B00000000;
        }
        else
        {
          PORTD = B00001000;
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
}

