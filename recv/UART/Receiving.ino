void UARTreceive()
{
  tempBit = (PIND & B00001000) >> 3;

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
        digestSwitch = checkingMajority; // The two lines above here are a bit I recently added. -Bas
      }
      else if (samplePlace == sampleAmount)
      {
        // Sampleplace reached end. Restart on next bytePlace
        samplePlace = 0;
        bytePlace++;
      }
      else
      {
        receivedByteBuffer[bytePlace][samplePlace] = tempBit;
        samplePlace++;
      }
      break;

    default:
      // Don't do anything
      break;
  }
}

void deserializeCharacter()
{
  // Does WAY too much. Checking majority and parity in a function called deserialize?
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
