const int wave1Pin = A0;
const int wave2Pin = A1;

enum Direction{forward = 10, reverse, stopped};
Direction motorDirection;
Direction motorDirectionPrev;

int directionChangeCounter; // Used to verify continuous direction change
int wave1, wave2; // wave's current state (high/low)
int wave1Prev, wave2Prev; // wave's previous state
boolean wave1Error, wave2Error; // if the wave is not outputting
int ticks; // Number of ticks counted
unsigned long time; // used for time reading
double rpm; // encoder's revolutions per minute
double rpmPrev; // previous rpm value

int error1Pin = 8;
int error2Pin = 9;
int forwardPin = 10;
int reversePin = 11;
int stoppedPin = 12;

void setup()
{
  Serial.begin(9600);
  pinMode(wave1Pin, INPUT);
  pinMode(wave2Pin, INPUT);
  wave1Prev = digitalRead(wave1Pin); // Initiallize previous state
  wave2Prev = digitalRead(wave2Pin);
  wave1Error = true; // Assume wave is not outputting
  wave2Error = true;
  motorDirectionPrev = forward;
  rpm = 0;
  rpmPrev = 0;
  directionChangeCounter = 0;
  time = millis();
  
  pinMode(error1Pin, OUTPUT);
  pinMode(error2Pin, OUTPUT);
  pinMode(forwardPin, OUTPUT);
  pinMode(reversePin, OUTPUT);
  pinMode(stoppedPin, OUTPUT);
}

void loop()
{
  Input();
  TestSensors();
  DetermineDirection();
  CalculateSpeed();
  Output();
  UpdatePreviousValues();
}

// Input the current wave values of the encoder
void Input()
{
  wave1 = digitalRead(wave1Pin); // update current wave state
  wave2 = digitalRead(wave2Pin);
}

// Test if either of the encoder waves is working
void TestSensors()
{
  if(wave1 == LOW) // Wave is working
  {
    wave1Error = false;
  }
  
  if(wave2 == LOW)
  {
    wave2Error = false;
  }
}

// Determine the direction of the encoder rotation
// Encoder must move in same direction for a certain amount of time
// before it is officially moving in that direction
void DetermineDirection()
{
  if(wave1 == LOW) // Testing for forward direction
  {
    if(wave1Prev == HIGH && wave2Prev== HIGH) // wave1 went high first, forward movement
    {
      if(motorDirection == reverse) // possible direction change
      {
        if(directionChangeCounter < 50) // not forward long enough
        {
          directionChangeCounter++;
        }
        else // consistent enough direction change
        {
          motorDirection = forward;
          directionChangeCounter = 0;
          ticks = 0;
          time = millis();
        }
      }
      else // already moving forward
      {
        ticks++;
        directionChangeCounter = 0;
      }
    }
  }
  else if(wave2 == LOW) // Testing for reverse direction
  {
    if(wave1Prev == HIGH && wave2Prev== HIGH) // wave1 went high first, reverse movement
    {
      if(motorDirection == forward) // possible direction change
      {
        if(directionChangeCounter < 50) // not reverse long enough
        {
          directionChangeCounter++;
        }
        else // consistent enough direction change
        {
          motorDirection = reverse;
          directionChangeCounter = 0;
          ticks = 0;
          time = millis();
        }
      }
      else // already moving reverse
      {
        directionChangeCounter = 0;
        ticks++;
      }
    }
  }
}

// Calculate the rpm speed of the encoder. If 0, change direction to "stopped"
void CalculateSpeed()
{
  if(millis() >= time + 100)
  {
    rpm = ticks * 12.0 / 5.0 ; // (ticks / 100ms) * (1000ms / 1sec) * (60sec / 1min) * (1rev / 250ticks) = 12 / 5
    
    if(rpm == 0)
    {
      motorDirection = stopped;
    }
    
    time = millis();
    ticks = 0;
  }
}

// Outpout to the serial monitor or LEDs
void Output()
{
  digitalWrite(error1Pin, wave1Error);
  digitalWrite(error2Pin, wave2Error);
  
  if(motorDirection != motorDirectionPrev) // Direction change
  {
    digitalWrite(motorDirectionPrev, LOW);
    digitalWrite(motorDirection, HIGH);
    //Serial.println(String(motorDirection)); // output
  }
  
  if(rpm != rpmPrev)
  {
    Serial.println(String(rpm)); // output
  }
}

// Record current values into previous values
void UpdatePreviousValues()
{
  wave1Prev = wave1; // update previous wave state
  wave2Prev = wave2;
  motorDirectionPrev = motorDirection;
  rpmPrev = rpm;
}
