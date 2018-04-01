#include "TMCStepper.h"
#include "SW_SPI.h"

TMC2660Stepper::TMC2660Stepper(uint8_t pinCS, float RS) {
  this->_pinCS = pinCS;
  Rsense = RS;
}

TMC2660Stepper::TMC2660Stepper(uint16_t pinCS, float RS, uint16_t pinMOSI, uint16_t pinMISO, uint16_t pinSCK) {
  this->_pinCS = pinCS;
  Rsense = RS;
  uses_sw_spi = true;
  TMC_SW_SPI.setPins(pinMOSI, pinMISO, pinSCK);
}

void TMC2660Stepper::switchCSpin(bool state) {
  // Allows for overriding in child class to make use of fast io
  digitalWrite(_pinCS, state);
}

uint32_t TMC2660Stepper::read() {
  uint32_t response = 0UL;
  switchCSpin(LOW);
  if (uses_sw_spi) {
    response |= TMC_SW_SPI.transfer((DRVCONF_register.cfg.sr >> 16) & 0xFF);
    response <<= 8;
    response |= TMC_SW_SPI.transfer((DRVCONF_register.cfg.sr >>  8) & 0xFF);
    response <<= 8;
    response |= TMC_SW_SPI.transfer(DRVCONF_register.cfg.sr & 0xFF);
  } else {
    SPI.begin();
    SPI.beginTransaction(SPISettings(spi_speed, MSBFIRST, SPI_MODE3));
    response |= SPI.transfer((DRVCONF_register.cfg.sr >> 16) & 0xFF);
    response <<= 8;
    response |= SPI.transfer((DRVCONF_register.cfg.sr >>  8) & 0xFF);
    response <<= 8;
    response |= SPI.transfer(DRVCONF_register.cfg.sr & 0xFF);
    SPI.endTransaction();
  }
  switchCSpin(HIGH);
  return response;
}

void TMC2660Stepper::write(uint8_t addressByte, uint32_t config) {
  uint32_t data = (uint32_t)addressByte<<17 | config;
  switchCSpin(LOW);
  if (uses_sw_spi) {
    TMC_SW_SPI.transfer((data >> 16) & 0xFF);
    TMC_SW_SPI.transfer((data >>  8) & 0xFF);
    TMC_SW_SPI.transfer(data & 0xFF);
  } else {
    SPI.begin();
    SPI.beginTransaction(SPISettings(spi_speed, MSBFIRST, SPI_MODE3));
    SPI.transfer((data >> 16) & 0xFF);
    SPI.transfer((data >>  8) & 0xFF);
    SPI.transfer(data & 0xFF);
    SPI.endTransaction();
  }
  switchCSpin(HIGH);
}

void TMC2660Stepper::begin() {
  //set pins
  pinMode(_pinCS, OUTPUT);
  switchCSpin(HIGH);

  //TODO: Push shadow registers

  toff(8); //off_time(8);
  tbl(1); //blank_time(24);

  _started = true;
}

bool TMC2660Stepper::isEnabled() { return toff() > 0; }

uint16_t TMC2660Stepper::rms_current() {
  return (float)(cs()+1)/32.0 * (vsense()?0.165:0.305)/(Rsense+0.02) / 1.41421 * 1000;
}
void TMC2660Stepper::rms_current(uint16_t mA) {
  uint8_t CS = 32.0*1.41421*mA/1000.0*(Rsense+0.02)/0.305 - 1;
  // If Current Scale is too low, turn on high sensitivity R_sense and calculate again
  if (CS < 16) {
    vsense(true);
    CS = 32.0*1.41421*mA/1000.0*(Rsense+0.02)/0.180 - 1;
  } else { // If CS >= 16, turn off high_sense_r
    vsense(false);
  }
  cs(CS);
  val_mA = mA;
}

void TMC2660Stepper::push() {
  DRVCTRL( sdoff() ? DRVCTRL_1_register.cfg.sr : DRVCTRL_0_register.cfg.sr);
  CHOPCONF(CHOPCONF_register.cfg.sr);
  SMARTEN(SMARTEN_register.cfg.sr);
  SGCSCONF(SGCSCONF_register.cfg.sr);
  DRVCONF(DRVCONF_register.cfg.sr);
}

void TMC2660Stepper::microsteps(uint16_t ms) {
  switch(ms) {
    case 256: mres(0); break;
    case 128: mres(1); break;
    case  64: mres(2); break;
    case  32: mres(3); break;
    case  16: mres(4); break;
    case   8: mres(5); break;
    case   4: mres(6); break;
    case   2: mres(7); break;
    case   0: mres(8); break;
    default: break;
  }
}

uint16_t TMC2660Stepper::microsteps() {
  switch(mres()) {
    case 0: return 256;
    case 1: return 128;
    case 2: return  64;
    case 3: return  32;
    case 4: return  16;
    case 5: return   8;
    case 6: return   4;
    case 7: return   2;
    case 8: return   0;
  }
  return 0;
}

void TMC2660Stepper::blank_time(uint8_t value) {
  switch (value) {
    case 16: tbl(0b00); break;
    case 24: tbl(0b01); break;
    case 36: tbl(0b10); break;
    case 54: tbl(0b11); break;
  }
}

uint8_t TMC2660Stepper::blank_time() {
  switch (tbl()) {
    case 0b00: return 16;
    case 0b01: return 24;
    case 0b10: return 36;
    case 0b11: return 54;
  }
  return 0;
}