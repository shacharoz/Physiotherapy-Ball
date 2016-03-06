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
const int delayAfterEachDetection = 3000;

//
const int eachFrameDelay = 300;

//threshold stuff
int min_thresh = 10000; //to cancel noise from ball moving on water. 10000 looks good enough for the purpose.
int thresh = 15000;  //threshold for a push. first value should be measured. 15000 looks good enough
  
const int samples = 5;  //number of pushes on each round
int above_thresh_counter = 0; //current pushes counter
int smallest = 0x7fffffff; //smallest value in the round (to become the next threshold





void setup() {
  Serial.begin(9600);

  CurieImu.initialize(); // initialise the IMU
  Serial.println("IMU initialisation complete, waiting for events...");

  isFeedbackActivating = false;
}

void loop() {
  /* loop returns every frame */

  PrintAccelerometer();
  
  delay(eachFrameDelay);
}

void PrintAccelerometer() {
  
  static int min_thresh = 10000; //to cancel noise from ball moving on water. 10000 looks good enough for the purpose.
  static int thresh = 15000;  //threshold for a push. first value should be measured. 15000 looks good enough
  
  static const int samples = 5;  //number of pushes on each round
  static int above_thresh_counter = 0; //current pushes counter
  static int smallest = 0x7fffffff; //smallest value in the round (to become the next threshold
  
  static int16_t x=0, y=0, z=0;
  static int16_t x1, y1, z1;
  static const int divider = 100;
  
  static const int delayAfterEachDetection = 3000;


  //get new accelerometer data. divide them by "divider" to make them easier to work with
  CurieImu.getAcceleration(&x1,&y1,&z1);
  x1 /= divider;
  y1 /= divider;
  z1 /= divider;



  if ((x != 0) && (y != 0) && (z != 0))
  {
    int sum_of_delta = (x-x1)*(x-x1) + (y-y1)*(y-y1) + (z-z1)*(z-z1); 
    Serial.println(sum_of_delta);

    if (isFeedbackActivating == false)
    {
      if (sum_of_delta > min_thresh)
      {
          //means ball moves above noise level
          Serial.println(" ball moved ");
           
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
                
                thresh = smallest - 500;
                Serial.print(" new thresh "); Serial.println(thresh);
                
                smallest = 0x7fffffff; //reset smallest amount so it could be redetected
                above_thresh_counter = 0;
              }
              
        } else {
            Serial.println(" reset above thresh counter ");
                
            above_thresh_counter = 0;
            smallest = 0x7fffffff;
        }
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

}


