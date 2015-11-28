// EXAMPLE USAGE

// telnet defaults to port 23
TCPServer server = TCPServer(23);
TCPClient client;
int bytecount = 0;
char  buffer[10];
char* bp =  buffer;
int startStop = D2;
int brake = D3;
int direction = D4;
int mode = D5;
int alarmReset = D6;
int pulseCount = D7;
int alarm = A5;
int speed = 2047;
int period = 0;
bool started = false;
int DACaddress = 0x62;
Timer runtimer(0,brakeMotor);

void setup() {
  pinMode(startStop, OUTPUT);                // sets pin as output
  pinMode(brake, OUTPUT);                    // sets pin as output
  pinMode(direction, OUTPUT);                // sets pin as output
  pinMode(mode, OUTPUT);                     // sets pin as output
  pinMode(alarmReset, OUTPUT);               // sets pin as output
  pinMode(pulseCount, INPUT);                // sets pin as input
  pinMode(alarm, INPUT);                     // sets pin as input
  // start listening for clients
  server.begin();
  Wire.setSpeed(CLOCK_SPEED_400KHZ);
  Wire.begin();
  // Make sure your Serial Terminal app is closed before powering your device
  Serial.begin(9600);
  // Now open your Serial Terminal, and hit any key to continue!
  while(!Serial.available()) Particle.process();

  Serial.println(WiFi.localIP());
  Serial.println(WiFi.subnetMask());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.SSID());
}

void initPump() {
    digitalWrite(direction,HIGH);
    digitalWrite(brake,HIGH);
    digitalWrite(startStop,HIGH);
    digitalWrite(mode,HIGH);
    digitalWrite(alarmReset,HIGH);
    started = false;
    period = 0;
    pumpSpeed(2047);
}

void startMotor() {
  unbrakeMotor();
  digitalWrite(startStop,LOW);
  unpulseMotor();
  started = true;
}

void pulseMotor() {
    digitalWrite(mode,LOW);
}

void unpulseMotor() {
    digitalWrite(mode,HIGH);
}

void clkwMotor() {
    digitalWrite(direction,HIGH);
}
void cclkwMotor() {
    digitalWrite(direction,LOW);
}

void brakeMotor() {
    digitalWrite(brake,HIGH);
}

void unbrakeMotor() {
    digitalWrite(brake,LOW);
}

void setRunTime(uint16_t ms) {
    runtimer.dispose();
    Timer runtimer(ms,brakeMotor);
}

void pumpSpeed(uint16_t sval) {
  if ( !Wire.isEnabled() ) {
    Wire.begin();
  }
  Wire.beginTransmission(DACaddress); // transmit to slave device #4
  Wire.write(0x40);
  Wire.write(sval / 16);
  Wire.write((sval %16)<<4);
  Wire.endTransmission();    // stop transmitting
}

void getStatus() {
    //digitalWrite(brake,LOW);
}

void loop()
{
  if (client.connected()) {
    // echo all available bytes back to the client
    while (client.available()) {
      buffer[bytecount] = client.read();
      bytecount++;
    }
    if(bytecount > 0) {
     server.write((const unsigned char*)buffer, bytecount);
     Serial.printf("Got: %s\n",buffer);
     bytecount = 0;
   }
   switch (buffer[0]) {
    case 'A':
    case 'a':
      Serial.println("CCW");
      cclkwMotor();
      break;
    case 'b':
    case 'B':
      Serial.println("Brake");
      brakeMotor();
      break;
    case 'c':
    case 'C':
      Serial.println("CW");
      clkwMotor();
      break;
    case 'G':
    case 'g':
      Serial.println("Go");
      startMotor();
      break;
    case 'I':
    case 'i':
      Serial.println("Init");
      initPump();
      break;
    case 'N':
    case 'n':
      Serial.println("Init");
      unpulseMotor();
      break;
    case 'P':
    case 'p':
      Serial.println("Pulse");
      pulseMotor();
      break;
    case 'S':
    case 's':
      Serial.println("Speed Change");
      speed = (uint16_t) atoi(++bp);
      Serial.println(speed);
      pumpSpeed(speed);
      bp--;
      break;
    case 'T':
    case 't':
      Serial.println("Timer set");
      period = (uint16_t) atoi(++bp);
      Serial.println(period);
      setRunTime(period);
      bp--;
      break;
    case 'U':
    case 'u':
      Serial.println("unBrake");
      unbrakeMotor();
      break;
    case 'X':
    case 'x':
      Serial.println("getStatus");
      getStatus();
      break;
   }
   buffer[0] = '0';
  } else {
    // if no client is yet connected, check for a new connection
    client = server.available();
  }
}
