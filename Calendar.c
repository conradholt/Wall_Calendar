// GLOBAL INCLUDES .H
#include <Keyboard.h>

// GLOBAL DEFINES
#define SENSOR_PIN 10                                         // Digital Sensor state input pin
#define RX_LED_PIN  17                                        // The RX LED has a defined Arduino pin
#define FW_BT_PIN  6                                          // The Forward button to increment the view
#define BK_BT_PIN  7                                          // The Back button to decrement the view
#define ROTATE_BT_PIN  8                                      // The Rotation button goes through "d,w,m,y"
#define RT_TDY_BT_PIN  9                                      // The Return button returns to weekly view
#define MIN_TIME_OUT   60500                                  // Costant for the 1 minute time_out   
#define DEBOUNCE_TIME  1000                                   // Set a debounce time for the button once is pressed
//#define DEBUG
// GLOBAL VARIABLES
//static bool sensor_previous_state = false;
char cRotationCounter = 'w';                                  // Counter goes from 0 to 3: 0 -> 'w' / 1 -> 'd' / 2 -> 'y' / 3 -> 'm'
char acPassword[] = "1234";
//bool bDontRepeat = false;
unsigned long currentMillis = 0;                              // Millis since the program started running
unsigned long previousMillis = 0;                             // Starting Millis since the program started running
bool bFirstTime = false;                                      // TimeOut for the 1 min screen to go suspended
bool bLastDetection = false;                                  // Used to capture the initial time for the TIME_OUT counting

// DEFINE FUNCTIONS
void vPressKey(void);                                         // Simulates the press and release Keyboard Action
bool uiCheckButton(uint8_t button);                           // Rising and falling edge time to avoid glitch when pressing button

void vPressKey()                                          
{
  Keyboard.press(KEY_RETURN); 
  Keyboard.release(KEY_RETURN);
}

bool uiCheckButton()
{
  bool bCheckBtn = false;

  return true;
}

void setup()
{
  pinMode(SENSOR_PIN, INPUT);                                 // PIR sensor pin as input
  pinMode(RX_LED_PIN, OUTPUT);                                // RX LED as an output
  pinMode(FW_BT_PIN, INPUT);                                  // Digital Forward button as input
  pinMode(BK_BT_PIN, INPUT);                                  // Digital Back button as input
  pinMode(ROTATE_BT_PIN, INPUT);                              // Digital Rotate button as input
  pinMode(RT_TDY_BT_PIN, INPUT);                              // Digital Return button as input

  Keyboard.begin();                                           // Allow Arduino to take control of Keyboard
  digitalWrite(RX_LED_PIN, HIGH);                             //  RX LED off       
  TXLED0;                                                     // switch on TX green to show init/powered up (only available by macro)
  //sensor_previous_state = digitalRead(SENSOR_PIN);          // get sensor initial state (can be already true)
#ifdef DEBUG
Serial.begin(9600);                                           // open the serial port at 9600 bps:
#endif

  delay(5000);                                                // To avoid wrong PIR reading at the beginning
}

void loop()
{
  bool sensor_current_state = digitalRead(SENSOR_PIN);        // Get the actual state of the sensor
  //bool button_check = false;                                // Boolean used to know if the button has been really pressed or not
  //int button_state = 0;
  //if (sensor_previous_state == false                        // looping until we detect a rising edge
       //&& sensor_current_state == true)
  if(sensor_current_state)                                    // If we detect an object then do stuff
  {                                                           // when sensor state is trigged, it takes about 20 sec to recover
    //USBDevice.wakeupHost();                                 // Wake up the host 
    Keyboard.press(KEY_LEFT_CTRL);                            // Wake up the host
    Keyboard.release(KEY_LEFT_CTRL);
    
    if(!bLastDetection) bLastDetection = true;

    //if(abs((currentMillis-previousMillis)) == 0)              // First time therefore we must press backspace, and 
    if(!bFirstTime)                                           // First time, therefore we must press backspace, and password (LINUX)
    {
      bFirstTime = true;                                      // We don't want the system to enter again
      digitalWrite(RX_LED_PIN, LOW);                          // set the LED on
      //Keyboard.press( KEY_CAPS_LOCK );
      //Keyboard.release( KEY_CAPS_LOCK );

      Keyboard.press(KEY_BACKSPACE);                          // Open Ubuntu save screen
      Keyboard.release(KEY_BACKSPACE);                        // Release Space bar
    
      Keyboard.write('1');                                    // Enter Password ASCII format
      Keyboard.write('2');
      Keyboard.write('3');
      Keyboard.write('4');
      //Keyboard.print(acPassword);
      vPressKey();
      //Keyboard.press(KEY_RETURN);                           // Press Enter key
      //Keyboard.release(KEY_RETURN);                         // Release the Enter key
    
      delay(1500);
    
      cRotationCounter = 'w';                                 // Restart the Counter
      Keyboard.write('w');
      vPressKey();
    
    }
  
    TXLED0;                                                   // great hackery in this: we have to force down the TXLED
    delay(1000);                                              // wait a bit for the led

  } 
  else                                                        // No detection
  {

    if(bLastDetection)                                        // Done to avoid refreshing time everytime we detect presence
    {
      bLastDetection = false;                                 // Set back the boolean to false so we don't repeat the refreshing
      previousMillis = millis();                              // Capture the last time since the object detected a person

    }
    if(bFirstTime)                                            // At least we have detected a presence once  
    {
      currentMillis = millis();                               // Capture the time
      if(abs(currentMillis-previousMillis) >= MIN_TIME_OUT)
      {
        digitalWrite(RX_LED_PIN, HIGH);                       // set the LED off
        TXLED0;
        bFirstTime = false;                                   // The timeout has happen
        previousMillis = 0;                                   // Reset the Milli couner   
        currentMillis = 0;
  
      }
    }
    
  }
  if(bFirstTime)                                              // Button Management 
  {
    //button_check = uiCheckButton();                         // If button check is successful then do the rest, if not skip.
    //button_state = analogRead(ROTATE_BT_PIN);
#ifdef DEBUG    
    Serial.print(button_state);
#endif    
    //if(digitalRead(ROTATE_BT_PIN) == LOW)
    if(digitalRead(ROTATE_BT_PIN == LOW))
    { 
      switch(cRotationCounter)
      {
        case 'w':
          cRotationCounter = 'm';
          Keyboard.write(cRotationCounter);
          delay(DEBOUNCE_TIME);                                      
        break;

        case 'm':
          cRotationCounter = 'y';
          Keyboard.write(cRotationCounter);
          delay(DEBOUNCE_TIME);
        break;

        case 'y':
          cRotationCounter = 'd';
          Keyboard.write(cRotationCounter);
          delay(DEBOUNCE_TIME);
        break;

        case 'd':
          cRotationCounter = 'w';
          Keyboard.write(cRotationCounter);
          delay(DEBOUNCE_TIME);
        break;
        
        default:
          cRotationCounter = 'w';
          Keyboard.write(cRotationCounter);
          delay(DEBOUNCE_TIME);
        break;
      }
    }
    else if (digitalRead(FW_BT_PIN))
    {
      Keyboard.write('n');
      delay(DEBOUNCE_TIME);
    }
    else if (digitalRead(BK_BT_PIN))
    {
      Keyboard.write('p');
      delay(DEBOUNCE_TIME);
    }
    else if (digitalRead(RT_TDY_BT_PIN))
    {
      Keyboard.write('t');
      delay(DEBOUNCE_TIME);
    }   
  }
}