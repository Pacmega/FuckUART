#define sizeOfSerializedByte 12
#define parityLocation 9
#define parityOn 1
#define parityOff 0
#define Baudrate 9600

const int MaxArrayLength        =     12;
const int MaxNumberOfCycles     =     2;
const int MaxNumberOfSamples    =     8;

byte zerobyte                   =     0;
byte onebyte                    =     1;

enum specialBits {
  startBit      = 1,
  stopBit       = 0,
  disabledBit   = 0
};

enum parityModeEnum {
  oddParityMode,
  evenParityMode
};

enum stopBitsEnum {
  oneStopbit,
  twoStopbits
};

unsigned int parityMode         =     evenParityMode;
unsigned int stopBits           =     twoStopbits;

int sendone                     =     0;
int SendOneInNine               =     9;
int ArrayPosition               =     0;
int ReceivePin                  =     4;
int SendPin                     =     3;
int CycleCounter                =     0;
int SampleCounter               =     0;
int DataArrayCounter            =     0;

boolean TakenAllSamples         = false;
boolean FallingEdgeDetected =   false;
boolean doneENCRYPT         =   false;

unsigned char serializedByte[sizeOfSerializedByte];
unsigned char DataArray[MaxArrayLength];

int SampleArray[MaxNumberOfSamples];

void setup()
{
  pinMode(ReceivePin, INPUT);
  pinMode(SendPin, OUTPUT);
  digitalWrite(SendPin, LOW);
  SetupInterupts();
  Serial.begin(Baudrate);
  PORTD = B00001000;
}

ISR(TIMER1_COMPA_vect)
{
  Sending();
  Receiving();
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


