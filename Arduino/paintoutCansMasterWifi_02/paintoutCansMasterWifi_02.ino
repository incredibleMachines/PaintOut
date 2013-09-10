
#include <WiFlyHQ.h>
#define frontButton 2
#define capButton 12
#define potInput A2
#define potGround A3
#define potPower A5
#define sharpInput A0
#define irLED 8


#define canID A1

/* Change these to match your WiFi network */
const char mySSID[] = "expressnet";
const char myPassword[] = "thisis4demo";
//const char mySSID[] = "paintout";
//const char myKey[] = "0123456890";
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
  //Serial.begin(57600);      // Debugging only
  //Serial.println("setup");

  // Initialise the IO and ISR
  // vw_set_ptt_inverted(true); // Required for DR3100
  // vw_setup(4000);     // Bits per sec
  //pinMode(7, OUTPUT);
  pinMode(frontButton, INPUT);
  digitalWrite(frontButton, HIGH );
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW );
  pinMode(A3,OUTPUT);
  pinMode(A5,OUTPUT);
  digitalWrite(A5,HIGH);
  digitalWrite(A3,LOW);
  pinMode(13,OUTPUT);
  pinMode(capButton,INPUT);
  digitalWrite(capButton,HIGH);
  digitalWrite(13,LOW);
  //pinMode(6, OUTPUT);
  pinMode(irLED,OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  
  char buf[32];
//    pinMode(resetPin,OUTPUT);
//    digitalWrite(resetPin, HIGH); 
    //Serial.begin(115200);
    //Serial.println("Starting");
    //Serial.print("Free memory: ");
    //Serial.println(wifly.getFreeMemory(),DEC);

    Serial.begin(9600);

    if (!wifly.begin(&Serial)) {
        //Serial.println("Failed to start wifly");
	//terminal();
    }
//    else{
//      wifly.print("factory RESET");
//    }

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
    wifly.setHost("10.0.1.2", 11999);	// Send UDP packet to this server and port

//    Serial.print("MAC: ");
//    Serial.println(wifly.getMAC(buf, sizeof(buf)));
//    Serial.print("IP: ");
//    Serial.println(wifly.getIP(buf, sizeof(buf)));
//    Serial.print("Netmask: ");
//    Serial.println(wifly.getNetmask(buf, sizeof(buf)));
//    Serial.print("Gateway: ");
//    Serial.println(wifly.getGateway(buf, sizeof(buf)));

    wifly.setDeviceID("Paintout-UDP");
    //Serial.print("DeviceID: ");
    //Serial.println(wifly.getDeviceID(buf, sizeof(buf)));

     wifly.setHost("10.0.1.2", 11999);	// Send UDP packet to this server and port

    //Serial.println("WiFly ready");
}

void loop()
{
//  int rpin = 9;
//  int gpin = 10;
//  int bpin = 11;
  //int ledPin = 4;
  //digitalWrite(4, HIGH);
  int ldrVal = analogRead(potInput);
  float h = ((float)ldrVal)/1024;
  int h_int = (int) 360*h;
  //int r=0, g=0, b=0;
  int buttonState = digitalRead(capButton);
  //digitalWrite(ledPin, HIGH);
  // Serial.println(ldrVal);

  h2rgb(h,r,g,b);
  int tmp;
  tmp = analogRead(sharpInput);
  // Get button event and act accordingly
  int bu = checkButton();
  if (bu == 1) clickEvent();
  if (bu == 2) doubleClickEvent();
  if (bu == 3) holdEvent();
  if (bu == 4) longHoldEvent();
  if (bu == 5) clickHoldEvent();

  if(buttonState == LOW){
    digitalWrite(irLED, HIGH);
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
    //Serial.println("X");
    //vw_send((uint8_t *)bufferX, strlen(bufferX));  //actually send on RF
    //vw_wait_tx();             // Wait until the whole message is TXed
    delay(50);                // found i need a short delay before second TX
  }
  else{
    digitalWrite(irLED, LOW);
  }

}


void clickEvent() {
  wifly.print(canID);
  wifly.print(".");
  wifly.print("CLICK");
  wifly.print("[/p]");
}
void doubleClickEvent() {
  wifly.print(canID);
  wifly.print(".");
  wifly.print("DOUBLE");
  wifly.print("[/p]");
}
void holdEvent() {
  wifly.print(canID);
  wifly.print(".");
  wifly.print("HOLD");
  wifly.print("[/p]");
}
void longHoldEvent() {
  //Serial.println("A2.LONG HOLD");
}
void clickHoldEvent() {
  wifly.print(canID);
  wifly.print(".");
  wifly.print("CLEAR");
  wifly.print("[/p]");
}





