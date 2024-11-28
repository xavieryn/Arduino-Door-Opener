#include <ArduinoBLE.h>
#include <ezButton.h>

// ezButton for limit switches
ezButton limitSwitchHandle(8); // Door handle
ezButton limitSwitchTop(9); // Top of range

 // Motor B connections (Connected to the L298N)
int enB = 3;
int in3 = 5;
int in4 = 4;

bool goingUp = false;


BLEService MotorService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy Motor Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup() {
  // Lets me see print to a console log
  Serial.begin(9600);
  while(!Serial);

  // Set up all the limit switches
  limitSwitchHandle.setDebounceTime(1);
  limitSwitchTop.setDebounceTime(1);


	// Set all the motor control pins to outputs
	pinMode(enB, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
	
	// Turn off motors - Initial state
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);

  if(!BLE.begin()){
    Serial.println("Starting Bluetooth Low Energy Module Failed!");

    while(1);
  }

  BLE.setLocalName("Room 315 Door Opener");
  BLE.setAdvertisedService(MotorService); 

  MotorService.addCharacteristic(switchCharacteristic);

  BLE.addService(MotorService);

  switchCharacteristic.writeValue(0);
  
  // Starts showing as an available device?
  BLE.advertise(); 

  Serial.println("BLE Motor Peripheral");

  bool diconnectedNotif = false;
}

void loop() {
  BLEDevice central = BLE.central();

  // Test for limit switch
  limitSwitchHandle.loop();
  limitSwitchTop.loop();

  if(central){ 
    Serial.print("Connected to central ");
    Serial.println(central.address());

    if(central.connected()){
        if (limitSwitchHandle.isPressed()){
    // run motor
           Serial.println("Limit Switch Handle is pressed");
  } 
  if (limitSwitchHandle.isReleased()){
      Serial.println("Limit Switch Handle is released");
  }
  if (limitSwitchTop.isPressed()){
    // run motor
           Serial.println("Limit Switch Top is pressed");
  }
        if (switchCharacteristic.value()){
          Serial.println(switchCharacteristic.value());
          if (!goingUp && !limitSwitchHandle.isPressed()){
            runMotor("down");
            Serial.println("Going Down");
          }  else if (limitSwitchHandle.isPressed()){
            //delay(5000);
            runMotor("up");
            goingUp = true;
            Serial.println("Going Up");
          } else if (limitSwitchTop.isPressed()) {
            goingUp == false;
            turnOffMotor();
            Serial.println("Reached the Top");
          }
        }   
    }
    else{
      Serial.println("Disconnected");
    }
  }

   // when the central disconnects, print it out:
  //  if (!disconnectedNotif){
  //    Serial.print(F("Disconnected from central: "));
  //     Serial.println(central.address());
  //     disconnectedNotif = true;
  //  }
   
	// directionControl();
	// delay(1000);
	// speedControl();
	// delay(1000);
}

// This function lets you control spinning direction of motors
void runMotor(String direction) {
	// Set motors to maximum speed
	// For PWM maximum possible values are 0 to 255
	analogWrite(enB, 255);

  if (direction == "up"){
    // Turn on motor B
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    delay(2000);
  } else if (direction == "down") {	
    // Now change motor directions
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    delay(2000);
  }
}

void turnOffMotor(){
  	// Turn off motors
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
}

// This function lets you control speed of the motors
void speedControl() {
	// Turn on motors
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
	
	// Accelerate from zero to maximum speed
	for (int i = 0; i < 256; i++) {
		analogWrite(enB, i);
		delay(20);
	}
	
	// Decelerate from maximum speed to zero
	for (int i = 255; i >= 0; --i) {
		analogWrite(enB, i);
		delay(20);
	}
	
	// Now turn off motors
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
}