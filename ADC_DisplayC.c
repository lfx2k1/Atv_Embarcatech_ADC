#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "pico/bootrom.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define JOYSTICK_PB 22
#define Botao_A 5
#define Botao_B 6
#define LED_R 13
#define LED_B 12
#define LED_G 11

volatile bool toggle_leds = true;
volatile bool toggle_green_led = false;
int border_style = 1;
volatile bool override_leds = false;

// Função para desenhar borda no display
void draw_border(ssd1306_t *ssd, uint8_t thickness, bool cor)
{
    ssd1306_fill(ssd, !cor); // Limpa o display

    for (uint8_t i = 0; i < thickness; i++)
    {
        ssd1306_rect(ssd, i, i, WIDTH - 1 - (i * 2), HEIGHT - 1 - (i * 2), cor, !cor);
    }
}

// Função de tratamento de interrupções dos botões e joystick
void irq_handler(uint gpio, uint32_t events)
{
    static absolute_time_t last_interrupt_time = {0};
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(last_interrupt_time, now) < 200000)
        return;
    last_interrupt_time = now;

    if (gpio == JOYSTICK_PB)
    {
        // Alternar o estado do LED verde
        toggle_green_led = !toggle_green_led;

        // Alternar entre borda fina e grossa
        border_style = (border_style == 1) ? 2 : 1;
    }
    else if (gpio == Botao_A)
    {
        // Ativar ou desativar LEDs PWM (azul e vermelho)
        toggle_leds = !toggle_leds;
    }
    else if (gpio == Botao_B)
    {
        // Ativar ou desativar LEDs PWM (azul e vermelho)
        reset_usb_boot(0, 0);
    }
}

// Função para ajustar o nível de PWM dos LEDs
void pwm_set_duty(uint gpio, uint16_t value)
{
    pwm_set_gpio_level(gpio, value);
}

int main()
{
    stdio_init_all();
    bool cor = true;

    // Configuração das GPIOs para botões e joystick
    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &irq_handler);

    // Para ser utilizado o modo BOOTSEL com botão B
    gpio_init(Botao_B);
    gpio_set_dir(Botao_B, GPIO_IN);
    gpio_pull_up(Botao_B);
    gpio_set_irq_enabled_with_callback(Botao_B, GPIO_IRQ_EDGE_FALL, true, &irq_handler);

    gpio_init(Botao_A);
    gpio_set_dir(Botao_A, GPIO_IN);
    gpio_pull_up(Botao_A);
    gpio_set_irq_enabled_with_callback(Botao_A, GPIO_IRQ_EDGE_FALL, true, &irq_handler);

    // Configuração do I2C para display SSD1306
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    draw_border(&ssd, border_style, cor);

    // Configuração do ADC para joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    // Configuração do PWM para LEDs vermelho e azul
    gpio_set_function(LED_R, GPIO_FUNC_PWM);
    gpio_set_function(LED_B, GPIO_FUNC_PWM);
    // LED verde como saída digital
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);

    uint slice_r = pwm_gpio_to_slice_num(LED_R);
    uint slice_b = pwm_gpio_to_slice_num(LED_B);
    
    // Configuração dos parâmetros de PWM
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0);
    pwm_config_set_wrap(&config, 4095);

    pwm_init(slice_r, &config, true);
    pwm_init(slice_b, &config, true);

    uint16_t adc_x, adc_y;
    int x_pos = 60, y_pos = 28;

    while (true)
    {
        // Leitura dos valores do joystick
        adc_select_input(1);
        adc_x = adc_read();
        adc_select_input(0);
        adc_y = adc_read();

        // Controle dos LEDs
        if (toggle_leds)
        {
            // Calcula a distância do centro (2048) para cada eixo
            int32_t dist_x = abs(2048 - adc_x);
            int32_t dist_y = abs(2048 - adc_y);

            // Define uma zona morta (deadzone) ao redor do centro
            const int32_t deadzone = 200;

            // Calcula o PWM para os LEDs vermelho (X) e azul (Y)
            uint16_t duty_r = (dist_x > deadzone) ? ((dist_x - deadzone) * 4095) / (2048 - deadzone) : 0;
            uint16_t duty_b = (dist_y > deadzone) ? ((dist_y - deadzone) * 4095) / (2048 - deadzone) : 0;

            // Limita o valor máximo do PWM
            duty_r = (duty_r > 4095) ? 4095 : duty_r;
            duty_b = (duty_b > 4095) ? 4095 : duty_b;

            // Aplica o PWM aos LEDs vermelho e azul
            pwm_set_duty(LED_R, duty_r);
            pwm_set_duty(LED_B, duty_b);
        }
        else
        {
            // LEDs vermelho e azul desligados quando toggle_leds é falso
            pwm_set_duty(LED_R, 0);
            pwm_set_duty(LED_B, 0);
        }

        // Controle independente do LED verde
        gpio_put(LED_G, toggle_green_led);

        // Atualização da posição do quadrado no display SSD1306
        y_pos = ((adc_x * (WIDTH - 8)) / 4095); // Remove a inversão do eixo Y
        x_pos = HEIGHT - 8 - ((adc_y * (HEIGHT - 8)) / 4095);

        // Garante que o quadrado fique dentro dos limites do display
        if (y_pos < 4)
            y_pos = 4;
        if (y_pos > 128 - 13)
            y_pos = 128 - 13;

        if (x_pos < 4)
            x_pos = 4;
        if (x_pos > 64 - 13)
            x_pos = 64 - 13;

        // Preenche o display e desenha a borda e o quadrado
        ssd1306_fill(&ssd, false);
        draw_border(&ssd, border_style, cor); // Desenhar borda de acordo com o estilo
        ssd1306_rect(&ssd, x_pos, y_pos, 8, 8, true, true);

        ssd1306_send_data(&ssd);
    }
}
