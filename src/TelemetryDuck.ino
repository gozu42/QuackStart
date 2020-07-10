/* 
   this is a mamaduck that sends some basic data quacks every now and then
   it is useful as a quickstart example since it doesnt need any configuration
   it reports some basic/crude stats like 
   - cpu situation (millis+loopcount)
   - memory situation (free and fragmentation)
   - temperature (from sx127x sensor)
   - battery level (from battery pin)

*/

// this is currently required to access the sx127x temperature sensor
#include <RadioLib.h>
extern SX1276 lora;

#include <ClusterDuck.h>

#include "driver/adc.h"
#include "esp_adc_cal.h"
#define DEFAULT_VREF    1100
esp_adc_cal_characteristics_t adc_chars;

#include "timer.h"
//auto timer = timer_create_default(); // create a timer with default settings
// use mamas tymer instead
extern Timer<> tymer;

// the ID will be autoreplaced by the last four digits of the MAC
String myid = "blah";

ClusterDuck duck;

void setup() {
  // use last 16 bit of MAC as duck-id
  uint64_t chipid = ESP.getEfuseMac();
  myid = String((uint16_t)(chipid>>32),HEX);
  while (myid.length() < 4) {
    myid = "0" + myid;
  }

  // switch on vext (so we can read the battery voltage)
#ifdef CDPCFG_PIN_VEXT
  pinMode(CDPCFG_PIN_VEXT,OUTPUT);
  digitalWrite(CDPCFG_PIN_VEXT,LOW);
#endif

#ifdef CDPCFG_PIN_HIGH
  pinMode(CDPCFG_PIN_HIGH,OUTPUT);
  digitalWrite(CDPCFG_PIN_HIGH,HIGH);
#endif

    //Characterize ADC at particular atten
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);

    //esp_adc_cal_characteristics_t *adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    //esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    //Check type of calibration value used to characterize ADC
//    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
//        printf("eFuse Vref");
//    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
//        printf("Two Point");
//    } else {
//        printf("Default");
//    }

  // standard mama setup
  duck.begin();
  duck.setDeviceId(myid);
  duck.setupMamaDuck();

  // dont send a quack right at startup 
  // this avoids spectrum spam in brownout rapid-reboot-loop situations
  schedSensor();
  Serial.println("SETUP DONE");
}

void schedSensor() {
  // send a sensor quack every 45-75 seconds
  tymer.in(45000+random(30000), runSensor);
}

uint32_t loopCount = 0;
void loop() {
  loopCount++;
  // let mama call the ticks for us
  //tymer.tick();
  duck.runMamaDuck();
}

bool runSensor(void *) {

#ifdef CDPCFG_PIN_BAT
    uint32_t reading = analogRead(CDPCFG_PIN_BAT);
    uint32_t bat = esp_adc_cal_raw_to_voltage(reading, &adc_chars);
#endif
#ifdef CDPCFG_BAT_MULDIV
  uint32_t raw = bat;
  bat = bat * CDPCFG_BAT_MULDIV;
  Serial.println("runSensor BAT read:"+String(reading)+" cal:"+String(raw)+" bat:"+String(bat));
#endif

#ifdef CDPCFG_PIN_PVV
    uint32_t pvvread = analogRead(CDPCFG_PIN_PVV);
    uint32_t pvv = esp_adc_cal_raw_to_voltage(pvvread, &adc_chars);
#endif
#ifdef CDPCFG_PVV_MULDIV
  uint32_t pvvraw = pvv;
  pvv = pvv * CDPCFG_PVV_MULDIV;
  Serial.println("runSensor PVV read:"+String(pvvread)+" cal:"+String(pvvraw)+" pvv:"+String(pvv));
#endif

  int8_t temp = lora.getTempRaw();

  String sensorVal = "myid:" + myid + " millis:" + String(millis(),DEC) + " lc:" + String(loopCount,DEC) +
#ifdef CDPCFG_PIN_BAT
	" bat:" + String(bat,DEC) + 
#endif
#ifdef CDPCFG_PIN_PVV
	" pvv:" + String(pvv,DEC) + 
#endif
	" gFH:" + String(ESP.getFreeHeap(),DEC) +
	" gMFH:" + String(ESP.getMinFreeHeap(),DEC) +
	" gMAH:" + String(ESP.getMaxAllocHeap(),DEC) +
	" temp:" + String(temp,DEC);

  schedSensor();

  Serial.println("SENSOR: " + sensorVal);
  duck.sendPayloadMessage(sensorVal);
  
  return true;
}
