#include <Wire.h>
#include <MPU6050_light.h>

// The number of MPU-6050 sensors in this example is 5.
#define NUM_MPUS 1

// Create objects. Each sensor is on the "Wire" I2C bus.
MPU6050 mpu[NUM_MPUS] = 
{
  MPU6050(Wire)
};

// For the AD0 pins of the MPU-6050.
// To select runtime the I2C address.
const int AD0pin[NUM_MPUS] = {46};

const int mpuAddr = 0x68;  // I2C address
const int mpuAddr_not_used = 0x69; // not even used in the code.

unsigned long previousMillis;
const unsigned long interval = 1000;

void setup() {
  Serial.begin(115200);
  Serial.println();

  Wire.begin();  // default pins: SDA=21, SCL=22.

  for(int i=0; i<NUM_MPUS; i++)
  {
    pinMode(AD0pin[i],OUTPUT);
    digitalWrite(AD0pin[i],HIGH); // default high for 0x69
  }

  // Initialize all the MPU-6050 sensors.
  Serial.println(F("Initializing the sensors and calculating offsets. Do not move MPU-6050 sensors."));
  Serial.println(F("Attention: Calculating the offset is good in real life, but it is better to skip this function in Wokwi."));
  delay(500);
  for(int i=0; i<NUM_MPUS; i++)
  {
    // Always select the proper sensor first.
    SelectMPU(i);

    // Run the library initialization function .begin
    byte status = mpu[i].begin();
    
    Serial.print(F("Initializing MPU-6050 "));
    Serial.print(i);
    Serial.print(F(", error = "));
    Serial.print(status);
    if(status == 0)
      Serial.print(F(" (no error)"));
    Serial.println();
  
    mpu[i].calcOffsets(true,true); // gyro and accelometer
  }
  Serial.println("Initialization done");

  // A message to click on a sensor to change
  // the settings when running in Wokwi.
  Serial.println("Click on a MPU-6050 module and change the acceleration and gyro.");

  // The setup() took so long,
  // set previousMillis to a fresh value.
  previousMillis = millis();
}

void loop() 
{
  unsigned long currentMillis = millis();

  // Update, this keeps the library going
  for(int i=0; i<NUM_MPUS; i++)
  {
    SelectMPU(i);
    mpu[i].update();
  }

  if(currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    for(int i=0; i<NUM_MPUS; i++)
    {
      SelectMPU(i);

      Serial.print(i);
      Serial.print(":");
      Serial.print(F("T="));
      Serial.print(mpu[i].getTemp(),0);
      Serial.print(F(",ACC="));
      Serial.print(mpu[i].getAccX());
      Serial.print(",");
      Serial.print(mpu[i].getAccY());
      Serial.print(",");
      Serial.print(mpu[i].getAccZ());
    
      Serial.print(F(",GYRO="));
      Serial.print(mpu[i].getGyroX(),0);
      Serial.print(",");
      Serial.print(mpu[i].getGyroY(),0);
      Serial.print(",");
      Serial.print(mpu[i].getGyroZ(),0);
    
      Serial.print(F(",ACC ANGLE="));
      Serial.print(mpu[i].getAccAngleX(),0);
      Serial.print(",");
      Serial.print(mpu[i].getAccAngleY(),0);
      
      Serial.print(F(",ANGLE="));
      Serial.print(mpu[i].getAngleX(),0);
      Serial.print(",");
      Serial.print(mpu[i].getAngleY(),0);
      Serial.print(",");
      Serial.print(mpu[i].getAngleZ(),0);
      Serial.println();
    }
    Serial.println(F("======================================================================================\n"));
  }
}

// Choose a MPU, parameter starts at zero for the first sensor.
// AD0 = high, I2C address = 0x69, not selected
// AD0 = low,  I2C address = 0x68, selected
void SelectMPU(int selection)
{
  for(int i=0; i<NUM_MPUS; i++)
  {
    if(i == selection)
      digitalWrite(AD0pin[i],LOW);  // selected, 0x68
    else
      digitalWrite(AD0pin[i],HIGH); // not selected, 0x69
  }
}
