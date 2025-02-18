# Atividade-ADC-PWM-Joystick-BitDogLab

## Repositório
Repositório para atividade de controle de LEDs RGB e display OLED utilizando ADC e PWM no Raspberry Pi Pico W com a placa BitDogLab.

## Descrição
Este projeto implementa um sistema de controle de intensidade de LEDs RGB e movimentação de um quadrado em um display SSD1306, com base nos valores de entrada de um joystick analógico, utilizando o Raspberry Pi Pico W.

## Vídeo com explicação e demonstração
Link: https://youtu.be/2P6vsuYNhhM

## Componentes Utilizados
- **LEDs RGB**:
  - **Azul (GPIO 11)** - Controlado pelo eixo Y do joystick.
  - **Vermelho (GPIO 12)** - Controlado pelo eixo X do joystick.
  - **Verde (GPIO 13)** - Alternado pelo botão do joystick.

- **Joystick**:
  - **Eixo X (GPIO 26)** - Controla o brilho do LED Vermelho.
  - **Eixo Y (GPIO 27)** - Controla o brilho do LED Azul.
  - **Botão (GPIO 22)** - Alterna o LED Verde e altera a borda do display.

- **Botão A**:
  - **GPIO 5** - Ativa ou desativa os LEDs PWM.
- **Botão B**:
  - **GPIO 6** - Coloca a placa em modo de gravação.

- **Display SSD1306**:
  - Controlado via **I2C (GPIO 14 e GPIO 15)** - Exibe um quadrado de 8x8 pixels que se move conforme os valores do joystick.

## Funcionamento
### Controle dos LEDs
- O LED Azul ajusta seu brilho conforme o eixo Y do joystick (apagado no centro e brilho máximo nos extremos 0 e 4095).
- O LED Vermelho ajusta seu brilho conforme o eixo X do joystick (apagado no centro e brilho máximo nos extremos 0 e 4095).
- Os LEDs são controlados via **PWM** para transição suave.

### Display SSD1306
- Exibe um **quadrado de 8x8 pixels**, inicialmente centralizado.
- O quadrado se move conforme os valores do joystick.

### Botões
- **Botão do Joystick (GPIO 22)**:
  - Alterna o estado do LED Verde a cada pressionamento.
  - Modifica a borda do display para indicar a interação.
- **Botão A (GPIO 5)**:
  - Liga/desliga os LEDs PWM.
  - **Botão B (GPIO 6) - EXTRA**:
  - Coloca a placa em modo de gravação.

## Utilização do Programa

### 1. Pré-requisitos
Antes de rodar o programa, certifique-se de ter os seguintes itens configurados:
- **Placa BitDogLab**
- **Ambiente de desenvolvimento**: SDK do Raspberry Pi Pico configurado (CMake, GCC ARM, etc.).
- **Cabo USB**: Para alimentação e comunicação serial com o Raspberry Pi Pico.

### 2. Compilação e Upload

Clone este repositório:

git clone https://github.com/lfx2k1/Atv_Embarcatech_ADC.git

cd Atv_Embarcatech_ADC

Compile o código e gere o arquivo .uf2.

Para carregar o código na placa BitDogLab:
1. Conecte a placa ao PC segurando o botão **BOOTSEL**.
2. O dispositivo será montado como um armazenamento USB.
3. Copie o arquivo .uf2 gerado para a unidade correspondente à BitDogLab.

### 3. Execução
Após carregar o código, o sistema estará pronto para interação:
- Movimente o joystick para alterar o brilho dos LEDs.
- Observe o quadrado se mover no display conforme os eixos X e Y do joystick.
- Pressione o botão do joystick para alternar o LED Verde e mudar a borda do display.
- Pressione o botão A para ativar/desativar os LEDs RGB.
- Pressione o botão B para colocar a placa BitDogLab em modo de gravação.

