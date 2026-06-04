/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Damjan Marion <damjan.marion@gmail.com>
 * Copyright (C) 2011 Mark Panajotovic <marko@electrontube.org>
 * Copyright (C) 2015 Piotr Esden-Tempski <piotr@esden.net>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#define LGREENF GPIO13
#define LGREENF_PORT GPIOG
#define LREDF GPIO14
#define LREDF_PORT GPIOG

#define LGREENB GPIO13
#define LGREENB_PORT GPIOB
#define LREDB GPIO5
#define LREDB_PORT GPIOC

#define LCC LREDF_PORT, LREDF
#define LUP LREDB_PORT, LREDB

/*
  Timer 1 clk frequency:
  If TIMPRE == 0: (default)
    if PPRE = DIV1:
      TIM1CLK = PCLK
    else:
      TIM1CLK = 2*PCLK
  else:
    if PPRE = DIV1|DIV2|DIV4:
      TIM1CLK = HCLK
    else:
      TIM1CLK = 4*PCLK
 */


/* Set STM32 to 168 MHz. */
static void clock_setup(void)
{
	rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
    // AHB Prescaler = 1 (NODIV) libopencm3/lib/stm32/f4/rcc.c
    // APB1: DIV4
    // APB2: DIV2
    // SYSCLK: 168MHz
    // AHBCLK (HCLK or FCLK): SYSCLK/AHBPre = 168MHz/1
    // APB1: 168/4 = 42MHz
    // APB2: 168/2 = 84MHz
    // rcc_ahb_frequency, rcc_apb1_frequency, rcc_apb2_frequency
    // are set here (libopencm3/lib/stm32/f4/rcc.c)
    // TIM1 is on APB2
    // TIMPRE: arriba. Default 0
    // TIM1CLK = 2*PCLK = 2*84MHz
    // TIM1CNT = 168MHz/2^16   (TIM1CLK/(PRESCALER+1))
    // TIM1CNT = 2563.48Hz
    // If TIMPRE == 0 and PCLK > DIV1:
    // FCNT = 2*PCLK/(TIMPRESC+1) (PCLK = APB2 for TIM1)
    // If TIMPRE == 0 and PCLK == DIV1:
    // FCNT = PCLK/(TIMPRESC+1) (PCLK = APB2 for TIM1)
    // If TIMPRE == 1 and PCLK > DIV4:
    // FCNT = 4*PCLK/(TIMPRESC+1) (PCLK = APB2 for TIM1)
    // If TIMPRE == 1 and PCLK <= DIV4:
    // FCNT = HCLK/(TIMPRESC+1) (PCLK = APB2 for TIM1)

	/* Enable GPIOG clock. */
	rcc_periph_clock_enable(RCC_GPIOG);

	/* Enable GPIOB clock. */
	rcc_periph_clock_enable(RCC_GPIOB);

	/* Enable GPIOB clock. */
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Enable GPIOA clock. */
        rcc_periph_clock_enable(RCC_GPIOA);

	/* Enable TIM1 clock. */
	rcc_periph_clock_enable(RCC_TIM1);
	rcc_periph_clock_enable(RCC_TIM2);
	rcc_periph_clock_enable(RCC_TIM3);
	rcc_periph_clock_enable(RCC_TIM4);
}

static void gpio_setup(void)
{

	/* Set GPIO13-14 (in GPIO port G) to 'output push-pull'. */
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT,
			GPIO_PUPD_NONE, GPIO13 | GPIO14);
	/* Set GPIO5 (in GPIO port C) to 'output push-pull'. */
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT,
                    GPIO_PUPD_NONE, GPIO5);
        /* Set GPIO5 (in GPIO port C) to 'output push-pull'. */
        gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT,
                    GPIO_PUPD_NONE, GPIO11);
        /* Set GPIO5 (in GPIO port C) to 'output push-pull'. */
        gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT,
                    GPIO_PUPD_NONE, GPIO12);



