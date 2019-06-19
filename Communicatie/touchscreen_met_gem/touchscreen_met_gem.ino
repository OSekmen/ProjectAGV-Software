
#include <VL53L0X.h>
#include <Wire.h>


//char text[20] = {0};

String gridX = "ABCDEFG"; // X-axis names
String gridY = "012"; //Y-axis names
String text; //var String for raw input
String command; //var String for editted input
int input; //var int for raw input
bool magnet = true; //var int for raw input
int interval = 65; // barrel spacing
int j = 0;
double millisnow;
double previousmillis;
VL53L0X sensorX;
VL53L0X sensorY;
VL53L0X sensorZ;
int idle = 31;
int oppositie = 23;
int oppakken = 25;
int lossen = 29;
int magneetactief = 27;

int sensorXreset = 8;
int sensorYreset = 9;
int sensorZreset = 10;
int RPWM_OutputX = 2;
int LPWM_OutputX = 3;
int RPWM_OutputY = 4;
int LPWM_OutputY = 5;
int RPWM_OutputZ = 6;
int LPWM_OutputZ = 7;
float gemiddeldeAantal = 5;
int posX[5];
int posY[5];
int posZ[5];

class Vector {
  public:
    int x;
    int y;
    int z;
    Vector () {}
    Vector(int _x, int _y, int _z) {
      x = _x;
      y = _y;
      z = _z;
    }

    bool operator == (Vector& other) {
      if (x == other.x && y == other.y && z == other.z) {
        return 1;
      }
      return 0;
    }

    Vector operator + (Vector& other) {
      return Vector(other.x + x, other.y + y, other.z + z);
    }

    Vector operator - (Vector& other) {
      return Vector(x - other.x, y - other.y, z - other.z);
    }

};


Vector pwm = Vector(0, 0, 0);
//Vector target = Vector(sensorX.readRangeSingleMillimeters(), sensorY.readRangeSingleMillimeters(), sensorZ.readRangeSingleMillimeters());
Vector target = Vector(75, 75, 50);
Vector positie = Vector(0, 0, 0);

