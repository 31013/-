/*这是我们做的防盗系统的硬件端模块的代码
这里有mcookie的官方教程的网站：https://wiki.microduino.cn/index.php?title=Microduino-Buzz%2Fzh 
（如果看我们程序的朋友对这个感兴趣又不知道怎么打代码，推荐哦哦哦（》—《））
这个程序是我们这组独创的。

*/
#include <Adafruit_NeoPixel.h>//引用头文件
#define humanHotSensor 4//PIR传感器D4
#define PIN 6//彩灯
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, PIN, NEO_GRB + NEO_KHZ800);
int humanHotState;
#define mic_pin A6
#define buzzer_pin 6
#define INTERVAL_SENSOR   17000             //定义传感器采样时间间隔  597000
#define INTERVAL_NET      17000             //定义发送时间
//传感器部分================================   
#include <Wire.h>                                  //调用库  
#include "./ESP8266.h"
#include "I2Cdev.h"                                //调用库  
//温湿度   
#include <SHT2x.h>
//光照
#define  sensorPin_1  A0

#define SSID           "HUAWEI nova 2"                   // cannot be longer than 32 characters!
#define PASSWORD       "11111111"

#define IDLE_TIMEOUT_MS  3000      // Amount of time to wait (in milliseconds) with no data 
                                   // received before closing the connection.  If you know the server
                                   // you're accessing is quick to respond, you can reduce this value.

//WEBSITE     
#define HOST_NAME   "jjfarfapi.heclouds.com"
#define DEVICEID   "20449602"
#define PROJECTID "106194"
#define HOST_PORT   (80)
String apiKey="RFeyfLJTSW6WcEeWSPIfmD5c8Xo=";
char buf[10];

#define INTERVAL_sensor 2000
unsigned long sensorlastTime = millis();

float tempOLED, humiOLED, lightnessOLED,micOLED;

#define INTERVAL_OLED 1000

String mCottenData;
String jsonToSend;

//3,传感器值的设置 
float sensor_tem, sensor_hum, sensor_lux,sensor_hot,sensor_sound;                    //传感器温度、湿度、光照   
char  sensor_tem_c[7], sensor_hum_c[7], sensor_lux_c[7],sensor_sound_c[7] ;    //换成char数组传输
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); /* RX:D3, TX:D2 */
ESP8266 wifi(mySerial);
//ESP8266 wifi(Serial1);                                      //定义一个ESP8266（wifi）的对象
unsigned long net_time1 = millis();                          //数据上传服务器时间
unsigned long sensor_time = millis();                        //传感器采样时间计时器

//int SensorData;                                   //用于存储传感器数据
String postString;                                //用于存储发送数据的字符串
//String jsonToSend;                                //用于存储发送的json格式参数

void setup(void)     //初始化函数  
{   
  strip.begin();
  
  //初始化串口波特率  
    Wire.begin();
    Serial.begin(115200);   
    while(!Serial);
    pinMode(sensorPin_1, INPUT);
    pinMode(humanHotSensor, INPUT);
    pinMode(buzzer_pin,OUTPUT);
    pinMode(mic_pin, INPUT);

   //ESP8266初始化
    Serial.print("setup begin\r\n");   

  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStationSoftAP()) {
    Serial.print("to station + softap ok\r\n");
  } else {
    Serial.print("to station + softap err\r\n");
  }

  if (wifi.joinAP(SSID, PASSWORD)) {      //加入无线网
    Serial.print("Join AP success\r\n");  
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
  }

  if (wifi.disableMUX()) {
    Serial.print("single ok\r\n");
  } else {
    Serial.print("single err\r\n");
  }

  Serial.print("setup end\r\n");
    
  
}
void loop(void)     //循环函数  
{   
  humanHotState = digitalRead(humanHotSensor);
  // print out the state of the button:
  //Serial.println(humanHotState);
  //delay(1);        // delay in between reads for stability

  if (humanHotState) {
    strip.setPixelColor(0, strip.Color(255, 0, 0));//红光
  strip.show();   //LED显示
  delay(1000);  //延迟1秒输出
   for(int i=200;i<=800;i++)  //用循环的方式将频率从200HZ 增加到800HZ
  {
    tone(buzzer_pin,i);    //在端口输出频率
    delay(5);      //该频率维持5毫秒   
  }
  delay(1000);     //最高频率下维持1秒钟
  for(int i=800;i>=200;i--)
  {
    tone(buzzer_pin,i);
    delay(10); //该频率维持10毫秒   
  }
  }
  else
  { strip.setPixelColor(0, strip.Color(0, 0, 0));//红光
  strip.show();   //LED显示
  delay(1000);  //延迟1秒输出
  }
  if (sensor_time > millis())  sensor_time = millis();  
    
  if(millis() - sensor_time > INTERVAL_SENSOR)              //传感器采样时间间隔  
  {  
    getSensorData();                                        //读串口中的传感器数据
    sensor_time = millis();
  }  

    
  if (net_time1 > millis())  net_time1 = millis();
  
  if (millis() - net_time1 > INTERVAL_NET)                  //发送数据时间间隔
  {                
    updateSensorData();                                     //将数据上传到服务器的函数
    net_time1 = millis();
  }
  
}

void getSensorData(){  
    sensor_tem = SHT2x.readT() ;   
    sensor_hum = SHT2x.readRH();   
    //获取光照
    sensor_lux = analogRead(A0);  
    sensor_sound=analogRead(A2);
    delay(1000);
    dtostrf(sensor_tem, 2, 1, sensor_tem_c);
    dtostrf(sensor_hum, 2, 1, sensor_hum_c);
    dtostrf(sensor_lux, 3, 1, sensor_lux_c);
}
void updateSensorData() {
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
    Serial.print("create tcp ok\r\n");

jsonToSend="{\"Temperature\":";
    dtostrf(sensor_tem,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+=",\"Humidity\":";
    dtostrf(sensor_hum,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+=",\"Light\":";
    dtostrf(sensor_lux,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+=",\"Sound\":";
    dtostrf(sensor_sound,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+="}";



    postString="POST /devices/";
    postString+=DEVICEID;
    postString+="/datapoints?type=3 HTTP/1.1";
    postString+="\r\n";
    postString+="api-key:";
    postString+=apiKey;
    postString+="\r\n";
    postString+="Host:api.heclouds.com\r\n";
    postString+="Connection:close\r\n";
    postString+="Content-Length:";
    postString+=jsonToSend.length();
    postString+="\r\n";
    postString+="\r\n";
    postString+=jsonToSend;
    postString+="\r\n";
    postString+="\r\n";
    postString+="\r\n";

  const char *postArray = postString.c_str();                 //将str转化为char数组
  Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
  Serial.println("send success");   
     if (wifi.releaseTCP()) {                                 //释放TCP连接
        Serial.print("release tcp ok\r\n");
        } 
     else {
        Serial.print("release tcp err\r\n");
        }
      postArray = NULL;                                       //清空数组，等待下次传输数据
  
  } else {
    Serial.print("create tcp err\r\n");
  }
  
}