// TIMER1 CHANEL 1 PIN PB13 EN AF1
    gpio_set_af(LGREENB_PORT, GPIO_AF1, LGREENB);
	/* Set GPIO13 (in GPIO port B) to 'alternate function push-pull'. */
	gpio_mode_setup(LGREENB_PORT, GPIO_MODE_AF,
                    GPIO_PUPD_NONE, LGREENB);


//TIMER 2 CHANEL 1 PIN PA5 EN AF1
    gpio_set_af(GPIOA, GPIO_AF1, GPIO5);
        gpio_mode_setup(GPIOA, GPIO_MODE_AF,
                    GPIO_PUPD_NONE, GPIO5);



//TIMER 3 CHANEL 1 PIN PB4 EN AF2
    gpio_set_af(GPIOB, GPIO_AF2, GPIO4);
        /* Set GPIO13 (in GPIO port B) to 'alternate function push-pull'. */
        gpio_mode_setup(GPIOB, GPIO_MODE_AF,
                    GPIO_PUPD_NONE, GPIO4);

//TIMER 4 CHANEL 2 PIN PB7 EN AF2
    gpio_set_af(GPIOB, GPIO_AF2, GPIO7);
        /* Set GPIO13 (in GPIO port B) to 'alternate function push-pull'. */
        gpio_mode_setup(GPIOB, GPIO_MODE_AF,
                    GPIO_PUPD_NONE, GPIO7);

//TIMER 3 CHANEL 3 PIN PC8 EN AF2
    gpio_set_af(GPIOC, GPIO_AF2, GPIO8);
        /* Set GPIO13 (in GPIO port B) to 'alternate function push-pull'. */
        gpio_mode_setup(GPIOC, GPIO_MODE_AF,
                    GPIO_PUPD_NONE, GPIO8);


}

