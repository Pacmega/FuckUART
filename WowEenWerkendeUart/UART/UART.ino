// Main TODOs:
// - interruptFreq?
// - Varying messageSize based on settings
// - Check BAS comments for more.

#define sizeOfSerializedByte 12

#define parityLocation 9
#define parityOn 1
#define parityOff 0

int sendone = 0;
int SendOneInNine = 9;

int currentBit = 0;

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

const int recvPin = 3;
const int sendPin = 13;

const int MaxNumberOfCycles = 2;
int CycleCounter = 0;

const int sampleAmount = 8;
int SampleCounter = 0;
int SampleArray[sampleAmount];
bool TakenAllSamples = false;

int DataArrayCounter = 0;
const int MaxArrayLength = 12;
unsigned char DataArray[MaxArrayLength];
unsigned char serializedByte[sizeOfSerializedByte];

// Settings
const unsigned int bitRate = 9600; // (bitRate must be >= 1 and < 65536) BAS - unused
const unsigned int parityMode = oddParityMode;
const unsigned int stopBits = twoStopbits;

long interruptFreq = 16000000 / bitRate / sampleAmount; // BAS - Unused

bool receivingData = false;
bool sendingData   = false;

ISR(TIMER1_COMPA_vect)
{
  sending();
  receiving();
}

void setup()
{
  pinMode(recvPin, INPUT);
  pinMode(sendPin, OUTPUT);
  
  cli(); // To be sure no interrupts top interrupts

  TCCR1A = 0; // Set entire TCCR1A register to 0
  TCCR1B = 0; // Same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0

  // BAS - Baudrate of 2400? What other possibilities are there?
  OCR1A  = 6665; // Set compare match register for selected baud rate

  TCCR1B |= (1 << WGM12); // Turn on Clear Timer on Compare match mode

  TCCR1B |= (1 << CS10); // Set CS10 bit for 1 prescaler

  TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt

  sei(); // Allow interrupts

  Serial.begin(9600);
}

void loop()
{
  if (receivingData)
  {
    sampling();
  }

  if (!sendingData && Serial.available() > 0)
  {
    serialize(Serial.read());
  }
}


