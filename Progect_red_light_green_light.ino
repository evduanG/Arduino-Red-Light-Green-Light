#define trigPin 2
#define echoPin 4
#define servoTrigPin 5
#define movingGunpin 6
#define led 7  
#define buzzer 10
#define led_green 12
#define led_red 13     
#include <Servo.h>

// Change to 0.5 for a slower version of the song, 1.25 for a faster version
const float songSpeed = 1.0;

float myArr[180]; // array to store the values of the distance of the ultrasonic sensor
int   pos;        // variable to store the position of the servo motor
int trgPos;
// Define pin 10 for buzzer, you can use any other digital pins (Pin 0-13)
int bullets; // variable to store the number of bullets

Servo movingGun;  // create servo object to control a servo
Servo trigServo;  // create servo object to control a servo

//int potpin = 0;  // analog pin used to connect the potentiometer
//int val;    // variable to read the value from the analog pin

//int bullets = 1; // loda bullets

long duration, distance; 

// ======================================================================================
// sond var
// Defining frequency of each music note
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_G5 784
#define NOTE_A5 880
#define NOTE_B5 988


void setup() {
  Serial.begin(9600);
  
  //ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // led s 
  pinMode(led, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
  digitalWrite(led_green, LOW);
  digitalWrite(led_red, LOW);
  digitalWrite(led, LOW);
  
  // Servo s
  movingGun.attach(movingGunpin);  // attaches the servo on pin 9 to the servo object
  trigServo.attach(servoTrigPin);

  // buzzer
  pinMode(buzzer, OUTPUT);
  
  test();
  bullets = 1; // loda bullets
  trgPos=90;
  Serial.println("The game - red light - green light");
  Serial.println("As long as the green light is on, you can move and hear music");
  Serial.println("But when the red light stops you must not move or the gun will shoot you");
}

void test() {
  Serial.println("TEST !! : RED 0N , GREEN ON , LED TO SHOT ON BUZZER BIP  " );
  digitalWrite(led_green, HIGH);
  digitalWrite(led_red, HIGH);
  digitalWrite(led, HIGH);
  digitalWrite(buzzer, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);
  digitalWrite(buzzer, LOW);    // turn the LED off by making the voltage LOW
  Serial.print("TEST !! : ultrasonic distance[cm]= " );
  getuUtrasonicDist();
  delay(1000);
  Serial.println("TEST !! : THE GUN " );
  digitalWrite(led_green, LOW);
  digitalWrite(led_red, LOW);
  digitalWrite(led, LOW);  
}

void play_song()
{
  // Music notes of the song, 0 is a rest/pulse
  int notes[] = {
      NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
      NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
      NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
      NOTE_A4, NOTE_G4, NOTE_A4, 0,
    };
  
  // Durations (in ms) of each music note of the song
  // Quarter Note is 250 ms when songSpeed = 1.0
  int durations[] = {
      125, 125, 250, 125, 125,
      125, 125, 250, 125, 125,
      125, 125, 250, 125, 125,
      125, 125, 375, 125
      };

  const int totalNotes = sizeof(notes) / sizeof(int);
  // Loop through each note
  for (int i = 0; i < totalNotes; i++)
  {
    const int currentNote = notes[i];
    float wait = durations[i] / songSpeed;
    // Play tone if currentNote is not 0 frequency, otherwise pause (noTone)
    if (currentNote != 0)
      {
        tone(buzzer, notes[i], wait); // tone(pin, frequency, duration)
      }
    else
      {
        noTone(buzzer);
      }
    // delay is used to wait for tone to finish playing before moving to next loop
    delay(wait);
  }
}

void red_light_func (){
    Serial.println ("red_light_func " );
    digitalWrite(led_red, HIGH); // sets the digital pin led_red on
    delay(1000);
    Serial .println ("Resets the array"); 
    // Reset of the data in the array
    for(pos=180;pos>=0;pos--)
      {
        movingGun.write(pos); 
        myArr[pos-1]= getuUtrasonicDist(); // enter the distance to his place in the array 
        delay(10);
      } 
   
    Serial .println ("start looking for movement");
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    movingGun.write(pos);              // tell servo to go to position in variable 'pos'
             float currentDistance= getuUtrasonicDist(); // enter the distance to his place in the array 
             
             if (myArr[pos] >currentDistance +2|| myArr[pos]<currentDistance -2)
                {
                    shoot(currentDistance,myArr[pos]);
                    break;    
                }
    
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  if (bullets>0){
      for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
          movingGun.write(pos);              // tell servo to go to position in variable 'pos'
            float currentDistance= getuUtrasonicDist(); // enter the distance to his place in the array 
               
               if (myArr[pos] >currentDistance +2|| myArr[pos]<currentDistance -2)
                  {
                      shoot(currentDistance,myArr[pos]);
                      break;    
                  }
          
          delay(15);                       // waits 15ms for the servo to reach the position
    }
  }
    delay(1000);
    digitalWrite(led_red, LOW); // sets the digital pin led_red on
}




float getuUtrasonicDist (){
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  
  // Displays the distance on the Serial Monitor
  Serial.println(distance);
  
  return distance;
}

void green_light_func () {
  // Green light = you can move + music is playing in the background
  Serial.println ("green_light_func " );
  digitalWrite(led_green, HIGH);
  play_song();
  digitalWrite(led_green, LOW);
  red_light_func();
}

void shoot(float currentDistance , float oldDistance )
{
  Serial.println("shoot!! ");
   Serial.print("oldDistance : ");
   Serial.print(oldDistance);
   Serial.print(" ,  currentDistance : ");
   Serial.print(currentDistance);
   Serial.println();
  digitalWrite(led, HIGH); 
  int angel = trigServo.read();
  angel-=10;
   while (angel>0)
   {
      trigServo.write(angel);
      Serial.println(angel);

        angel-=10;
   }



  delay(500);
  // return to 90 digre 
  trigServo.write(90);
  movingGun.write(90); 
  bullets = 0;

}


void loop()
{
  if (bullets > 0)
  {
    green_light_func();
  }
}