static void tim_setup(void)
{
  /* Enable TIM1 clock. */
	rcc_periph_clock_enable(RCC_TIM1);
	rcc_periph_clock_enable(RCC_TIM2);
	rcc_periph_clock_enable(RCC_TIM3);
	rcc_periph_clock_enable(RCC_TIM4);

	/* Enable TIM1 interrupt. */
	nvic_enable_irq(NVIC_TIM1_CC_IRQ);
	nvic_enable_irq(NVIC_TIM1_UP_TIM10_IRQ);

	/* TIM2 */
        nvic_enable_irq(NVIC_TIM2_IRQ);

	/* TIM3 */
	nvic_enable_irq(NVIC_TIM3_IRQ);

	/* TIM4 */
	nvic_enable_irq(NVIC_TIM4_IRQ);

	/* Reset TIM1 peripheral to defaults. */
	rcc_periph_reset_pulse(RST_TIM1);
	rcc_periph_reset_pulse(RST_TIM2);
	rcc_periph_reset_pulse(RST_TIM3);
	rcc_periph_reset_pulse(RST_TIM4);

	/* Timer global mode:
        nvic_enable_irq(NVIC_TIM3_IRQ);
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 * (These are actually default values after reset above, so this call
	 * is strictly unnecessary, but demos the api for alternative settings)
	 */

	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT,
                   TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
        
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT,
                   TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	timer_set_mode(TIM4, TIM_CR1_CKD_CK_INT,
                   TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/*
	 * Please take note that the clock source for STM32 timers
	 * might not be the raw APB1/APB2 clocks.  In various conditions they
	 * are doubled.  See the Reference Manual for full details!
	 */
	timer_set_prescaler(TIM1, 0x00FF); // 2563Hz clk
	timer_set_prescaler(TIM2, 0x00FF);
        timer_set_prescaler(TIM3, 0x00FF);
        timer_set_prescaler(TIM4, 0x00FF);

    //timer_set_repetition_counter(TIM1, 15);
    timer_disable_preload(TIM1);
    timer_continuous_mode(TIM1);

    timer_disable_preload(TIM2);
    timer_continuous_mode(TIM2);

    timer_disable_preload(TIM3);
    timer_continuous_mode(TIM3);

    timer_disable_preload(TIM4);
    timer_continuous_mode(TIM4);

    /* Count period */
	timer_set_period(TIM1, 13124);
	timer_set_period(TIM2, 6562);
	timer_set_period(TIM3, 6562);
        timer_set_period(TIM4, 6562);

	/* Set the initual output compare value for OC1. */
	timer_set_oc_value(TIM1, TIM_OC1, 656); // no usar los negativos
	timer_set_oc_value(TIM2, TIM_OC1, 328);
        timer_set_oc_value(TIM3, TIM_OC1, 328); // no usar los negativos
        timer_set_oc_value(TIM4, TIM_OC2, 328);
        timer_set_oc_value(TIM3, TIM_OC3, 328); // no usar los negativos

    /* Disable outputs. */
    //timer_enable_oc_output(TIM1, TIM_OC1);
    timer_enable_oc_output(TIM1, TIM_OC1N);
    timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1); // no usar los negativos

    timer_enable_oc_output(TIM2, TIM_OC1);
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1);

    timer_enable_oc_output(TIM3, TIM_OC1);
    timer_set_oc_mode(TIM3, TIM_OC1, TIM_OCM_PWM1);

    timer_enable_oc_output(TIM4, TIM_OC2);
    timer_set_oc_mode(TIM4, TIM_OC2, TIM_OCM_PWM1);

    timer_enable_oc_output(TIM3, TIM_OC3);
    timer_set_oc_mode(TIM3, TIM_OC3, TIM_OCM_PWM1);
    //timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_FORCE_HIGH);
    //timer_set_oc_polarity_high(TIM1, TIM_OC1N);
    //timer_set_oc_idle_state_unset(TIM1, TIM_OC1N);
    //time_reset_output_idle(TIM1, TIM_CR2_OIS1N);
    //timer_disable_oc_output(TIM1, TIM_OC2);
    //timer_disable_oc_output(TIM1, TIM_OC2N);
    //timer_disable_oc_output(TIM1, TIM_OC3);
    //timer_disable_oc_output(TIM1, TIM_OC3N);

    /* Generate update event to reload all registers before starting*/
    timer_enable_break_main_output(TIM1);
    timer_enable_break_main_output(TIM2);
    timer_enable_break_main_output(TIM3);
    timer_enable_break_main_output(TIM4);

    //timer_set_disabled_off_state_in_idle_mode(TIM1);
    //timer_set_disabled_off_state_in_run_mode(TIM1);
    timer_disable_break(TIM1);
    timer_disable_break(TIM2);
    timer_disable_break(TIM3);
    timer_disable_break(TIM4);

    /* Counter enable. */
	timer_enable_counter(TIM1);
	timer_enable_counter(TIM2);
        timer_enable_counter(TIM3);
        timer_enable_counter(TIM4);

	/* Enable Channel 1 compare interrupt to recalculate compare values */
	timer_enable_irq(TIM1, TIM_DIER_CC1IE);
	timer_enable_irq(TIM1, TIM_DIER_UIE);

	timer_enable_irq(TIM2, TIM_DIER_CC1IE);
        timer_enable_irq(TIM2, TIM_DIER_UIE);

        timer_enable_irq(TIM3, TIM_DIER_CC1IE);
        timer_enable_irq(TIM3, TIM_DIER_UIE);

        timer_enable_irq(TIM4, TIM_DIER_CC1IE);
        timer_enable_irq(TIM4, TIM_DIER_UIE);

        timer_enable_irq(TIM3, TIM_DIER_CC1IE);

    //timer_generate_event(TIM1, TIM_EGR_UG);

}

void tim1_cc_isr(void)
{
  timer_clear_flag(TIM1, TIM_SR_CC1IF);
  gpio_toggle(LCC);
}
void tim1_up_tim10_isr(void)
{
  timer_clear_flag(TIM1, TIM_SR_UIF);
  gpio_toggle(LUP);
}

