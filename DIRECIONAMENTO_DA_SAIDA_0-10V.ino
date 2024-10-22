#include "OptaBlue.h"

#define PERIODIC_UPDATE_TIME 5000
#define DELAY_AFTER_SETUP 200

/* -------------------------------------------------------------------------- */
void printExpansionType(ExpansionType_t t) {
  /* -------------------------------------------------------------------------- */
  if (t == EXPANSION_NOT_VALID) {
    Serial.print("Unknown!");
  } else if (t == EXPANSION_OPTA_DIGITAL_MEC) {
    Serial.print("Opta --- DIGITAL [Mechanical]  ---");
  } else if (t == EXPANSION_OPTA_DIGITAL_STS) {
    Serial.print("Opta --- DIGITAL [Solid State] ---");
  } else if (t == EXPANSION_DIGITAL_INVALID) {
    Serial.print("Opta --- DIGITAL [!!Invalid!!] ---");
  } else if (t == EXPANSION_OPTA_ANALOG) {
    Serial.print("~~~ Opta  ANALOG ~~~");
  } else {
    Serial.print("Unknown!");
  }
}

/* -------------------------------------------------------------------------- */
void printExpansionInfo() {
  /* -------------------------------------------------------------------------- */
  static long int start = millis();

  if (millis() - start > 5000) {
    start = millis();
    Serial.print("Number of expansions: ");
    Serial.println(OptaController.getExpansionNum());

    for (int i = 0; i < OptaController.getExpansionNum(); i++) {
      Serial.print("Expansion n. ");
      Serial.print(i);
      Serial.print(" type ");
      printExpansionType(OptaController.getExpansionType(i));
      Serial.print(" I2C address ");
      Serial.println(OptaController.getExpansionI2Caddress(i));
    }
  }
}

/* -------------------------------------------------------------------------- */
/*                                 SETUP                                      */
/* -------------------------------------------------------------------------- */
void setup() {
  /* -------------------------------------------------------------------------- */
  Serial.begin(115200);
  delay(2000);

  OptaController.begin();

  for (int device = 0; device < OptaController.getExpansionNum(); device++) {
        for (int ch = 0; ch < OA_AN_CHANNELS_NUM; ch++) {

      /* odd channel are configured as voltage DAC */
      AnalogExpansion::beginChannelAsDac(OptaController,
                                         device,
                                         ch,
                                         OA_VOLTAGE_DAC,
                                         true,
                                         false,
                                         OA_SLEW_RATE_0);
      }
  }
}



/* -------------------------------------------------------------------------- */
void optaAnalogTask() {
  /* -------------------------------------------------------------------------- */

  static long int start = millis();

  /* using this the code inside the if will run every PERIODIC_UPDATE_TIME ms
     assuming the function is called repeatedly in the loop() function */

  if (millis() - start > PERIODIC_UPDATE_TIME) {
    start = millis();

    static uint16_t dac_value = 0;

    dac_value += 1000;
    if (dac_value > 6000) {
      /* go in falling state */
      dac_value = 0;
    }
    
    for (int i = 0; i < OptaController.getExpansionNum(); i++) {
      AnalogExpansion exp = OptaController.getExpansion(i);
      if (exp) {
        Serial.println("Setting dac value " + String(dac_value) + " on expansion n. " + String(exp.getIndex()));
          exp.setDac(OA_CH_7, dac_value);
      
      }
    }
  }
}

/* -------------------------------------------------------------------------- */
/*                                  LOOP                                      */
/* -------------------------------------------------------------------------- */
void loop() {
  /* -------------------------------------------------------------------------- */
  OptaController.update();
  printExpansionInfo();
  optaAnalogTask();
}