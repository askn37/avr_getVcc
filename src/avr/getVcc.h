/***************
 *
 * getVcc.h - AVR core utilities
 *
 * target architectures: Atmel AVR
 *
 * release site: https://github.com/askn37/avr_getVcc
 * maintainer: askn https://twitter.com/askn37
 *
 */

#ifndef __AVR_GETVCC_H
#define __AVR_GETVCC_H
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

extern float getVcc (void);
extern float getCoreTemperature (void);

#if defined(ADC0) && defined(VREF)
// Target of megaAVR
//  __AVR_ATmega4809__
//    ARDUINO_ARCH_MEGAAVR
//	  ARDUINO_AVR_UNO_WIFI_REV2
//    ARDUINO_AVR_NANO_EVERY
uint16_t analogReadRaw (void) {
    ADC0.COMMAND |= ADC_STCONV_bm;
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
    return ADC0.RES;
}
float getVcc (void) {
    noInterrupts();
    uint8_t vref_a = VREF.CTRLA;
    uint8_t adc0_c = ADC0.CTRLC;
    analogReference(VDD);
    ADC0.MUXPOS = (0x1c << ADC_MUXPOS_gp);	// MUXPOS4,,0
    VREF.CTRLA = (INTERNAL1V1 << VREF_ADC0REFSEL0_bp)
               | (INTERNAL1V1 << VREF_AC0REFSEL0_bp);
    ADC0.CTRLC |= ADC_SAMPCAP_bm;
    interrupts();
    analogReadRaw();
    delay(10);
    float vcc = (1024.0 * 1.1) / analogReadRaw();
    noInterrupts();
    VREF.CTRLA = vref_a;
    ADC0.CTRLC = adc0_c;
    interrupts();
    return vcc;
}
float getCoreTemperature (void) {
    noInterrupts();
    uint8_t vref_a = VREF.CTRLA;
    uint8_t adc0_c = ADC0.CTRLC;
    analogReference(INTERNAL1V1);
    ADC0.CTRLC |= ADC_SAMPCAP_bm;
    ADC0.MUXPOS = (0x1e << ADC_MUXPOS_gp);	// MUXPOS4,,0
    interrupts();
    analogReadRaw();
    delay(10);
    float tmp = (float)(
        (uint32_t)(analogReadRaw()
        - ((int8_t)SIGROW.TEMPSENSE1))
        * ((uint8_t)SIGROW.TEMPSENSE0)) / 256.0;
    noInterrupts();
    VREF.CTRLA = vref_a;
    ADC0.CTRLC = adc0_c;
    interrupts();
    return tmp;
}

#elif defined(ARDUINO_ARCH_AVR)
// Target of AVR
//  ARDUINO_ARCH_AVR
//  __AVR_ATmega328P__
//    ARDUINO_AVR_UNO
//  __AVR_ATmega32U4__
//    ARDUINO_AVR_LEONARDO
uint16_t analogReadRaw (void) {
    ADCSRA |= _BV(ADSC);
    while (ADCSRA & _BV(ADSC));
    // Serial.println(ADCW);
    return ADCW;
}

#if defined(__AVR_ATmega32U4__)
// ARDUINO_AVR_LEONARDO etc...
float getVcc (void) {
    noInterrupts();
    uint8_t adc_mux = ADMUX;
    uint8_t adc_srb = ADCSRB;
    ADMUX = (1 << REFS0) | 0x1E;    // REFS=AVCC, MUX=VBG:1V1
    ADCSRB = (ADCSRB & ~_BV(MUX5));
    interrupts();
    analogReadRaw();
    delay(10);
    float vcc = (1024.0 * 1.1) / analogReadRaw();
    noInterrupts();
    ADMUX = adc_mux;
    ADCSRB = adc_srb;
    interrupts();
    return vcc;
}
float getCoreTemperature (void) {
    noInterrupts();
    uint8_t adc_mux = ADMUX;
    uint8_t adc_srb = ADCSRB;
    ADMUX = (3 << REFS0) | 0x07;    // REFS=INTERNAL2V56, MUX=TMP
    ADCSRB |= _BV(MUX5);
    int16_t TmpOfst = -26;
    uint8_t TmpGain = 230;
    interrupts();
    analogReadRaw();
    delay(10);
    float tmp = (float)(
        (uint32_t)(analogReadRaw() - TmpOfst)
        * TmpGain) / 256.0;
    noInterrupts();
    ADMUX = adc_mux;
    ADCSRB = adc_srb;
    interrupts();
    return tmp;
}

#else
// __AVR_ATmega328P__ etc...
float getVcc (void) {
    noInterrupts();
    uint8_t adc_mux = ADMUX;
    ADMUX = (1 << REFS0) | 0x1E;    // REFS=AVCC, MUX=VBG:ADC14(30):1V1
    interrupts();
    analogReadRaw();
    delay(10);
    float vcc = (1024.0 * 1.1) / analogReadRaw();
    noInterrupts();
    ADMUX = adc_mux;
    interrupts();
    return vcc;
}
float getCoreTemperature (void) {
    noInterrupts();
    uint8_t adc_mux = ADMUX;
    ADMUX = (3 << REFS0) | 0x08;    // REFS=INTERNAL, MUX=TMP
    int16_t TmpOfst = 14;
    uint8_t TmpGain = 230;
    interrupts();
    analogReadRaw();
    delay(10);
    float tmp = (float)(
        (uint32_t)(analogReadRaw() - TmpOfst)
        * TmpGain) / 256.0;
    noInterrupts();
    ADMUX = adc_mux;
    interrupts();
    return tmp;
}

#endif  /* __AVR_ATmega32U4__ */
#endif  /* ARDUINO_ARCH_AVR */

#ifdef __cplusplus
}
#endif

#endif

// end of header