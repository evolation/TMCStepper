#include "TMCStepper.h"

#define SET_REG(SETTING) PWMCONF_register.SETTING = B; write(PWMCONF_register.address, PWMCONF_register.sr)
#define GET_REG(SETTING) return PWMCONF_register.SETTING

// PWMCONF
uint32_t TMC2130Stepper::PWMCONF() { return PWMCONF_register.sr; }
void TMC2130Stepper::PWMCONF(uint32_t input) {
	PWMCONF_register.sr = input;
	write(PWMCONF_register.address, PWMCONF_register.sr);
}

void TMC2130Stepper::pwm_ampl(		uint8_t B )	{ SET_REG(pwm_ampl);		}
void TMC2130Stepper::pwm_grad(		uint8_t B )	{ SET_REG(pwm_grad);		}
void TMC2130Stepper::pwm_freq(		uint8_t B )	{ SET_REG(pwm_freq);		}
void TMC2130Stepper::pwm_autoscale(	bool 	B )	{ SET_REG(pwm_autoscale);	}
void TMC2130Stepper::pwm_symmetric(	bool 	B )	{ SET_REG(pwm_symmetric);	}
void TMC2130Stepper::freewheel(		uint8_t B )	{ SET_REG(freewheel);		}

uint8_t TMC2130Stepper::pwm_ampl()		{ GET_REG(pwm_ampl);		}
uint8_t TMC2130Stepper::pwm_grad()		{ GET_REG(pwm_grad);		}
uint8_t TMC2130Stepper::pwm_freq()		{ GET_REG(pwm_freq);		}
bool 	TMC2130Stepper::pwm_autoscale()	{ GET_REG(pwm_autoscale);	}
bool 	TMC2130Stepper::pwm_symmetric()	{ GET_REG(pwm_symmetric);	}
uint8_t TMC2130Stepper::freewheel()		{ GET_REG(freewheel);		}