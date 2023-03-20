#include <ADCTouch.h>
#include <IRLibAll.h>

//Create a receiver object to listen on pin 2
IRrecvPCI myReceiver(2);

//Create a decoder object 
IRdecode myDecoder;  

IRsend mySender;
int ref0, ref1,ref2, ref3, ref4;     //reference values to remove offset



int pressed = 1;
int heldDown = 0;

long customButton1[3] = {};
long customButton101[3] = {};

long prevVal = 0;

int counter = 0;
//unsigned long previousMillis = 0;



void setup() {
  Serial.begin(9600);
  delay(2000); while (!Serial); //delay for Leonardo
  myReceiver.enableIRIn(); // Start the receiver
  Serial.println(F("Ready to receive IR signals"));

  // put your setup code here, to run once:
  ref0 = ADCTouch.read(A4, 500);    //create reference values to 
  ref1 = ADCTouch.read(A5, 500);    //account for the capacitance of the
  ref2 = ADCTouch.read(A3,500);
  ref3 = ADCTouch.read(A2, 500);
  ref4 = ADCTouch.read(A1, 500);

}

void loop() {
  //currentMillis = millis();
  // put your main code here, to run repeatedly:
  int value0 = ADCTouch.read(A4);   //no second parameter
  int value1 = ADCTouch.read(A5);   //   --> 100 samples
  int value2 = ADCTouch.read(A3);
  int value3 = ADCTouch.read(A2);
  int value4 = ADCTouch.read(A1);
  value0 -= ref0;       //remove offset
  value1 -= ref1;
  value2 -= ref2;
  value3 -= ref3;
  value4 -= ref4;


  if (value4 >= 100) //purple wire
  {
    if (heldDown == 0)
    {
      
      Serial.println("Test1");
      mySender.send(SONY, 1168, 12);
      //mySender.send(SONY, 1168, 20);
      //mySender.send(SONY, 1168, 20);
    }
    heldDown = 1;
  }

  if (value1 >= 100)
  {
    if (heldDown == 0)
    {
      mySender.send(NEC, 16712445, 32);
      Serial.println("Power button");
      
    }
    heldDown = 1;

  }
  else if (value0 >= 100)
  {
    if (heldDown == 0)
    {
      mySender.send(NEC, 16755285, 32);
      Serial.println("Blue button");      
    }

    heldDown = 1;   
  }
   else if (value2 >= 100)
  {
    if (heldDown == 0)
    {
      pressed = pressed * -1;
      Serial.println("Receiver Mode ON/ OFF");
      Serial.println(pressed); 
    }
    heldDown = 1;
    //mySender.send(NEC, 16722645, 32);
    
  }
  
  else if (value3 >= 100) //other blue wire
  {
    if (heldDown == 0)
    {
      if (customButton101[0] != 0)
      {
        Serial.println("Custom Button 101");
        for (int i =0; i<3; i++){
        mySender.send(SONY, customButton101[i], 12);
        Serial.println(customButton101[i]);
        delay(400);     
        }
      }
      else
      {
        Serial.println("Button is not mapped yet!");
      }

    }
    heldDown = 1;
    
  }
  else{
    heldDown = 0;
  }
  if (pressed == -1)
  {
    if (myReceiver.getResults()) {
    myDecoder.decode();           //Decode it
    //Serial.println(myDecoder.value); //prints the decimal value a a button press
    myReceiver.enableIRIn();      //Restart receiver
    //delay(1000);
    }
    if (myDecoder.value != 4294967295 && myDecoder.value != 0) //Ignores useless data 
    {
      if (counter < 3)
      {
        if (prevVal != myDecoder.value)
        {
          customButton1[counter] = myDecoder.value;
          Serial.println(customButton1[counter]);
          //myDecoder.value = 0;
          counter += 1;
          prevVal = myDecoder.value;
          myDecoder.value = 0;
          Serial.println(prevVal);
          delay(800);
        }
        //Serial.println(myDecoder.value);

        //delay(800);     
      }
 
      //prevVal = 0;
   
    }
      if (counter > 2)
      {
        Serial.println("Press Button to Map");
        Serial.print(customButton1[0]);
        Serial.print("  ");
        Serial.print(customButton1[1]);
        Serial.print("  ");
        Serial.println(customButton1[2]);
        Serial.println("----------------------------------");

        if (value3 >= 100){
          //customButton1[0] = 101; //ID for custom button 1
          for (int i = 0; i <3 ; i++)
          {
            customButton101[i] = customButton1[i];
          }
          pressed = pressed * -1;
          counter = 0;
          delay(500);
        }
      }     
      
  }

  //myDecoder.value = 0; 
  Serial.println(prevVal);
  
}
