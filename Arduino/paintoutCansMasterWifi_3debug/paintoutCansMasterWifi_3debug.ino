
#include <WiFlyHQ.h>
#define frontButton 2
#define frontLED 3
#define capButton 7
#define capButtonGND 6
#define potInput A0
#define potGround A1
#define potPower A2
#define sharpInput A5
#define irLED 5
#define redLED 9
#define greenLED 10
#define blueLED 11


#define canID A2

/* Change these to match your WiFi network */
//const char mySSID[] = "expressnet";
//const char myPassword[] = "thisis4demo";
//const char mySSID[] = "paintout";
//const char myKey[] = "0123456890";
const char mySSID[] = "localnet";
const char myPassword[] = "Just4Machines";
const int resetPin = 11;


  

//void terminal();

WiFly wifly;


int ldr = 2;   // LDR on analog 0
int ldrVal;    // LDR Value
//int capButton = 3;
int capButtonState = 0;
float h;
int h_int;
int r=0, g=0, b=0;

void setup()
{
  Serial.begin(57600);      // Debugging only
//  while(!Serial){
//    ;
//  }
  Serial.println("setup");
  //FRONT BUTTON
  pinMode(frontButton, INPUT);
  digitalWrite(frontButton, HIGH );
  //CAP BUTTON
  pinMode(capButtonGND, OUTPUT);
  digitalWrite(capButtonGND, LOW);
  pinMode(capButton,INPUT);
  digitalWrite(capButton,HIGH);
  //POT
  pinMode(potPower,OUTPUT);
  digitalWrite(potPower,HIGH);
  pinMode(potGround,OUTPUT);
  digitalWrite(potGround,LOW);
  //IR LED
  pinMode(irLED,OUTPUT);
  digitalWrite(irLED,HIGH);

  char buf[32];
    Serial.println("Starting");
    Serial.print("Free memory: ");
    Serial.println(wifly.getFreeMemory(),DEC);

    Serial1.begin(9600);

    if (!wifly.begin(&Serial1,&Serial)) {
        //Serial.println("Failed to start wifly");
	//terminal();
    }
    if (wifly.getFlushTimeout() != 10) {
        //Serial.println("Restoring flush timeout to 10msecs");
        wifly.setFlushTimeout(10);
	wifly.save();
	wifly.reboot();
    }

    /* Join wifi network if not already associated */
    if (!wifly.isAssociated()) {
	/* Setup the WiFly to connect to a wifi network */
	//Serial.println("Joining network");
	wifly.setSSID(mySSID);
	wifly.setPassphrase(myPassword);
        //wifly.setKey(myKey);
	wifly.enableDHCP();
        wifly.setJoin(WIFLY_WLAN_JOIN_AUTO);
        wifly.save();

	if (wifly.join()) {
	    //Serial.println("Joined wifi network");
	} else {
	    //Serial.println("Failed to join wifi network");
	    //terminal();
	}
    } else {
        //Serial.println("Already joined network");
    }


//    /* Setup for UDP packets, sent automatically */
    wifly.setIpProtocol(WIFLY_PROTOCOL_UDP);
    wifly.setHost("10.0.1.100", 11999);	// Send UDP packet to this server and port
    wifly.setDeviceID("Paintout-A2");
    wifly.setHost("10.0.1.100", 11999);	// Send UDP packet to this server and port

}

void loop()
{
  //SET THE RGB BASED ON THE POT VAL
  int ldrVal = analogRead(potInput);
  float h = ((float)ldrVal)/1024;
  int h_int = (int) 360*h;
  h2rgb(h,r,g,b);

  // Get button event and act accordingly
  int bu = checkButton();
  if (bu == 1) clickEvent();
  if (bu == 2) doubleClickEvent();
  if (bu == 3) holdEvent();
  if (bu == 4) longHoldEvent();
  if (bu == 5) clickHoldEvent();
  
  //GET THE PROX READING
  int tmp;
  tmp = analogRead(sharpInput);
  
  //CHECK THE CAP BUTTON
  int buttonState = digitalRead(capButton);
  if(buttonState == LOW){
    digitalWrite(irLED, LOW); //SYNC not source
    wifly.print(canID);
    wifly.print(".");
    wifly.print(r);
    wifly.print(",");
    wifly.print(g);
    wifly.print(",");
    wifly.print(b);
    wifly.print(".");
    wifly.print(tmp);
    wifly.print("[/p]");
    Serial.print(canID);
    Serial.print(".");
    Serial.print(r);
    Serial.print(",");
    Serial.print(g);
    Serial.print(",");
    Serial.print(b);
    Serial.print(".");
    Serial.print(tmp);
    Serial.print("[/p]");
    delay(10);                // found i need a short delay before second TX
  }
  else{
    digitalWrite(irLED, HIGH); //sync not source
  }

}

//THESE ARE ALL BUTTON EVENTS
void clickEvent() {
  wifly.print(canID);
  wifly.print(".");
  wifly.print("CLICK");
  wifly.print("[/p]");
  Serial.print(canID);
  Serial.print(".");
  Serial.print("CLICK");
  Serial.print("[/p]");
}
void doubleClickEvent() {
  wifly.print(canID);
  wifly.print(".");
  wifly.print("DOUBLE");
  wifly.print("[/p]");
  Serial.print(canID);
  Serial.print(".");
  Serial.print("DOUBLE");
  Serial.print("[/p]");
}
void holdEvent() {
  wifly.print(canID);
  wifly.print(".");
  wifly.print("HOLD");
  wifly.print("[/p]");
  Serial.print(canID);
  Serial.print(".");
  Serial.print("HOLD");
  Serial.print("[/p]");
}
void longHoldEvent() {
  //Serial.println("A2.LONG HOLD");
}

void clickHoldEvent() {
  wifly.print(canID);
  wifly.print(".");
  wifly.print("CLEAR");
  wifly.print("[/p]");
  Serial.print(canID);
  Serial.print(".");
  Serial.print("CLEAR");
  Serial.print("[/p]");
}