void tim2_isr(void)
{
    if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {
        timer_clear_flag(TIM2, TIM_SR_CC1IF);
        gpio_toggle(LCC);   // lo mismo que tim1_cc_isr
    }

    if (timer_get_flag(TIM2, TIM_SR_UIF)) {
        timer_clear_flag(TIM2, TIM_SR_UIF);
        gpio_toggle(LUP);   // lo mismo que tim1_up_tim10_isr
    }
}

void tim3_isr(void)
{
    if (timer_get_flag(TIM3, TIM_SR_CC1IF)) {
        timer_clear_flag(TIM3, TIM_SR_CC1IF);
        gpio_toggle(LCC);   // lo mismo que tim1_cc_isr
    }

    if (timer_get_flag(TIM3, TIM_SR_UIF)) {
        timer_clear_flag(TIM3, TIM_SR_UIF);
        gpio_toggle(LUP);   // lo mismo que tim1_up_tim10_isr
    }
}

void tim4_isr(void)
{
    if (timer_get_flag(TIM4, TIM_SR_CC1IF)) {
        timer_clear_flag(TIM4, TIM_SR_CC1IF);
        gpio_toggle(LCC);   // lo mismo que tim1_cc_isr
    }

    if (timer_get_flag(TIM4, TIM_SR_UIF)) {
        timer_clear_flag(TIM4, TIM_SR_UIF);
        gpio_toggle(LUP);   // lo mismo que tim1_up_tim10_isr
    }
}


