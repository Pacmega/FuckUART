void UARTreceive()
{
  tempBit = (int)readBit();
  int index = 0;

  switch(receiveSwitch)
  {
    case waitingForStartBit:
      // Serial.print("waitingforstartbit \n");
      if (tempBit == 0) // Falling edge
      {
        receiveSwitch = readingStartBit;
      }
      break;

    case readingStartBit:
      // Serial.print("readingstartbit \n");
      // Start sampling startbit
      if (samplePlace < sampleAmount)
      {
        // Write sample to startbitbuffer
        startBitBuffer[samplePlace] = tempBit;
        samplePlace++;
      }
      else
      {
        // Buffer filled
        if (checkStartBit())
        {
          // startbit found
          receiveSwitch = readingData;
          // start reading data
        }
        else
        {
          // The falling edge was an error. Reset process
          receiveSwitch = waitingForStartBit;
        }
      }
      break;

    case readingData:
      if (byteBufferFilled)
      {
        // majority check
        // parity check
        // received -> true
        if (index == sizeOfReceivedByte)
        {
          Serial.print(ReceivedByte);
        }
        int shift = sizeOfReceivedByte - index;
        ReceivedByte |= (int)checkMajority(receivedByteBuffer[index]) << shift;
        index++;
      }
      else
      {
        
        // Serial.print("readingdata \n");
        // fill buffer
        receivedByteBuffer[bytePlace][samplePlace] = tempBit;
        samplePlace++;

        if (samplePlace == sampleAmount && bytePlace == sizeOfReceivedByte)
        {
          // buffer filled
          byteBufferFilled = true;
          samplePlace = 0;
          bytePlace = 0;
        }

        if (samplePlace == sampleAmount)
        {
          // Sampleplace reached end. Restart on next bytePlace
          samplePlace = 0;
          bytePlace++;
        }

      }
      break;

    default:
    // Don't do anything
      break;
  }
}

bool checkStartBit()
{
  // This function is only called when the buffer is full and needs to be as small as possible, so don't check.

  int amountOfSamplesUsed = sampleAmount/4;

  unsigned char usedSamples[amountOfSamplesUsed];
  int startOfUsedSamplesInBuffer = (sampleAmount/2-(sampleAmount/8));

  for (int i = 0; i < amountOfSamplesUsed; ++i)
  {
    usedSamples[i] = startBitBuffer[startOfUsedSamplesInBuffer+i];
  }

  samplePlace = 0;

  return !checkMajority(startBitBuffer); // If checkmajority returns 0, startbit has been found and true will be returned.

}
bool checkMajority(unsigned char sampleArray[])
{
  int totalOnes = 0;

  for (int i = 0; i < sampleAmount; ++i)
  {
    totalOnes += sampleArray[i];
  }

  if (totalOnes >= sampleAmount / 2)
  {
    return 1;
  }
  return 0;
}

bool checkParity()
{
  
}

char readBit()
{
  // for debugging purposes:
  //Serial.println(digitalRead(recvPin));
  //Serial.println((PORTD & B00001000) >> 3);

  // Serial.println(digitalRead(recvPin));
  // return 'a';
  // return (PIND & B00001000) >> 3;
  return digitalRead(recvPin);
}
