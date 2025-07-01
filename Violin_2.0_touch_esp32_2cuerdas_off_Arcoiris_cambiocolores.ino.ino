#include <Adafruit_NeoPixel.h>

// Configuración de los NeoPixels
#define PIN_NEOPIXEL 5 // D5 = GPIO5 (asegúrate de que este pin sea compatible con NeoPixels y que tu ESP32 pueda usarlo)
#define NUM_PIXELS 20 // Cambia este número según cuántos LEDs tengas

Adafruit_NeoPixel strip(NUM_PIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// Pines táctiles (Touch Pins)
// Asegúrate de que estos pines GPIO sean los correctos para tus sensores táctiles en el ESP32
#define TOUCH_COLOR_PIN 32   // GPIO32 (Touch4)
#define TOUCH_ON_OFF_PIN 33  // GPIO33 (Touch5)
#define TOUCH_COMP_PIN 14    // GPIO14 (Touch6)
#define TOUCH_RAINBOW_PIN 27 // GPIO27 (Touch7)

// Umbral táctil (ajustable): Un valor más bajo significa más sensibilidad
#define TOUCH_THRESHOLD 30

// Variables de estado
bool ledsEncendidos = false; // Esta variable no se usa actualmente en el código, pero se mantiene.
int secuenciaColor = 0;      // Índice del color actual en la matriz 'colores'
const int NUM_COLORES = 14;   // Número total de colores definidos
uint32_t colores[NUM_COLORES] = {
  strip.Color(255, 0, 0),   // Rojo
  strip.Color(0, 0, 0),     // Apagado (Negro) - Este es el color que quieres que su complementario sea también apagado.
  strip.Color(255, 127, 0), // Naranja
  strip.Color(0, 0, 0),     // Apagado (Negro) - Este es el color que quieres que su complementario sea también apagado.
  strip.Color(255, 255, 0), // Amarillo
  strip.Color(0, 0, 0),     // Apagado (Negro) - Este es el color que quieres que su complementario sea también apagado.
  strip.Color(0, 255, 0),   // Verde
  strip.Color(0, 0, 0),     // Apagado (Negro) - Este es el color que quieres que su complementario sea también apagado.
  strip.Color(0, 0, 255),   // Azul
  strip.Color(0, 0, 0),     // Apagado (Negro) - Este es el color que quieres que su complementario sea también apagado.
  strip.Color(75, 0, 130),  // Índigo
  strip.Color(0, 0, 0),     // Apagado (Negro) - Este es el color que quieres que su complementario sea también apagado.
  strip.Color(148, 0, 211),  // Violeta
  strip.Color(0, 0, 0)     // Apagado (Negro) - Este es el color que quieres que su complementario sea también apagado.

};

bool arcoirisActivo = false;         // Indica si el efecto arcoíris está en marcha
unsigned long arcoirisInicioTiempo = 0; // Guarda el millis() cuando el arcoíris se inició
const unsigned long arcoirisDuracion = 2000; // Duración del efecto arcoíris en milisegundos (2 segundos)

void setup() {
  Serial.begin(115200); // Inicia la comunicación serial para depuración
  strip.begin();        // Inicializa la tira de NeoPixels
  strip.clear();        // Apaga todos los píxeles (los pone en negro)
  strip.show();         // Envía los datos "apagados" a la tira
}


void loop() {
  // Lee los valores de los sensores táctiles
  int touchValueColor = touchRead(TOUCH_COLOR_PIN);
  int touchValueOnOff = touchRead(TOUCH_ON_OFF_PIN);
  int touchValueComp = touchRead(TOUCH_COMP_PIN);
  int touchValueRainbow = touchRead(TOUCH_RAINBOW_PIN);

  // Imprime los valores táctiles en el monitor serial para depuración
  Serial.print("Touch (Color - GPIO32): "); // Nota: en los comentarios originales era GPIO13 y 12, pero los defines son 32 y 33. Lo corregí para que coincida con los defines.
  Serial.print(touchValueColor);
  Serial.print(" | Touch (On/Off - GPIO33): ");
  Serial.print(touchValueOnOff);
  Serial.print(" | Touch (Comp - GPIO14): ");
  Serial.print(touchValueComp);
  Serial.print(" | Touch (Rainbow - GPIO27): ");
  Serial.println(touchValueRainbow);

  // --- Lógica de control de los sensores táctiles ---

  // Botón para cambiar colores (Ciclo entre la lista de colores)
  // Se activa si se toca el sensor de color Y NO se está tocando el sensor de arcoíris.
  if (touchValueColor < TOUCH_THRESHOLD && touchValueRainbow >= TOUCH_THRESHOLD) {
    cambiarColores();
  }

  // Botón para mostrar el color actual
  // Activa el color actualmente seleccionado.
  if (touchValueOnOff < TOUCH_THRESHOLD) {
    mostrarColorActual();
  }

  // Botón para mostrar el color complementario
  // Se activa si se toca el sensor de complemento Y el efecto arcoíris NO está activo.
  if (touchValueComp < TOUCH_THRESHOLD && !arcoirisActivo) {
    mostrarColorComplementario();
  }

  // Botón para activar el efecto arcoíris
  // Se activa si se toca el sensor de arcoíris Y el efecto arcoíris NO está activo actualmente.
  if (touchValueRainbow < TOUCH_THRESHOLD && !arcoirisActivo) {
    arcoirisActivo = true;
    arcoirisInicioTiempo = millis(); // Guarda el momento en que se activa
  }

  // Lógica para detener el efecto arcoíris después de su duración
  if (arcoirisActivo && (millis() - arcoirisInicioTiempo >= arcoirisDuracion)) {
      arcoirisActivo = false; // Desactiva el modo arcoíris
      strip.clear();          // Apaga los LEDs
      strip.show();
  }

  // Si el efecto arcoíris está activo, sigue ejecutándolo
  if (arcoirisActivo) {
    rainbowCycle(2); // Dibuja un ciclo arcoíris con un pequeño retraso
  }

  delay(50); // Pequeño retraso para evitar lecturas de touch muy rápidas y "ruido"
}

// --- Funciones auxiliares ---

// Función para cambiar al siguiente color en la secuencia
void cambiarColores() {
  secuenciaColor = (secuenciaColor + 1) % NUM_COLORES; // Avanza al siguiente color (vuelve a 0 si llega al final)
  Serial.print("Color seleccionado: ");
  // Imprime el nombre del color actual en el monitor serial
  switch (secuenciaColor) {
    case 0: Serial.println("Rojo"); break;
    case 1: Serial.println("Apagado"); break; // Caso para el color negro
    case 2: Serial.println("Naranja"); break;
    case 3: Serial.println("Apagado"); break; // Caso para el color negro
    case 4: Serial.println("Amarillo"); break;
    case 5: Serial.println("Apagado"); break; // Caso para el color negro
    case 6: Serial.println("Verde"); break;
    case 7: Serial.println("Apagado"); break; // Caso para el color negro
    case 8: Serial.println("Azul"); break;
    case 9: Serial.println("Apagado"); break; // Caso para el color negro
    case 10: Serial.println("Índigo"); break;
    case 11: Serial.println("Apagado"); break; // Caso para el color negro
    case 12: Serial.println("Violeta"); break;
    case 13: Serial.println("Apagado"); break; // Caso para el color negro

  }
  mostrarColor(colores[secuenciaColor], 1000); // Muestra el color por 1000ms (1 segundo)
}

// Función para mostrar el color actualmente seleccionado
void mostrarColorActual() {
  mostrarColor(colores[secuenciaColor], 32); // Muestra el color actual por 32ms
}

// Función para mostrar el color complementario del color actual
void mostrarColorComplementario() {
  uint32_t color = colores[secuenciaColor];       // Obtiene el color actual
  uint32_t colorComp = obtenerColorComplementario(color); // Calcula su complementario
  mostrarColor(colorComp, 32); // Muestra el color complementario por 32ms
}

// Función CRÍTICA: Calcula el color complementario
uint32_t obtenerColorComplementario(uint32_t color) {
  // === ESTE ES EL CAMBIO CLAVE ===
  // Si el color de entrada es negro (0,0,0), su complementario también es negro.
  if (color == strip.Color(0, 0, 0)) {
    return strip.Color(0, 0, 0);
  }
  // Para cualquier otro color, calcula el complementario normal.
  uint8_t r = (color >> 16) & 0xFF; // Extrae el componente Rojo
  uint8_t g = (color >> 8) & 0xFF;  // Extrae el componente Verde
  uint8_t b = (color >> 0) & 0xFF;  // Extrae el componente Azul
  return strip.Color(255 - r, 255 - g, 255 - b); // Devuelve el color complementario
}

// Función para encender todos los LEDs con un color específico por una duración
void mostrarColor(uint32_t color, unsigned long duracion) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color); // Establece el color para cada píxel
  }
  strip.show();      // Envía los colores a la tira de LEDs
  delay(duracion);   // Espera la duración especificada
  strip.clear();     // Apaga todos los píxeles
  strip.show();      // Envía los datos "apagados"
}

// Función de ciclo arcoíris (Rainbow Cycle)
void rainbowCycle(uint8_t wait) {
  for (uint16_t j = 0; j < 256; j++) { // Itera a través de todos los colores del círculo cromático (256 valores)
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      // Asigna un color a cada píxel, desplazando la fase del color a lo largo de la tira
      strip.setPixelColor(i, Wheel((i * 256 / strip.numPixels() + j) & 255));
    }
    strip.show(); // Actualiza la tira
    delay(wait);  // Pequeño retraso para la velocidad del efecto
  }
}

// Función para generar colores del arcoíris a partir de una posición (0-255)
// Simula un "círculo cromático" para crear el efecto arcoíris.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos; // Invierte la posición para un mapeo de color diferente
  if (WheelPos < 85) {
    // Primera sección: Rojo a Verde
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    // Segunda sección: Verde a Azul
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  // Tercera sección: Azul a Rojo
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}