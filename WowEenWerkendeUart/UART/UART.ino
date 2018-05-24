// Main TODOs:
// - interruptFreq?
// - Varying messageSize based on settings

#define sizeOfSerializedByte 12

#define parityLocation 9
#define parityOn 1
#define parityOff 0

int sendone = 0;
int SendOneInNine = 9;

int ArrayPosition = 0;

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

int ReceivePin = 4;
int SendPin = 3;

const int MaxNumberOfCycles = 2;
int CycleCounter = 0;

const int sampleAmount = 8;
int SampleCounter = 0;
int SampleArray[sampleAmount];
boolean TakenAllSamples = false;

int DataArrayCounter = 0;
const int MaxArrayLength = 12;
unsigned char DataArray[MaxArrayLength];
unsigned char serializedByte[sizeOfSerializedByte];

// Settings
const unsigned int bitRate = 9600; // (bitRate must be >= 1 and < 65536)
const unsigned int parityMode = oddParityMode;
const unsigned int stopBits = oneStopbit;

long interruptFreq = 16000000 / bitRate / sampleAmount; // BAS - Unused

byte zerobyte = 0x00;
byte onebyte = 0x01;

boolean FallingEdgeDetected = false;
boolean doneENCRYPT         = false;

void setup()
{
  pinMode(ReceivePin, INPUT);
  pinMode(SendPin, OUTPUT);
  
  cli(); // To be sure no interrupts top interrupts

  TCCR1A = 0; // Set entire TCCR1A register to 0
  TCCR1B = 0; // Same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0

  // BAS - Wtf is this number? (it works at 2400 Hz)
  OCR1A  = 6665; // Set compare match register for selected baud rate

  TCCR1B |= (1 << WGM12); // Turn on Clear Timer on Compare match mode

  TCCR1B |= (1 << CS10); // Set CS10 bit for 1 prescaler

  TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt

  sei(); // Allow interrupts

  Serial.begin(9600);
  PORTD = B00001000;
}

ISR(TIMER1_COMPA_vect)
{
  Sending();
  
  if (DetectedFallingEdge())
  {
    if (SampleCounter == sampleAmount + 1 && !TakenAllSamples)
    {
      SampleCounter = 0;
      TakenAllSamples = true;
    }
    else
    {
      if (!TakenAllSamples)
      {
        TakeSample();
        SampleCounter++;
      }
    }
  }
}

void loop()
{
  if (DetectedFallingEdge())
  {
    sampleing();
  }
  if (!doneENCRYPT && Serial.available() > 0)
  {
    unsigned char data = Serial.read();
    serialize(data);
  }
}


