void UARTreceive()
{
  tempBit = (int)readBit();

  switch (receiveSwitch)
  {
    case waitingForStartBit:
      if (tempBit == 0) // Falling edge
      {
        receiveSwitch = readingStartBit;
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
        // Buffer filled
        if (checkStartBit())
        {
          // startbit found
          receiveSwitch = fillingBuffer;
          // start reading data
        }
        else
        {
          // The falling edge was an error. Reset process
          receiveSwitch = waitingForStartBit;
        }
      }
      break;

    case fillingBuffer:
      // fill buffer
      receivedByteBuffer[bytePlace][samplePlace] = tempBit;
      samplePlace++;

      if (samplePlace == sampleAmount && bytePlace == sizeOfReceivedByte)
      {
      	Serial.print("filled the buffer");
        // buffer filled
        byteBufferFilled = true;
        samplePlace = 0;
        bytePlace = 0;
        digestSwitch = checkingMajority;
        receiveSwitch++;
      }

      if (samplePlace == sampleAmount)
      {
        // Sampleplace reached end. Restart on next bytePlace
        samplePlace = 0;
        bytePlace++;
      }
      break;

    default:
      // Don't do anything
      break;
  }
}

void deserializeCharacter()
{
  switch(digestSwitch)
  {
    case checkingMajority:
    	for (int i = 0; i < sizeOfReceivedByte; ++i)
    	{
    		ReceivedByte |= (int)checkMajority(receivedByteBuffer[i]) << bytePlace;
    	}

        bytePlace = 0;
        receiveSwitch = 0;
        digestSwitch = checkingParity;
      break;

    case checkingParity:
      if (checkParity(ReceivedByte))
      {
        // Parity is correct, continue to deserializing
        receiveSwitch = deserializing;
      }
      else
      {
        // Parity is incorrect. Notify user and reset
        Serial.println("Parity error.");
        digestSwitch = -1;
      }
      break;

    case deserializing:
      // Remove the stopbits and the parity bit if selected
      extraBits = 1; // There is always 1 stopbit at the end, so that one will always be removed
      if (parityMode != noParityMode)
      {
        extraBits++;
      }

      if (stopBits == twoStopbits)
      {
        extraBits++;
      }
      // Remove the bits that are not part of the data
      ReceivedByte = ReceivedByte >> extraBits;
      receiveSwitch = readyForReading;
      break;

    case readyForReading:
      received = true;
      digestSwitch++;
      // Wait for the loop to read the character. Loop will continue the switch.
      break;

    default:
      // do nothing
      break;
  }
}

bool checkStartBit()
{
  // This function is only called when the buffer is full and needs to be as small as possible, so don't check.

  int amountOfSamplesUsed = sampleAmount / 4;

  unsigned char usedSamples[amountOfSamplesUsed];
  int startOfUsedSamplesInBuffer = (sampleAmount / 2 - (sampleAmount / 8));

  for (int i = 0; i < amountOfSamplesUsed; ++i)
  {
    usedSamples[i] = startBitBuffer[startOfUsedSamplesInBuffer + i];
  }

  samplePlace = 0;

  return !checkMajority(startBitBuffer); // If checkmajority returns 0, startbit has been found and true will be returned.

}

bool checkMajority(int sampleArray[])
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

bool checkParity(unsigned char receivedByte)
{
  for (int i = 0; i < sizeof(receivedByte); ++i)
  {
  	
  }
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