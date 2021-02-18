
//Really basic Lift Kite steering 
//Getting rid of mpu6050 and using DFRobot 6dof shield instead.,

//Any value deviating fron 0 straight up maps straight to a servo steering

//roll reading is through a running smoothing matrix

//Reading an value from phone bluetooth and applying it to further offset the kite angle
// only acceptting 1 char at a time just now until we sort out parsing number data
// each offset value is case set as an offset steering angle


//STILL NEED TO ADD full PID 
//BTserial software from somewhere internety

//  Sketch: basicSerialWithNL_001
// 
//  Uses hardware serial to talk to the host computer and software serial 
//  for communication with the Bluetooth module
//  Intended for Bluetooth devices that require line end characters "\r\n"
//
//  Pins
//  Arduino 5V out TO BT VCC
//  Arduino GND to BT GND
//  Arduino D11 to BT RX through a voltage divider
//  Arduino D10 BT TX (no need voltage divider)
//
//  When a command is entered in the serial monitor on the computer 
//  the Arduino will relay it to the bluetooth module and display the result.
//

// # Editor     : Roy from DFRobot
// # Date       : 10.12.2013
// # Product name: 6 Dof shield for Arduino
// # Product SKU : DFR0209
// # Version     : 0.1
// # Description:
// # The sketch for driving the 6 Dof shield for Arduino via I2C interface

#include <FreeSixIMU.h>
#include <FIMU_ADXL345.h>
#include <FIMU_ITG3200.h>

#include <Wire.h>

int16_t angle[2]; // pitch & roll

// Set the FreeSixIMU object
FreeSixIMU sixDOF = FreeSixIMU();
int rawSixDof[6];

#include <SoftwareSerial.h>
SoftwareSerial BTserial(8, 2); // RX | TX BT05 Uno connections


short c = 0;
boolean NL = true;//4


#include <Servo.h>
Servo myservo;  // create servo object to control a servo

long val;    // variable to set as roll value read from MPU6050
int offsetANGLEselected; //a variable to set as chosen offset angle
int steer;  // variable to send to the servo 
int slowcount;  // dont send every reading over bluetooth only when this count is...
int slowamount;// only every 5 (as set in setup) over BT



const int numReadings = 9;  //size of array for smoothing roll readings 

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average



void setup() {
    Serial.begin(9600);
    Serial.print("Sketch:   ");   Serial.println(__FILE__);
    Serial.print("Uploaded: ");   Serial.println(__DATE__);
    Serial.println(" ");
 
    BTserial.begin(9600);  
    BTserial.print("BTserial started at "); BTserial.println(9600);
    BTserial.println(" ");
    
    delay (1);      //because rods messed with programme timing


  Wire.begin();

  delay(5);
  sixDOF.init();                        //begin the IMU
  delay(5);
     
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
                          readings[thisReading] = 0;} //clear array    
                          Serial.println("smoothing array ready");
      delay(10);



 
      // attaches the servo on pin 9 to the servo object
    myservo.attach(3);  
     
    delay(1);       //because rods messed with programme timing
    
    slowcount=(0);  //count reset
    slowamount=(10);  //count limit

    
    BTserial.print("Sketch:   ");   BTserial.println(__FILE__);
        delay (1);
    BTserial.print("Uploaded: ");   BTserial.println(__DATE__);
        delay (1);
    BTserial.println(" ");
  
}

void loop() {


  sixDOF.getRawValues(rawSixDof);
  angle[0] = _atan2(rawSixDof[0],rawSixDof[2]);
  angle[1] = _atan2(rawSixDof[1],rawSixDof[2]);

  Serial.print("roll X:");              //pitch & roll
  Serial.println(angle[0]/10.0);
  val = (angle[0]/10.0);
  
  Serial.print("pitch Y:");
  Serial.println(angle[1]/10.0);
  Serial.println("");
  delay(50);

  

 if (Serial.available())
    BTserial.write(Serial.read());
     delay(10);
          // Read from the Bluetooth module and send to the Arduino Serial Monitor
          if (BTserial.available())
          {
              c = BTserial.read();
                    switch(c) { 
                      //case 48: /* Number 0 */
                      case '0':
                    offsetANGLEselected = (0);     
                            break;      
                      //case 49: /* Number 1 */
                      case '1':
                    offsetANGLEselected = (5);     
                            break;
                     case '2':
                    offsetANGLEselected = (10);     
                            break;      
                      case '3':
                    offsetANGLEselected = (15);     
                            break;      
                       case '4':
                    offsetANGLEselected = (20);     
                            break;      
                      case '5':
                    offsetANGLEselected = (25);     
                            break; 
                     case '6':
                    offsetANGLEselected = (30);     
                            break;      
                      case '7':
                    offsetANGLEselected = (35);     
                            break;
                      case '8':
                    offsetANGLEselected = (40);     
                            break;      
                      case '9':
                    offsetANGLEselected = (65);     
                            break;         
                            default:
                              //Serial.print("Offset Angle: ");
                             //Serial.println(offsetANGLEselected);
                            BTserial.print("Offset Angle: ");
                            BTserial.print(offsetANGLEselected);

                            break;
                            }
                   delay(1);
           }
              
   
   delay(1);
  
    //Serial.print(val);
      
        total = total - readings[readIndex];// subtract the last reading:
        readings[readIndex] = val;// read roll value from this program:
        total = total + readings[readIndex];// add the reading to the total:
        readIndex = readIndex + 1;// advance to the next position in the array:
        
        if (readIndex >= numReadings) { // if we're at the end of the array...wrap around to the beginning:
           readIndex = 0;             }
           delay(1);        // delay in between reads for stability
           
        average = total / numReadings; // calculate the average:  
            delay(1);   

      
            steer = map(average, -40, 40, 150, 30);      // set human readable kite roll limits to servo range
            delay(1); 

            steer = (steer - offsetANGLEselected);
            if (steer <30) { steer = 30; }
            if (steer >150) { steer = 150; }
            myservo.write(steer);                        // sets the servo position according to roll 
            delay(5);                              // wait for the servo to get there

           // Serial.print("    Steer =  ");
           // Serial.println(steer);  

      slowcount++;
      
      if (slowcount >= slowamount){
                slowcount = (0);
               // BTserial.write(val);
               
                BTserial.println(average);
                BTserial.print("    Steer =  ");
                BTserial.println(steer);


                      //Serial.print(val);
                      //Serial.print("Smoothed Average Angle: ");
                      //Serial.println(average);
                      //Serial.print("    Steer =  ");
                      //Serial.println(steer);  
                }
  
delay (20);
}


int16_t _atan2(int32_t y, int32_t x)   //get the _atan2
{
  float z = (float)y / x;
  int16_t a;
  if ( abs(y) < abs(x) )
  {
    a = 573 * z / (1.0f + 0.28f * z * z);
    if (x<0)
    {
    if (y<0) a -= 1800;
    else a += 1800;
    }
  }
  else
  {
    a = 900 - 573 * z / (z * z + 0.28f);
    if (y<0) a -= 1800;
  }
  return a;
}
