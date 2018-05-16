// Working send

#define sizeOfSerializedByte 12

void UARTsend()
{
  if (sending)
  {
    if (messageSize > currentBit)
    {
      if (ReceivedBitSample == sampleAmount - 1) // MagicNumber.exe
      {
        if (serializedByte[currentBit] == HIGH)
        {
          // Set pin 13 to HIGH (1)
          digitalWrite(sendPin, HIGH);
        }
        else
        {
          // Set pin 13 to LOW (0)
          digitalWrite(sendPin, LOW);
        }
        currentBit++;
        ReceivedBitSample = 0;
      }
      else
      {
        ReceivedBitSample++;
      }
    }
    else
    {
      sending = false;
      currentBit = 0;
      ReceivedBitSample = 0;
    }
  }
  else
  {
    // Idle: set pin 13 to HIGH (1)
    digitalWrite(sendPin, HIGH);
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
	messageSize++;
	serializedByte[firstStopbit] = stopBit;
	
	if (stopBits == twoStopbits)
	{
		messageSize++;
		// stopBits == twoStopBits
		serializedByte[firstStopbit+1] = stopBit;
	}
}

void addParity(unsigned char byteToSend)
{
	// After the start bit and the 8 data bits the parity, if any,
	// is placed at position 9 in the array.

	messageSize++;
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

void createPacket(unsigned char byteToSend)
{
	// The first bit in the array is a start bit
	serializedByte[0] = startBit;
	messageSize = 9;

	for (int i = 0; i < 8; ++i)
	{
		// Starting at the beginning (LSB) of the byte, serialize the byte into the array.
		// The bits are written to the position in the array that is one more than their position in the original byte,
		// since there is a start bit before the data.
		serializedByte[i+1] = (byteToSend >> i) & 0b1;
	}

	// The last one to three functional bits in the array are the stop bit (-s) and possibly a parity bit.
	if (parityMode == noParityMode)
	{
		// The stop bits are one bit earlier than with parity
		addStopBits(9); // Stop bit (-s) are at position 10 and 11
	}
	else
	{
		addParity(byteToSend);
		addStopBits(10); // Stop bit (-s) are at position 11 and 12
	}
}

void Sending(unsigned char recvByte)
{
	if (recvByte != '\0')
	{
		clearBuffer(sizeOfSerializedByte, serializedByte);
		createPacket(recvByte);
    	
		// Serial.println();
		// for (int i = 0; i < messageSize; ++i)
		// {
		// 	Serial.println(serializedByte[i]);
		// }

		sending = true;
	}
}