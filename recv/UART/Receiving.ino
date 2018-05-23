void DBG_printBuffer()
{
  for(int i = 0; i < sizeOfReceivedByte; i++)
  {
    Serial.print("Byte ");
    Serial.print(i);
    Serial.print(" - ");
    for (int o = 0; o < sampleAmount; o++)
    {
      Serial.print(receivedByteBuffer[i][o]);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

void UARTreceive()
{
  tempBit = (PIND & B00001000) >> 3;

  switch (receiveSwitch)
  {
    case waitingForStartBit:
      if (tempBit == 0) // Falling edge
      {
        receiveSwitch = fillingBuffer;
      }
      break;

    case readingStartBit:
      //Serial.print("readingstartbit \n");
      // Start sampling startbit
      if (samplePlace < sampleAmount)
      {
        // Write sample to startbitbuffer
        startBitBuffer[samplePlace] = tempBit;
        samplePlace++;
      }
      else
      {
        // Startbitbuffer filled.
        samplePlace = 0;
        receiveSwitch = checkingStartBit;
        cli(); // Temporarily kill interrupts so the check (in the main loop, to keep this interrupt from taking too long) will be finished sooner.
      }
      break;

    case fillingBuffer:
      // Serial.print(samplePlace);
      // Serial.print(" - ");
      // Serial.print(bytePlace);
      // Serial.print(" | ");
      
      if (samplePlace == sampleAmount - 1 && bytePlace == sizeOfReceivedByte - 1)
      {
      	// buffer filled
        byteBufferFilled = true;
        samplePlace = 0;
        bytePlace = 0;
        receiveSwitch = checkingData; // Should allow for loop() to take over
        cli(); // Don't try to receive more while still working on this one.
        //digestSwitch = checkingMajority; // The two lines above here are a bit I recently added. -Bas (also, this doesn't do anything anymore.)
      }
      else if (samplePlace == sampleAmount)
      {
        // Sampleplace reached end. Restart on next bytePlace
        samplePlace = 0;
        bytePlace++;
        receivedByteBuffer[bytePlace][samplePlace] = tempBit;
      }
      else
      {
        receivedByteBuffer[bytePlace][samplePlace] = tempBit;
        samplePlace++;
      }
      break;

    default:
      // Don't do anything
      //Serial.println("in default...");
      break;
  }
}

bool checkStartBit() // Hardcoded :confettiballs:
{
  // check majority on startBitBuffer

  // int value = checkMajority(startBitBuffer);

  return !checkMajority(startBitBuffer[3], startBitBuffer[4], startBitBuffer[5]); // Want to have true on startbit found (0)
}

/*bool checkStartStopBits()
{
  // Built around sampleAmount = 7 & samplesUsed = 3, and reading the start bit into the full buffer.
  
  unsigned char usedSamples[samplesUsed];
  // usedSamples[0] = receivedByteBuffer[0][3];
  // usedSamples[1] = receivedByteBuffer[0][4];
  // usedSamples[2] = receivedByteBuffer[0][5];

  // if(!checkMajority(usedSamples)) // If checkmajority returns 0, startbit has been found and true will be returned.
  // {
    if(parityMode == noParityMode)
    {
      if (checkMajority(receivedByteBuffer[9][3], receivedByteBuffer[9][4], receivedByteBuffer[9][5]))
      {
        if(stopBits == twoStopbits)
        {
          usedSamples[0] = receivedByteBuffer[10][3];
          usedSamples[1] = receivedByteBuffer[10][4];
          usedSamples[2] = receivedByteBuffer[10][5];
          return checkMajority(usedSamples);
        }
        
        return true;
      }
    }
    else
    {
      usedSamples[0] = receivedByteBuffer[10][3];
      usedSamples[1] = receivedByteBuffer[10][4];
      usedSamples[2] = receivedByteBuffer[10][5];
      if (checkMajority(usedSamples))
      {
        if(stopBits == twoStopbits)
        {
          usedSamples[0] = receivedByteBuffer[11][3];
          usedSamples[1] = receivedByteBuffer[11][4];
          usedSamples[2] = receivedByteBuffer[11][5];
          return checkMajority(usedSamples);
        }
        
        return true;
      }
    }
  //}
  
  // No start bit or an incorrect number of stop bits detected, that was a mistake.
  return false;
}*/

bool checkParity()
{
  int ones = 0;

  for (int i = 0; i < 9; i++)
  {
    ones += receivedDataBits[i];
  }

  if ((ones % 2 == 1 && parityMode == oddParityMode) || (ones % 2 == 0 && parityMode == evenParityMode))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// This is basically the foolproof way to check it...
bool checkMajority(unsigned char a, unsigned char b, unsigned char c)
{
  // Serial.println(totalOnes);

  if (a + b + c >= 2)
    return 1; // AKA true

  return 0; // AKA false
}

unsigned char deserialize() 
{
  unsigned char deserializedChar = '\0';

  for (int i = 7; i >= 0; i--)
  {
    deserializedChar |= (receivedDataBits[i] << i);
  }

  return deserializedChar;
}

