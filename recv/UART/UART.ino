#define sizeOfSerializedByte 12
#define sizeOfReceivedByte 11 // for receiving, the startbit wont be counted into the program.
#define sampleAmount 15

#define parityLocation 9
#define parityOn 1
#define parityOff 0

enum receiveBitStates {
  waitingForStartBit,
  readingStartBit,
  fillingBuffer
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
  noParityMode,
  oddParityMode,
  evenParityMode
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
bool byteBufferFilled = false;

int bytePlace = 0;
int samplePlace = 0;
int tempBit;
int ReceivedByte;

int receiveSwitch = 0;
int digestSwitch = -1;
int extraBits;

// Used for checking bits
const int amountOfSamplesUsed = sampleAmount / 4;
unsigned char usedSamples[amountOfSamplesUsed];
int startOfUsedSamplesInBuffer = (sampleAmount / 2 - (sampleAmount / 8));

ISR(TIMER1_COMPA_vect) // Interrupt service routine: Timer 1 matches desired count.
{
  //UARTsend();
  UARTreceive();
}

void setup()
{
  pinMode(sendPin, OUTPUT);
  pinMode(recvPin, INPUT);

  Serial.begin(9600);

  cli(); // To be sure no interrupts top interrupts

  TCCR1A = 0; // Set entire TCCR1A register to 0
  TCCR1B = 0; // Same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0

  // Set compare match register for selected baud rate
  OCR1A  = 16000000 / bitRate;

  TCCR1B |= (1 << WGM12); // Turn on Clear Timer on Compare match mode

  // Set CS12, CS11 and CS10 bits for 1 prescaler
  TCCR1B |= (1 << CS10);

  TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt

  sei(); // Allow interrupts

  Serial.begin(9600);
}

void loop()
{
/*  if (Serial.available() > 0 && !sending)
  {
    bufferByte = Serial.read();
    Sending(bufferByte);
  }*/

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
}

void clearBuffer(int bufferSize, unsigned char* array)
{
  for (int i = 0; i < bufferSize; ++i)
  {
    // Set all of the bits in the serialized byte buffer to 0.
    array[i] = 0;
  }
}
