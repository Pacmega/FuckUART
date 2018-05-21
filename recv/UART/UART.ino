// RecieveInProgress, Send working but commented in ISR

#define sizeOfReceivedByte 10 // for receiving, the startbit wont be counted into the program.
// DBG: for now it is, because timing is fucking weird apparently. This allows for printing the start bit
//      and looking at it after everything's come in, since there is no time before then.

#define sizeOfSerializedByte 12
#define sampleAmount 7
#define samplesUsed 3

#define parityLocation 9
#define parityOn 1
#define parityOff 0

enum receiveBitStates {
  waitingForStartBit,
  readingStartBit,
  checkingStartBit,
  fillingBuffer,
  checkingData
};

enum digestBitsStates {
  checkingMajority,
  checkingParity,
  deserializing,
  readyForReading
};

enum specialBits {
  startBit = 0b0,
  stopBit = 0b1,
  disabledBit = 0b0
};

enum parityModeEnum {
  evenParityMode,
  oddParityMode,
  noParityMode
};

enum stopBitsEnum {
  oneStopbit,
  twoStopbits
};

const unsigned int sendPin = 13;
const unsigned int recvPin = 3;

bool received = false;  // Did we receive something to analyse & decode?
bool flushRecvBuffer = false; // Flushing the buffer?

// Settings
const unsigned int bitRate = 9600; // (bitRate must be >= 1 and < 65536)
const unsigned int parityMode = oddParityMode;
const unsigned int stopBits = oneStopbit;

long interruptFreq = 16000000 / bitRate / sampleAmount;

int startBitBuffer[sampleAmount];
int receivedByteBuffer[sizeOfReceivedByte][sampleAmount];
unsigned char receivedDataBits[9];
bool byteBufferFilled = false;

int bytePlace = 0;
int samplePlace = 0;
int tempBit;

int receiveSwitch = waitingForStartBit;
int digestSwitch = -1;
int extraBits;

// DBG - I don't think these are actually used...?
// Used for checking bits
// const int amountOfSamplesUsed = sampleAmount / 4;
// unsigned char usedSamples[amountOfSamplesUsed];
// int startOfUsedSamplesInBuffer = (sampleAmount / 2 - (sampleAmount / 8));

ISR(TIMER1_COMPA_vect) // Interrupt service routine: Timer 1 matches desired count.
{
  //UARTsend();
  UARTreceive();
}

void setup()
{
  pinMode(sendPin, OUTPUT);
  pinMode(recvPin, INPUT);

  if (parityMode != noParityMode)
  {
    sizeOfReceivedByte++;
  }
  if (stopBits == twoStopbits)
  {
    sizeOfReceivedByte++;
  }

  Serial.begin(9600);

  cli(); // To be sure no interrupts top interrupts

  TCCR1A = 0; // Set entire TCCR1A register to 0
  TCCR1B = 0; // Same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0

  // Set compare match register for selected baud rate
  OCR1A  = interruptFreq;

  TCCR1B |= (1 << WGM12); // Turn on Clear Timer on Compare match mode

  // Set CS12, CS11 and CS10 bits for 1 prescaler
  TCCR1B |= (1 << CS10);

  TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt

  sei(); // Allow interrupts

  Serial.begin(9600);
}

void loop()
{
  // Buffer filled (should checkStartBit be in the ISR? Doesn't seem like it.)
  if (receiveSwitch == checkingStartBit)
  {
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

    sei(); // Restart interrupts.
  }
  else if (receiveSwitch == checkingData)
  {
    DBG_printBuffer();

    if (checkStartStopBits())
    {
      Serial.println("Packet detected.");

      for (int i = 1; i <= 9; ++i) // Iterate over the data bits and the parity bit
      {
        receivedDataBits[i-1] = checkMajority(receivedByteBuffer[i]);
      }

      if (parityMode != noParityMode)
      {
        if (checkParity())
        {
          Serial.print(deserialize());
        }
        else
        {
          Serial.println("Incorrect parity.");
        }
      }
      
      Serial.print(deserialize());
      
      // Re-enable interrupts when everything is done.
      // Should probably clear the buffer after finishing.
    }
    else
    {
      Serial.println("Incorrect packet detected. Oops.");
    }
    
    clearBuffer(sizeOfReceivedByte, receivedByteBuffer)

    receiveSwitch = waitingForStartBit;

    // End of DBG

    // Shit
  }

  // DBG: PortManipulation version of digitalRead(3);
  //Serial.println((PIND & B00001000) >> 3);
  
  /*
  if (Serial.available() > 0 && !sending)
  {
    bufferByte = Serial.read();
    Sending(bufferByte);
  }

  //byteReading();
  deserializeCharacter();
  if (received)
  {
    //Serial.print("ReceivedByte ");
    Serial.println(ReceivedByte);
    ReceivedByte = 0;
    //receiveSwitch = flushing;	
    received = false;
  }
  */
}

void clearBuffer(int bufferSize, unsigned char* array)
{
  for (int i = 0; i < bufferSize; ++i)
  {
    // Set all of the bits in the serialized byte buffer to 0.
    array[i] = 0;
  }
}
