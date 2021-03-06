// BIEP BOEP
#define sizeOfSerializedByte 12
#define sizeOfReceivedByte 11 // for receiving, the startbit wont be counted into the program.
#define sampleAmount 8

#define parityLocation 9
#define parityOn 1
#define parityOff 0

enum receiveStates {
  waitingForStartBit,
  readingStartBit,
  readingData,
  flushing
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

bool sending = false;
bool received = false;  // Did we receive something to analyse & decode?
bool flushRecvBuffer = false; // Flushing the buffer?

int currentBit = 0;
int ReceivedBitSample = 0;

// Settings
const unsigned int bitRate = 9600; // (bitRate must be >= 1 and < 65536)
const unsigned int parityMode = oddParityMode;
const unsigned int stopBits = oneStopbit;

long interruptFreq = 16000000 / bitRate / sampleAmount;

unsigned char serializedByte[sizeOfSerializedByte];
unsigned char startBitBuffer[sampleAmount];
unsigned char receivedByteBuffer[sizeOfReceivedByte][sampleAmount];
bool byteBufferFilled = false;
unsigned char bufferByte = '\0';

int bytePlace = 0;
int samplePlace = 0;
int tempBit;
int ReceivedByte;

int receiveSwitch = 0;
int messageSize = 0;

// Used for checking bits
const int amountOfSamplesUsed = sampleAmount / 4;
unsigned char usedSamples[amountOfSamplesUsed];
int startOfUsedSamplesInBuffer = (sampleAmount / 2 - (sampleAmount / 8));

ISR(TIMER1_COMPA_vect) // Interrupt service routine: Timer 1 matches desired count.
{
  // The combination of the two appears to be too slow to function.
  UARTsend();
  UARTreceive();
  // Serial.print(digitalRead(13));
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
  if (!sending && Serial.available() > 0)
  {
    bufferByte = Serial.read();
    Sending(bufferByte);
  }

  //byteReading();
  if (received)
  {
    //Serial.print("ReceivedByte ");
    Serial.println(ReceivedByte);
    ReceivedByte = 0;
    receiveSwitch = flushing;
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
