// Send maybe working, receive broken

#define sizeOfSerializedByte 12
#define sampleAmount 9

#define parityLocation 9
#define parityOn 1
#define parityOff 0

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

bool sending = false;

int currentBit = 0;
int ReceivedBitSample = 0;

// Settings
const unsigned int bitRate = 9600; // (bitRate must be >= 1 and < 65536)
const unsigned int parityMode = oddParityMode;
const unsigned int stopBits = oneStopbit;

long interruptFreq = 16000000 / bitRate / sampleAmount;

unsigned char serializedByte[sizeOfSerializedByte];
unsigned char bufferByte = '\0';

int bytePlace = 0;
int samplePlace = 0;
int tempBit;

int messageSize = 0;

ISR(TIMER1_COMPA_vect) // Interrupt service routine: Timer 1 matches desired count.
{
  // The combination of the two appears to be too slow to function.
  UARTsend();
  // UARTreceive();
}

void setup()
{
  pinMode(sendPin, OUTPUT);

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
}

void clearBuffer(int bufferSize, unsigned char* array)
{
  for (int i = 0; i < bufferSize; ++i)
  {
    // Set all of the bits in the serialized byte buffer to 0.
    array[i] = 0;
  }
}