int main(void)
{
int i;

clock_setup();
gpio_setup();
tim_setup();

// WHILE GENERAL
while(1){

	// SETEAR SALIDAS PARA LOS LEDS RUTINA #1
	gpio_clear(GPIOC, GPIO11); // 0
	gpio_clear(GPIOC, GPIO12); // 0

	int cnt = 0;

	// RUTINA #1
        while (cnt != 5) {

        timer_set_oc_value(TIM1, TIM_OC1, 656);
	timer_set_oc_value(TIM2, TIM_OC1, 656);
        timer_set_oc_value(TIM3, TIM_OC1, 656);
        timer_set_oc_value(TIM4, TIM_OC2, 360);
        timer_set_oc_value(TIM3, TIM_OC3, 460);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 721);
        timer_set_oc_value(TIM2, TIM_OC1, 641);
        timer_set_oc_value(TIM3, TIM_OC1, 624);
        timer_set_oc_value(TIM4, TIM_OC2, 374);
        timer_set_oc_value(TIM3, TIM_OC3, 466);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 786);
        timer_set_oc_value(TIM2, TIM_OC1, 626);
        timer_set_oc_value(TIM3, TIM_OC1, 592);
        timer_set_oc_value(TIM4, TIM_OC2, 388);
        timer_set_oc_value(TIM3, TIM_OC3, 472);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 851);
        timer_set_oc_value(TIM2, TIM_OC1, 611);
        timer_set_oc_value(TIM3, TIM_OC1, 560);
        timer_set_oc_value(TIM4, TIM_OC2, 402);
        timer_set_oc_value(TIM3, TIM_OC3, 478);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 916);
        timer_set_oc_value(TIM2, TIM_OC1, 596);
        timer_set_oc_value(TIM3, TIM_OC1, 528);
        timer_set_oc_value(TIM4, TIM_OC2, 416);
        timer_set_oc_value(TIM3, TIM_OC3, 484);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 981);
        timer_set_oc_value(TIM2, TIM_OC1, 581);
        timer_set_oc_value(TIM3, TIM_OC1, 496);
        timer_set_oc_value(TIM4, TIM_OC2, 430);
        timer_set_oc_value(TIM3, TIM_OC3, 490);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1046);
        timer_set_oc_value(TIM2, TIM_OC1, 566);
        timer_set_oc_value(TIM3, TIM_OC1, 464);
        timer_set_oc_value(TIM4, TIM_OC2, 444);
        timer_set_oc_value(TIM3, TIM_OC3, 496);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1111);
        timer_set_oc_value(TIM2, TIM_OC1, 551);
        timer_set_oc_value(TIM3, TIM_OC1, 432);
        timer_set_oc_value(TIM4, TIM_OC2, 458);
        timer_set_oc_value(TIM3, TIM_OC3, 502);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1176);
        timer_set_oc_value(TIM2, TIM_OC1, 536);
        timer_set_oc_value(TIM3, TIM_OC1, 400);
        timer_set_oc_value(TIM4, TIM_OC2, 472);
        timer_set_oc_value(TIM3, TIM_OC3, 508);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1241);
        timer_set_oc_value(TIM2, TIM_OC1, 521);
        timer_set_oc_value(TIM3, TIM_OC1, 368);
        timer_set_oc_value(TIM4, TIM_OC2, 486);
        timer_set_oc_value(TIM3, TIM_OC3, 514);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1312);
	timer_set_oc_value(TIM2, TIM_OC1, 500);
        timer_set_oc_value(TIM3, TIM_OC1, 328);
        timer_set_oc_value(TIM4, TIM_OC2, 500);
        timer_set_oc_value(TIM3, TIM_OC3, 524);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1241);
        timer_set_oc_value(TIM2, TIM_OC1, 521);
        timer_set_oc_value(TIM3, TIM_OC1, 368);
        timer_set_oc_value(TIM4, TIM_OC2, 486);
        timer_set_oc_value(TIM3, TIM_OC3, 514);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1176);
        timer_set_oc_value(TIM2, TIM_OC1, 536);
        timer_set_oc_value(TIM3, TIM_OC1, 400);
        timer_set_oc_value(TIM4, TIM_OC2, 472);
        timer_set_oc_value(TIM3, TIM_OC3, 508);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1111);
        timer_set_oc_value(TIM2, TIM_OC1, 551);
        timer_set_oc_value(TIM3, TIM_OC1, 432);
        timer_set_oc_value(TIM4, TIM_OC2, 458);
        timer_set_oc_value(TIM3, TIM_OC3, 502);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1046);
        timer_set_oc_value(TIM2, TIM_OC1, 566);
        timer_set_oc_value(TIM3, TIM_OC1, 464);
        timer_set_oc_value(TIM4, TIM_OC2, 444);
        timer_set_oc_value(TIM3, TIM_OC3, 496);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 981);
        timer_set_oc_value(TIM2, TIM_OC1, 581);
        timer_set_oc_value(TIM3, TIM_OC1, 496);
        timer_set_oc_value(TIM4, TIM_OC2, 430);
        timer_set_oc_value(TIM3, TIM_OC3, 490);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 916);
        timer_set_oc_value(TIM2, TIM_OC1, 596);
        timer_set_oc_value(TIM3, TIM_OC1, 528);
        timer_set_oc_value(TIM4, TIM_OC2, 416);
        timer_set_oc_value(TIM3, TIM_OC3, 484);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 851);
        timer_set_oc_value(TIM2, TIM_OC1, 611);
        timer_set_oc_value(TIM3, TIM_OC1, 560);
        timer_set_oc_value(TIM4, TIM_OC2, 402);
        timer_set_oc_value(TIM3, TIM_OC3, 478);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 786);
        timer_set_oc_value(TIM2, TIM_OC1, 626);
        timer_set_oc_value(TIM3, TIM_OC1, 592);
        timer_set_oc_value(TIM4, TIM_OC2, 388);
        timer_set_oc_value(TIM3, TIM_OC3, 472);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 721);
        timer_set_oc_value(TIM2, TIM_OC1, 641);
        timer_set_oc_value(TIM3, TIM_OC1, 624);
        timer_set_oc_value(TIM4, TIM_OC2, 374);
        timer_set_oc_value(TIM3, TIM_OC3, 466);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 656);
        timer_set_oc_value(TIM2, TIM_OC1, 656);
        timer_set_oc_value(TIM3, TIM_OC1, 656);
        timer_set_oc_value(TIM4, TIM_OC2, 360);
        timer_set_oc_value(TIM3, TIM_OC3, 460);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

	cnt++;
	}


	// SETEAR LA SALIDA PARA LOS LEDS RUTINA #2
	gpio_clear(GPIOC, GPIO11); // 0
	gpio_set(GPIOC, GPIO12); // 1
	// resetear cnt
	cnt = 0;
	// RUTINA #2
	while(cnt != 4){

        timer_set_oc_value(TIM1, TIM_OC1, 656);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 721);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 786);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 851);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 916);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 981);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1046);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1111);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1176);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1241);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1312);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

	timer_set_oc_value(TIM2, TIM_OC1, 656);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 641);
	for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 626);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 611);
	for (i = 0; i < 4200000; i++) {

            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 596);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 581);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 566);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 551);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 536);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 521);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

	timer_set_oc_value(TIM2, TIM_OC1, 500);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

		timer_set_oc_value(TIM3, TIM_OC1, 656);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 624);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 592);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 560);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 528);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 496);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 464);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 432);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 400);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 368);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 328);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

        cnt++;	
	}


        // SETEAR LA SALIDA PARA LOS LEDS
	gpio_set(GPIOC, GPIO11); // 1 
	gpio_clear(GPIOC, GPIO12); // 0
        // resetear cnt
        cnt = 0;
        // RUTINA #3
        while(cnt != 5){

        timer_set_oc_value(TIM4, TIM_OC2, 360);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 374);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 388);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 402);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 416);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 430);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 444);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 458);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 472);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 486);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 500);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 460);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 466);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 472);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 478);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 484);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 490);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 496);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 502);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 508);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 514);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 524);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 514);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 508);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 502);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 496);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 490);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 484);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 478);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 472);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 466);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 460);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 486);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 472);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 458);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 444);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 430);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 416);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 402);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 388);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 374);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 360);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

        cnt++;  
        }


        // SETEAR LA SALIDA PARA LOS LEDS
        gpio_set(GPIOC, GPIO11); // 1
        gpio_set(GPIOC, GPIO12); // 1
        // resetear cnt
        cnt = 0;
        // RUTINA #4
        while(cnt != 2){

        timer_set_oc_value(TIM1, TIM_OC1, 656);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 721);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 786);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 851);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 916);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 981);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1046);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1111);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1176);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1241);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1312);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1241);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1176);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1111);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 1046);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 981);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 916);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 851);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 786);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 721);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM1, TIM_OC1, 656);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

	timer_set_oc_value(TIM2, TIM_OC1, 656);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 641);
	for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 626);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 611);
	for (i = 0; i < 4200000; i++) {

            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 596);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 581);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 566);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 551);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 536);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 521);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

	timer_set_oc_value(TIM2, TIM_OC1, 500);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 521);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 536);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 551);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 566);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 581);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 596);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 611);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 626);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 641);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

        timer_set_oc_value(TIM2, TIM_OC1, 656);
        for (i = 0; i < 4200000; i++) {
            __asm__("nop");
        }

	timer_set_oc_value(TIM3, TIM_OC1, 656);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 624);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 592);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 560);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 528);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 496);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 464);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 432);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 400);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 368);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 328);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 368);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 400);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 432);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 464);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 496);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 528);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 560);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 592);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 624);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC1, 656);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 360);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 374);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 388);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 402);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 416);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 430);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 444);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 458);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 472);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 486);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 500);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 486);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 472);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 458);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 444);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 430);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 416);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 402);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 388);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 374);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM4, TIM_OC2, 360);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 460);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 466);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 472);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 478);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 484);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 490);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 496);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 502);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 508);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 514);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 524);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 514);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 508);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 502);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 496);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 490);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 484);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 478);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 472);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 466);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }

timer_set_oc_value(TIM3, TIM_OC3, 460);
for (i = 0; i < 4200000; i++) { __asm__("nop"); }


        cnt++;  
        }

//CERRAR WHILE GENERAL
}

return 0;
}
