/*
 * Copyright (c) 2015 Intel Corporation.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "CurieImu.h"

//feedback stuff
bool isFeedbackActivating = false;

//
const int eachFrameDelay = 25;

//threshold stuff
int min_thresh = 10000; //to cancel noise from ball moving on water. 10000 looks good enough for the purpose.
int thresh = 15000;  //threshold for a push. first value should be measured. 15000 looks good enough
  
const int samples = 5;  //number of pushes on each round
int above_thresh_counter = 0; //current pushes counter
int smallest = 0x7fffffff; //smallest value in the round (to become the next threshold




//LIGHT EFFECT VARIABLES
int steps;
int maxSteps = 5; //sets number of time the effect circles




void setup() {
  Serial.begin(19200);

  CurieImu.initialize(); // initialise the IMU
  Serial.println("IMU initialisation complete, waiting for events...");

  initLights();
  
  isFeedbackActivating = false;
}

void loop() {
  /* loop returns every frame */

  PrintAccelerometer();
  
  delay(eachFrameDelay);
}
int local_maximum_value;

void PrintAccelerometer() {
  
  static int min_thresh = 10000; //to cancel noise from ball moving on water. 10000 looks good enough for the purpose.
  static int thresh = 15000;  //threshold for a push. first value should be measured. 15000 looks good enough
  
  static const int samples = 5;  //number of pushes on each round
  static int above_thresh_counter = 0; //current pushes counter
  static int smallest = 0x7fffffff; //smallest value in the round (to become the next threshold
  
  static int16_t x=0, y=0, z=0;
  static int16_t x1, y1, z1;
  static const int divider = 100;
  
  static const int delayAfterEachDetection = 300;

  static const int n = 3;
  static int consecutive_counter = 0;
  static int maximum_delta = 0;
  
  //get new accelerometer data. divide them by "divider" to make them easier to work with
  CurieImu.getAcceleration(&x1,&y1,&z1);
  x1 /= divider;
  y1 /= divider;
  z1 /= divider;



  if ((x != 0) && (y != 0) && (z != 0))
  {
    int sum_of_delta = (x-x1)*(x-x1) + (y-y1)*(y-y1) + (z-z1)*(z-z1); 

    if (isFeedbackActivating == false)
    {
      if (sum_of_delta > min_thresh)
      {
          // check n consecutive samples above min_thresh
          consecutive_counter = consecutive_counter + 1;
           Serial.print(" - "); Serial.print(consecutive_counter); Serial.print(" - "); Serial.print(sum_of_delta); Serial.print(" - "); Serial.print(min_thresh); Serial.print(" - ");Serial.println(thresh);
          if ( consecutive_counter < n)
           {
              if(sum_of_delta > maximum_delta)
              {
                maximum_delta = sum_of_delta;
              }
              
           } else { // n consecutive samples happened

              consecutive_counter = 0;
                  
              sum_of_delta = maximum_delta;
              maximum_delta = 0;
          
              //means ball moves above noise level
              Serial.print(" ball moved : "); Serial.println(sum_of_delta);
               
              if (sum_of_delta > thresh)
              {
                  //add 1 to the samples counter
                  above_thresh_counter++;
      
                  // means user pushes hard enough
                  ActivateFeedback(above_thresh_counter, samples);
    
                  //zero values after each push
                  x=0; y=0; z=0;
                  x1=0; y1=0; z1=0;
    
                  //delay everything before you do that again
                  delay (delayAfterEachDetection);
                  isFeedbackActivating = false;
                
                  
                  //save the smallest sample (it will be the next threshold bar)
                  if (sum_of_delta < smallest)
                  {
                      smallest = sum_of_delta;
                  }
                
                  if (above_thresh_counter >= samples)
                  {
                    //finished a round of good pushes. reset the threshold
      
                   //only for now remove the learning aspect   
              //     thresh = smallest - 500;
                    Serial.print(" new thresh "); Serial.println(thresh);
    
                    //reset the effect steps
                    steps = 0;
                    
                    smallest = 0x7fffffff; //reset smallest amount so it could be redetected
                    above_thresh_counter = 0;
                  }
                  
            } else {
                Serial.println(" reset above thresh counter ");
    
                delay(delayAfterEachDetection);
                above_thresh_counter = 0;
                smallest = 0x7fffffff;
            }
           }
      } else
      {
        //Serial.print(" Z "); Serial.print(consecutive_counter); Serial.print(" - "); Serial.print(sum_of_delta); Serial.print(" - "); Serial.print(min_thresh); Serial.print(" - ");Serial.println(thresh);
        consecutive_counter = 0;
        maximum_delta = 0;
      }
    }
  }
    
  x = x1;
  y = y1;
  z = z1;
}




bool start;

void ActivateFeedback(int counter, int maxCount){

    isFeedbackActivating = true;
    
    //just a print feedback
    if (start)
    {
      Serial.print(" - lights !"); Serial.print(counter); Serial.print(" out of "); Serial.println(maxCount);
    } else {
      Serial.print(" | lights !"); Serial.print(counter); Serial.print(" out of "); Serial.println(maxCount);
    }
    start = !start;

    //start effect in second arduino
    StartEffect();

}







//LIGHT EFFECT CODE

void initLights(){
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





