/**
 *  Copyright (c) 2022 Milko van Valen
**/

#include <stdlib.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

#include "MakerPicoBuzzer.h"

void on_pwm_wrap() {
    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(PICO_MAKERPICOBUZZER_PIN));
    // Square the fade value to make the LED's brightness appear more linear
    // Note this range matches with the wrap value
    pwm_set_gpio_level(PICO_MAKERPICOBUZZER_PIN, 2260);
}


int main(){
    const uint ButtonPIN = 20;
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    uint debounce = 0;

    gpio_init(LED_PIN);
    gpio_init(ButtonPIN);

    gpio_set_function(PICO_MAKERPICOBUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PICO_MAKERPICOBUZZER_PIN);
    pwm_clear_irq(slice_num);
    //pwm_set_irq_enabled(slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1.f);    
    pwm_init(slice_num, &config, true);
    pwm_set_wrap(slice_num, 0x08d4u);
    pwm_set_clkdiv(slice_num, 125.f);

    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(ButtonPIN, GPIO_IN);
    gpio_pull_up(ButtonPIN);
    while (true) {
        if (!gpio_get(ButtonPIN)){
            debounce++;
        }
        if (debounce == 10){
            debounce = 0;
            pwm_set_irq_enabled(slice_num, true);
        }else{
            pwm_set_irq_enabled(slice_num, false);
        }

        /*gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);*/
    }
}