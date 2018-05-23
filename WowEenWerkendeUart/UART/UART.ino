#define sizeOfSerializedByte 12
unsigned char serializedByte[sizeOfSerializedByte];

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

int parityMode = evenParityMode;
int stopBits = twoStopbits;

int receivePin = 3;
int sendPin = 13;

const int Baudrate = 9600;

const int MaxNumberOfCycles = 2;
int CycleCounter = 0;

const int MaxNumberOfSamples = 8;
int SampleCounter = 0;
int SampleArray[MaxNumberOfSamples];
boolean TakenAllSamples = false;

int DataArrayCounter = 0;
const int MaxArrayLength = 12;
unsigned char DataArray[MaxArrayLength];

byte zerobyte = 0x00;
byte onebyte = 0x01;

boolean FallingEdgeDetected = false;
boolean doneENCRYPT         = false;

void setup()
{
  pinMode(receivePin, INPUT);
  pinMode(sendPin, OUTPUT);

  digitalWrite(sendPin, LOW);

  SetupInterupts();

  Serial.begin(Baudrate);
  PORTD = B00001000;
}

ISR(TIMER1_COMPA_vect)
{
  Sending();
  if (DetectedFallingEdge())
  {
    if (SampleCounter == MaxNumberOfSamples + 1 && !TakenAllSamples)
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