void setup() {
  Wire.begin(); //init I2C

  ////////////////////////////Assign each sensor different I2C adress.
  pinMode(sensorXreset, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(31, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(sensorYreset, OUTPUT);
  pinMode(sensorZreset, OUTPUT);
  pinMode(RPWM_OutputX, OUTPUT);
  pinMode(LPWM_OutputX, OUTPUT);
  pinMode(RPWM_OutputY, OUTPUT);
  pinMode(LPWM_OutputY, OUTPUT);
  pinMode(RPWM_OutputZ, OUTPUT);
  pinMode(LPWM_OutputZ, OUTPUT);

  sensorSet();

  target = Vector(sensorX.readRangeSingleMillimeters(), sensorY.readRangeSingleMillimeters(), sensorZ.readRangeSingleMillimeters());

  ////////////////////////////////////////////
  Serial2.begin(115200);
  Serial1.begin(9600);
  Serial.begin(9600);


}

void loop() {
  millisnow = millis();

  gemiddelde();
  inputCode();

  ///////////////////////////////////Schrijven nextion
  if (millisnow - previousmillis > 1) {

    writeToNextion();
  }
  ///////////////////////////////////Motor aansturen
  if (positie.x > target.x - 615&& positie.x < target.x + 15
      && positie.y > target.y - 6 && positie.y < target.y + 6
      && positie.z > target.z - 15 && positie.z < target.z + 15) {
    digitalWrite(oppositie, HIGH);
    digitalWrite(idle, HIGH);
    if (digitalRead(magneetactief) == HIGH) {
      digitalWrite(idle, LOW);
    }
  } else {
    digitalWrite(oppositie, LOW);
    digitalWrite(idle, LOW);
  }

  if (positie.z > target.z - 15 && positie.z < target.z + 15) {
    digitalWrite(lossen, LOW);
    digitalWrite(oppakken, LOW);
  } else {
    if (digitalRead(magneetactief) == HIGH) {
      digitalWrite(lossen, HIGH);
      digitalWrite(oppakken, LOW);
    } else {
      digitalWrite(oppakken, HIGH);
      digitalWrite(lossen, LOW);
    }
  }
  
  motorAansturen(positie.x,
                 target.x,
                 pwm.x,
                 LPWM_OutputX,
                 RPWM_OutputX,
                 5,
                 100,
                 80,
                 255,
                 "c0",
                 "bt0");

  motorAansturen(positie.y,
                 target.y,
                 pwm.y,
                 LPWM_OutputY,
                 RPWM_OutputY,
                 3,
                 100,
                 100,
                 255,
                 "c1",
                 "bt1");

  motorAansturen(positie.z,
                 target.z,
                 pwm.z,
                 LPWM_OutputZ,
                 RPWM_OutputZ,
                 10,
                 100,
                 255,
                 255,
                 "c2",
                 "bt2");
}


void motorAansturen(int positie, //current position X-axis
                    int target, //target position X-axis
                    int pwm, //the current pwm value X-motor
                    int lPwm, //Left motor driver pin
                    int rPwm, //Right motor driver pin
                    int stopMarge, //distance motor stop
                    int marge, //Distance motor ramp down
                    int maxPWM, //Minimum pwm
                    int minPWM, //Maximum pwm
                    String checkbox1, //Checkbox1 element name on nextion
                    String checkbox2) { //Checkbox2 element name on nextion
  if ((positie < target - stopMarge) || (positie > target + stopMarge)) {

    if (positie < target - stopMarge) { //if not in position, uncheck the box on nextion
      nexWriteVal("Aflees", checkbox1, 0);
      nexWriteVal("Aflees", checkbox2, 0);

      if (positie < target - marge) { //if position is in left marge, slow motor down
        pwm = 255;
      } else if (positie > target - marge && positie < target) {
        pwm = map(positie, target - marge, target, minPWM, maxPWM);
      }
      analogWrite(lPwm, 0);
      analogWrite(rPwm, pwm);
    }
    else if (positie > target + stopMarge) {//if position is in right marge, slow motor down
      nexWriteVal("Aflees", checkbox1, 0);
      nexWriteVal("Aflees", checkbox2, 0);
      if (positie > target + marge) {
        pwm = 255;
      } else if (positie < target + marge && positie > target) {
        pwm = map(positie, target, target + marge, maxPWM, minPWM);
      }

      analogWrite(rPwm, 0);
      analogWrite(lPwm, pwm);
    }
  } else {
    nexWriteVal("Aflees", checkbox1, 1);//if in position, check the box on nextion
    nexWriteVal("Aflees", checkbox2, 1);
    analogWrite(rPwm, 0);//if in position, stop motors
    analogWrite(lPwm, 0);

  }

}
void sensorSet() {
  //reset all sensors by pulling shutdownpins low and high (HIGH = active)
  digitalWrite(sensorXreset, LOW);
  digitalWrite(sensorYreset, LOW);
  digitalWrite(sensorZreset, LOW);
  delay(10);
  digitalWrite(sensorXreset, HIGH);
  digitalWrite(sensorYreset, HIGH);
  digitalWrite(sensorZreset, HIGH);
  delay(10);

  //keep X-axis sensor on by pulling others LOW, assign new adress
  digitalWrite(sensorYreset, LOW);
  digitalWrite(sensorZreset, LOW);
  sensorX.init();
  sensorX.setAddress(0x30);
  sensorX.setTimeout(0);

  //repeat on Y-axis
  digitalWrite(sensorYreset, HIGH);
  sensorY.init();
  sensorY.setAddress(0x31);
  sensorY.setTimeout(0);

  //repeat on Z-axis
  digitalWrite(sensorZreset, HIGH);
  sensorZ.init();
  sensorZ.setAddress(0x32);
  sensorZ.setTimeout(0);

  //target = Vector(sensorX.readRangeSingleMillimeters(), sensorY.readRangeSingleMillimeters(), sensorZ.readRangeSingleMillimeters());


}


void gemiddelde() {
  int posXgem = 0;
  int posYgem = 0;
  int posZgem = 0;
  int flag = 0;

  posX[j] = sensorX.readRangeSingleMillimeters(); //read Sensor value X-axis
  posY[j] = sensorY.readRangeSingleMillimeters(); //read Sensor value Y-axis
  posZ[j] = sensorZ.readRangeSingleMillimeters(); //read Sensor value Z-axis

  // Serial.println(posX[i]);
  // Serial.println(posY[i]);
  // Serial.println(posZ[i]);
  // Serial.println(i);
  // Serial.println("       ");
  for (int i = 0; i < gemiddeldeAantal; i++) { //Put value into an array,  changing the array position everytime
    posXgem += posX[i];
    posYgem += posY[i];
    posZgem += posZ[i];
  }
  posXgem /= gemiddeldeAantal;
  posYgem /= gemiddeldeAantal;
  posZgem /= gemiddeldeAantal;

  if (posXgem < 0 || posYgem < 0 || posZgem < 0 && flag == 0) {
    sensorSet();
    flag = 1;
  } else {
    positie.x = posXgem; //make the position value the average value.
    positie.y = posYgem;
    positie.z = posZgem;
    flag = 0;
  }
  j++;
  if (j >= gemiddeldeAantal) {
    j = 0;
  }



}
void inputCode() {
  while (Serial2.available()) { //while nextion --> arduino communication
    static int cntFF;
    input = Serial2.read();
    Serial.print(input);
    switch (input) {
      case 0: //ignore error message
        break;
      case 26://ignore error message
        break;
      case 112:
        if (text.length() < 2) {//nextion Startcode
          text = "";
        } else {
          text += char(input); // normal letter 'p'
        }
        break;
      case 255: //nextion Stopcode
        cntFF++;
        break;
      default:
        text += char(input); // ASCII hex to char
        break;
    }
    if (cntFF == 3)  { //3 Stop codes = end of transmission
      Serial.println(text);
      switch (text[0]) {
        case 'M':
          magnet = !magnet;
          digitalWrite(33, magnet);
          digitalWrite(magneetactief, !magnet);
          break;
        case 'Z':
          if (text.length() > 1) {
            command = text;
            setPositionZ(); //processing nextion--> arduino to target coordinates.
          }
          break;
        default:
          if (text.length() > 2) {
            command = text;
            setPositionXY(); //processing nextion--> arduino to target coordinates.
          }
          break;
      }

      text = "";
      cntFF = 0;
    }
  }

}
void writeToNextion() {
  //write current sensor distance to nextion screen as TEXT
  nexWriteTxt("Aflees", "t8", String(positie.x));
  nexWriteTxt("Aflees", "t9", String(positie.y));
  nexWriteTxt("Aflees", "t10", String(positie.z));

  //write target distance to nextion screen as TEXT
  nexWriteTxt("Aflees", "t3", String(target.x));
  nexWriteTxt("Aflees", "t4", String(target.y));
  nexWriteTxt("Aflees", "t5", String(target.z));


  //write current sensor distance to nextion screen as SLIDER values
  //X-axis
  if (positie.x < target.x - 15) {
    nexWriteSlid("Aflees", "j0", positie.x, 0, target.x);
    nexWriteSlid("Aflees", "j1", 0, 0, 100);
  } else if (positie.x > target.x + 5) {
    nexWriteSlid("Aflees", "j1", positie.x, target.x, 1000);
    nexWriteSlid("Aflees", "j0", 100, 0, 100);
  }

  //Y-axis
  if (positie.y < target.y - 5) {
    nexWriteSlid("Aflees", "j2", positie.y, 0, target.y);
    nexWriteSlid("Aflees", "j3", 0, 0, 100);
  } else if (positie.y > target.y + 5) {
    nexWriteSlid("Aflees", "j3", positie.y, target.y, 500);
    nexWriteSlid("Aflees", "j2", 100, 0, 100);
  }
  //Z-axis
  if (positie.z < target.z - 5) {
    nexWriteSlid("Aflees", "j4", positie.z, 0, target.z);
    nexWriteSlid("Aflees", "j5", 0, 0, 100);
  } else if (positie.z > target.z + 5) {
    nexWriteSlid("Aflees", "j5", positie.z, target.z, 500);
    nexWriteSlid("Aflees", "j4", 100, 0, 100);
  }
  previousmillis = millisnow;
}

void nexWriteVal(String scherm, String element, int msg) {
  String cmd;
  cmd += scherm;
  cmd += ".";
  cmd += element;
  cmd += ".val=";
  cmd += msg;
  Serial2.print(cmd);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}

void nexWriteSlid(String screen, String element, int msg, int mini1, int maxi1) {
  int mapValue = map(msg, mini1, maxi1, 0, 100);
  String cmd;
  cmd += screen;
  cmd += ".";
  cmd += element;
  cmd += ".val=";
  cmd += mapValue;
  Serial2.print(cmd);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}

void nexWriteTxt(String screen, String element, String msg) {
  String cmd;
  cmd += screen;
  cmd += ".";
  cmd += element;
  cmd += ".txt=\"";
  cmd += msg;
  cmd += "\"";
  Serial2.print(cmd);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}
//int targetX[8] = {35, 85, 145, 205, 270, 340, 415, 505};
int targetX[8] = {35, 85, 145, 205, 270, 320, 375, 435};
void setPositionXY() {
  int offset;
  if (command.substring(2, command.length()) == "opslag") {
    //Serial.println("BEDOELT VOOR OPSLAG");
    offset = 5;
  } else if (command.substring(2, command.length()) == "laadruim") {
    //Serial.println("BEDOELT VOOR laadruimte");
    offset = 0;
  }
  //target.x = (((gridX.indexOf(command[0]) + offset) * interval ) - 30);
  target.y = ((gridY.indexOf(command[1])) *  interval) + 80;
  target.x = targetX[ (offset + gridX.indexOf(command[0]))];
  Serial.print("xTarget = ");
  Serial.print(target.x);
  Serial.print("  yTarget = ");
  Serial.print(target.y);
}

void setPositionZ() {
  String temp;
  int strength;
  temp = command.substring(1, command.length());
  strength = map(temp.toInt(), 0, 100, 156, 30);
  target.z = strength;
  Serial.print("  ZTarget = ");
  Serial.println(target.z);
}
