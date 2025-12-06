#include <AccelStepper.h>
#include <Wire.h>
#include <BH1750.h>

// Pines del motor
#define STEP_PIN 2
#define DIR_PIN 3

// Pines de los botones
#define BTN_RAPIDO 11      // Barrido rápido (36 puntos)
#define BTN_EXHAUSTIVO 12 // Medición exhaustiva (cada grado)
#define BTN_ALINEAR 13     // Retornar a posición inicial (0°)

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
BH1750 lightMeter;

const int stepsPerRevolution = 200 * 32; // 200 pasos/revolución * 32 micropasos

// Variables de estado
bool modoActivo = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
    Serial.begin(9600);

    // Configurar motor
    stepper.setMaxSpeed(1000);
    stepper.setAcceleration(10000);

    // Configurar sensor de luz
    Wire.begin();
    
    //CONTINUOUS_HIGH_RES_MODE Rango max: 55k lux, res: 1 lux
    //CONTINUOUS_LOW_RES_MODE Rango max: 100k lux, res: 4-8 lux
    if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
        Serial.println("BH1750 inicializado");
    } else {
        Serial.println("ERROR: BH1750 no responde");
        while(1);
    }

    // Configurar botones con pull-up interno
    pinMode(BTN_RAPIDO, INPUT_PULLUP);
    pinMode(BTN_EXHAUSTIVO, INPUT_PULLUP);
    pinMode(BTN_ALINEAR, INPUT_PULLUP);

    Serial.println("Sistema iniciado. Presione un botón para comenzar:");
    Serial.println("- Botón pin 7: Barrido rápido (36 puntos)");
    Serial.println("- Botón pin 8: Medición exhaustiva (cada grado)");
    Serial.println("- Botón pin 9: Retornar a posición inicial (0°)");
    
    delay(5000);
}

void loop() {
    // Verificar botones (activos en LOW por pull-up)
    if (digitalRead(BTN_RAPIDO) == LOW && !modoActivo) {
        delay(debounceDelay);
        if (digitalRead(BTN_RAPIDO) == LOW) {
            Serial.println("\n=== MODO: Barrido Rápido ===");
            barridoRapido();
            esperarLiberacion(BTN_RAPIDO);
        }
    }
    
    if (digitalRead(BTN_EXHAUSTIVO) == LOW && !modoActivo) {
        delay(debounceDelay);
        if (digitalRead(BTN_EXHAUSTIVO) == LOW) {
            Serial.println("\n=== MODO: Medición Exhaustiva ===");
            medicionExhaustiva();
            esperarLiberacion(BTN_EXHAUSTIVO);
        }
    }
    
    if (digitalRead(BTN_ALINEAR) == LOW && !modoActivo) {
        delay(debounceDelay);
        if (digitalRead(BTN_ALINEAR) == LOW) {
            Serial.println("\n=== Retornando a posición inicial (0°) ===");
            moveToAngle(0);
            Serial.println("Motor en posición inicial.\n");
            esperarLiberacion(BTN_ALINEAR);
        }
    }
}

// Modo 1: Barrido rápido con 36 puntos (cada 10 grados)
void barridoRapido() {
    modoActivo = true;

    Serial.println("Angulo,Intensidad(lux)"); // Cambiar ; por , para consistencia
    
    for (int angle = 0; angle <= 360; angle += 15) {
        moveToAngle(angle);
        delay(1000);
        measureIntensity();
        delay(100);
    }
    
    Serial.println("FIN"); // Agregar señal de finalización

    modoActivo = false;
}

// Modo 2: Medición exhaustiva cada grado
void medicionExhaustiva() {
    modoActivo = true;

    Serial.println("Angulo,Intensidad(lux)");
    
    for (int angle = 0; angle <= 360; angle += 2) {
        moveToAngle(angle);
        delay(500);
        measureIntensity();
        delay(50);
    }
    
    Serial.println("FIN"); // Agregar señal de finalización

    modoActivo = false;
}

// Función para mover el motor a un ángulo específico
void moveToAngle(int angle) {
    long targetPosition = map(angle, 0, 360, 0, stepsPerRevolution);
    stepper.moveTo(targetPosition);
    stepper.runToPosition();
}

// Función para medir intensidad y enviar datos por serial
void measureIntensity() {
    float lux = lightMeter.readLightLevel();
    int currentAngle = map(stepper.currentPosition(), 0, stepsPerRevolution, 0, 360);
    
    Serial.print(currentAngle);
    Serial.print(",");
    Serial.println(lux);
}

// Función para esperar a que se libere el botón
void esperarLiberacion(int pinBoton) {
    while (digitalRead(pinBoton) == LOW) {
        delay(10);
    }
    delay(debounceDelay);
}
