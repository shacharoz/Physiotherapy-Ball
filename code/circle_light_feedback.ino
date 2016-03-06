
/*
  8x8 Dot Matrix LED Display
  Display four numbers or characters on 8x8 Dot Matrix LED Display
  In this sketch, we use D2-D19 to control cathod. And use D10-D17 to control the anode.
*/

int steps;
int maxSteps = 5; //sets number of time the effect circles


int inputPin = A5; // the number of the pin we use for input


void setup()
{
    Serial.begin(115200); //only for debug prints

    //Set D2-D17 as OUTPUT
    int i=2;
  
    for(i=2;i<18;i++)
    {
     pinMode(i,OUTPUT);
     digitalWrite(i,HIGH);
    }
  
    //reset the number of steps
    steps = 0;
    maxSteps = 5;


  //we dont have enough digital pins
    // initialize the pin as an output:
    //pinMode(inputPin, INPUT);
    //digitalWrite(inputPin, LOW);
}

void loop()
{
  // read the state of the inputPin and run the effect only when it gets HIGH
  //int inputPinState = digitalRead(inputPin);
  int inputPinState = analogRead(inputPin);

  Serial.print("data: ");
  Serial.println(inputPinState);
  delay(100);
  if (inputPinState > 950) {
      StartEffect();
  }
  
}



void StartEffect()
{
  int circleRadius;
  
  //Serial.println(runit, DEC);  
  
  if (steps < maxSteps)
  {
    for (circleRadius=0; circleRadius<4; circleRadius++) 
    {
      MakeCircle(circleRadius);
    }

    steps++;
  }
}

//present a circle with a radius sized "circle"
void MakeCircle(int radius)
{
  byte DrowCircle[4][8]={{0XFF,0XFF,0XFF,0XE7,0XE7,0XFF,0XFF,0XFF}, {0XFF,0XFF,0XE7,0XDB,0XDB,0XE7,0XFF,0XFF}, {0XFF,0XE7,0XDB,0XBD,0XBD,0XDB,0XE7,0XFF}, {0XE7,0XDB,0XBD,0X7E,0X7E,0XBD,0XDB,0XE7}};//the character code for the character or number(it is the code for "A" here), put your code here
  int j;
  int k;
  int stability;

  for(stability=0; stability<8; stability++)  
  {
    //Display ROW1-ROW8
    for(k=0; k<8; k++)
    { 
      digitalWrite(k+10,HIGH); //control the row of the display
      //display  ROW(k)
      for(j=0; j<8; j++)
      {  
        if(DrowCircle[radius][k]&1<<j)
          digitalWrite(9-j,HIGH);
        else
          digitalWrite(9-j,LOW);
      }
      delay(1);
      digitalWrite(k+10,LOW); //control the anode of the display
    }  
  }  
  delay(150);
}

