
#include <WiFlyHQ.h>
#define buttonPin 2

#define canID A1

/* Change these to match your WiFi network */
const char mySSID[] = "The_Internet";
const char myPassword[] = "JustMach1n3";
//const char mySSID[] = "paintout";
//const char myKey[] = "0123456890";
const int resetPin = 11;


  

//void terminal();

WiFly wifly;


int ldr = 2;   // LDR on analog 0
int ldrVal;    // LDR Value
int capPin = 3;
int capPinState = 0;
float h;
int h_int;
int r=0, g=0, b=0;

void setup()
{
  Serial.begin(57600);      // Debugging only
  Serial.println("setup");

  // Initialise the IO and ISR
  // vw_set_ptt_inverted(true); // Required for DR3100
  // vw_setup(4000);     // Bits per sec
  //pinMode(7, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH );
  //pinMode(3, OUTPUT);
 // pinMode(5, OUTPUT);
  //pinMode(6, OUTPUT);
  pinMode (8, INPUT);
  pinMode(10, OUTPUT);
  
  char buf[32];
    pinMode(resetPin,OUTPUT);
    digitalWrite(resetPin, HIGH); 
    //Serial.begin(115200);
    //Serial.println("Starting");
    //Serial.print("Free memory: ");
    //Serial.println(wifly.getFreeMemory(),DEC);

    Serial.begin(9600);

    if (!wifly.begin(&Serial)) {
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

//    /* Ping the gateway */
//    wifly.getGateway(buf, sizeof(buf));
//
//    Serial.print("ping ");
//    Serial.print(buf);
//    Serial.print(" ... ");
//    if (wifly.ping(buf)) {
//	Serial.println("ok");
//    } else {
//	Serial.println("failed");
//    }
//
//    Serial.print("ping google.com ... ");
//    if (wifly.ping("google.com")) {
//	Serial.println("ok");
//    } else {
//	Serial.println("failed");
//    }

    /* Setup for UDP packets, sent automatically */
    wifly.setIpProtocol(WIFLY_PROTOCOL_UDP);
    wifly.setHost("192.168.0.24", 11999);	// Send UDP packet to this server and port

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

    wifly.setHost("192.168.0.24", 11999);	// Send UPD packets to this server and port

    //Serial.println("WiFly ready");
}

void loop()
{
  int rpin = 9;
  int gpin = 10;
  int bpin = 11;
  //int ledPin = 4;
  //digitalWrite(4, HIGH);
  int ldrVal = analogRead(ldr);
  float h = ((float)ldrVal)/1024;
  int h_int = (int) 360*h;
  //int r=0, g=0, b=0;
  int buttonState = digitalRead(8);
  //digitalWrite(ledPin, HIGH);
  // Serial.println(ldrVal);

  h2rgb(h,r,g,b);
  int tmp;
  tmp = analogRead(4);
  // Get button event and act accordingly
  int bu = checkButton();
  if (bu == 1) clickEvent();
  if (bu == 2) doubleClickEvent();
  if (bu == 3) holdEvent();
  if (bu == 4) longHoldEvent();
  if (bu == 5) clickHoldEvent();

  if(buttonState == HIGH){
    digitalWrite(10, HIGH);
    wifly.print(canID);
    wifly.print(".");
    wifly.print(r);
    wifly.print(",");
    wifly.print(g);
    wifly.print(",");
    wifly.print(b);
    wifly.print(".");
    wifly.println(tmp);
    //Serial.println("X");
    //vw_send((uint8_t *)bufferX, strlen(bufferX));  //actually send on RF
    //vw_wait_tx();             // Wait until the whole message is TXed
    delay(50);                // found i need a short delay before second TX
    // }
  }
  else{
    digitalWrite(10, LOW);
  }

}


void clickEvent() {
  wifly.print(canID);
  wifly.print(".");
  wifly.println("CLICK");
}
void doubleClickEvent() {
  wifly.print(canID);
  wifly.print(".");
  wifly.println("DOUBLE");
}
void holdEvent() {
  wifly.print(canID);
  wifly.print(".");
  wifly.println("HOLD");
}
void longHoldEvent() {
  //Serial.println("A2.LONG HOLD");
}
void clickHoldEvent() {
  wifly.print(canID);
  wifly.print(".");
  wifly.println("CLEAR");
}




