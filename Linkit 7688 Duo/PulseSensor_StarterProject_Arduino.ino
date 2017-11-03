/*
   Start Bee Hyperparameter
*/
int speakerPin = 4;
// 依照簡譜的順序，填入代表的音符，空白代表休止符
char notes[] = "fCfC";
// 決定每個音階的拍子，注意這邊用 unsigned long 所以拍子只能是正整數
unsigned long beats[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
// 利用 sizeof()，算出總共要多少音符
int length = sizeof(notes);
// 決定一拍多長，這邊一拍 300 ms
int tempo = 500;
/*
   End Bee Hyperparameter
*/

/*
   Start ADXL345 Hyperparameter and Declaraction
*/
#include <Wire.h>
#include <ADXL345.h>

ADXL345 adxl;
int falling = 0;
/*
   End ADXL345 Hyperparameter and Declaraction
*/


/*
   Start Heart Beat Rate Hyperparameter and Declaraction
*/
int PulseSensorPurplePin = A0;        // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0

const int BUFF_SIZE = 100;
const int SAMPLE_PERIOD = 10;
int readData, preReadData;
int timeCount = 0;
int firstTimeCount = 0;
int secondTimeCount = 0;


int IBI, BPM, SIG;
int data[BUFF_SIZE] = {0};
int index = 0;
int max_value, min_value, mid_value;
int filter ;

boolean PULSE = false;
boolean PRE_PULSE = false;

int pulseCount = 0;
int Threshold = 550;            // Determine which Signal to "count as a beat", and which to ingore.
/*
  End Heart Beat Rate Hyperparameter and Declaraction
*/

// The SetUp Function:
void setup() {
  Serial1.begin(57600);
  Serial.begin(9600);         // Set's up Serial Communication at certain speed.

  pinMode(speakerPin, OUTPUT); // Speaker
  /*
      ADXL345 Initialization
  */
  adxl.powerOn();
  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?

  //look of activity movement on this axes - 1 == on; 0 == off
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);

  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);

  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);

  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment

  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment

  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );

  //register interrupt actions - 1 == on; 0 == off
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);
}


// The Main Loop Function
void loop() {

  double xyz[3];
  double ax, ay, az;
  adxl.getAcceleration(xyz);
  ax = xyz[0];
  ay = xyz[1];
  az = xyz[2];
  Serial.print("X=");
    Serial.print(ax);
    Serial.println(" g");
    Serial.print("Y=");
    Serial.print(ay);
    Serial.println(" g");
    Serial.print("Z=");
    Serial.print(az);
    Serial.println(" g");
    Serial.println("**********************");
  falling = 0;

  if (ax >= 0.5 || ax <= -0.5)      // x-axis falling
    falling = 1;
  else if (ay >= 0.5 || ay <= -0.5)  // y-axis falling
    falling = 1;

  preReadData = readData;
  readData = analogRead(PulseSensorPurplePin);  // Read the PulseSensor's value.

  //Serial.println(readData);

  if ((readData - preReadData) < filter)
  {
    data[index++] = readData;
  }

  if (index >= BUFF_SIZE)
  {
    index = 0;

    max_value = Get_Array_Max(data, BUFF_SIZE);
    min_value = Get_Array_Min(data, BUFF_SIZE);
    mid_value = (max_value - min_value) * 3 / 4 + min_value;
    filter = (max_value - min_value) / 2;
  }

  PRE_PULSE = PULSE;
  PULSE = (readData > mid_value) ? true : false;

  if (PRE_PULSE == false && PULSE == true)
  {
    //Serial.println("hi you are here!");
    pulseCount++;
    pulseCount %= 2;

    if (pulseCount == 1)
    {
      firstTimeCount = timeCount;
    }
    if (pulseCount == 0)
    {
      secondTimeCount = timeCount;
      timeCount = 0;

      if (secondTimeCount > firstTimeCount)
      {
        IBI = (secondTimeCount - firstTimeCount) * SAMPLE_PERIOD;
        BPM = 60000 / IBI;

        if (BPM > 150)
          BPM = 130;
        if (BPM < 30)
          BPM = 40;
      }
    }
  }

  int count_error = 0;
  for (int i = 0 ; i < 50 ; ++i) {
    if (data[i] == 1023)
      count_error++;
  }


  if (timeCount % 100 == 0) {
    char buffer_transmit[7];
    itoa(BPM, buffer_transmit, 10);
    Serial1.write(buffer_transmit);
    Serial1.write("\n");
    Serial.println(buffer_transmit);

    if (count_error > 25) {
      Serial1.write("1");
      Serial.println(1);
    }

    else {
      Serial1.write("0");
      Serial.println(0);
    }
    Serial1.write("\n");

    if (falling) {
      Serial1.write("1");
      Serial.println(falling);
    }

    else {
      Serial1.write("0");
      Serial.println(falling);
    }
    Serial1.write("\n");

    //Serial.print("BPM:\t");
    //Serial.println(BPM);
    if (count_error > 25 || falling) {
      // 利用 for 來播放我們設定的歌曲，一個音一個音撥放
      for (int i = 0; i < length; i++) {
        // 如果是空白的話，不撥放音樂
        if (notes[i] == ' ') {
          delay(beats[i] * tempo); // rest
        } else {
          // 呼叫 palyNote() 這個 function，將音符轉換成訊號讓蜂鳴器發聲
          playNote(speakerPin, notes[i], beats[i] * tempo);
        }
        // 每個音符之間的間隔，這邊設定的長短會有連音 or 段音的效果
        delay(tempo / 10);
      }
    }
  }

  timeCount++;
  delay(9);


}


int Get_Array_Max(int array[], int size)
{
  int max = array[0];
  int i;

  for (i = 1; i < size; i++)
  {
    if (array[i] > max)
      max = data[i];
  }

  return max;
}

int Get_Array_Min(int array[], int size)
{
  int min = array[0];
  int i;

  for (i = 1; i < size; i++)
  {
    if (array[i] < min)
      min = data[i];
  }

  return min;
}

void playNote(int OutputPin_1, char note, unsigned long duration) {
  // 音符字元與對應的頻率由兩個矩陣表示
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C', 'F' , 'Z'};
  int tones[] = { 261, 294, 330, 349, 392, 440, 494, 523, 698, 1046 };
  // 播放音符對應的頻率
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      tone(OutputPin_1, tones[i], duration);
      //tone(OutputPin_2, tones[i], duration);
      //下方的 delay() 及 noTone ()，測試過後一定要有這兩行，整體的撥放出來的東西才不會亂掉，可能是因為 Arduino 送出tone () 頻率後會馬上接著執行下個指令，不會等聲音播完，導致撥出的聲音混合而亂掉
      delay(duration);
      noTone(OutputPin_1);
      //noTone(OutputPin_2);
    }
  }
}
