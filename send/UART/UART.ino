// heck

#define sizeOfSerializedByte 12
#define sizeOfReceivedByte 11 // for receiving, the startbit won't be read into the buffer.
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
int currentBit = 0;

const unsigned int bitRate = 9600; // (bitRate must be >= 1 and < 65536)
const unsigned int parityMode = oddParityMode;
const unsigned int stopBits = oneStopbit;


long interruptFreq = 16000000 / bitRate / sampleAmount;

int ReceivedBitSample = 0;

unsigned char serializedByte[sizeOfSerializedByte];
unsigned char receivedByte[sizeOfReceivedByte];
unsigned char bufferByte = '\0';

int messageSize = 0;

ISR(TIMER1_COMPA_vect) // Interrupt service routine: Timer 1 matches desired count.
{
  if (sending)
  {
    if (messageSize > currentBit)
    {
      if (ReceivedBitSample == sampleAmount - 1) // MagicNumber.exe
      {
        if (serializedByte[currentBit] == HIGH)
        {
          // Set third bit from the right to HIGH
          digitalWrite(sendPin, HIGH);
        }
        else
        {
          // Disable third bit from the right
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
    digitalWrite(sendPin, HIGH);
  } 
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

  // Set CS10 bits for 1 prescaler
  TCCR1B |= (1 << CS10);

  TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt

  sei(); // Allow interrupts
}

void loop() 
{
  if (!sending && Serial.available() > 0)
  {
    bufferByte = Serial.read();
    Sending(bufferByte);
  }
}

void clearBuffer(int bufferSize, unsigned char* array)
{
  for (int i = 0; i < bufferSize; ++i)
  {
    // Set all of the bits in the buffer to 0.
    array[i] = 0;
  }
}
