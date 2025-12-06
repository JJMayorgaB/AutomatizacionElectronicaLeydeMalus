#include <AccelStepper.h>
#include <Wire.h>
#include <BH1750.h>

// pines
#define STEP_PIN 2
#define DIR_PIN 3

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
BH1750 lightMeter;

const int stepsPerRevolution = 200 * 32; // 200 pasos/revolución * 32 micropasos
const int stepSize = 5; // Tamaño del paso en grados
bool medicionCompleta = false; // Bandera para controlar una sola medición

void setup() {
    Serial.begin(9600);

    stepper.setMaxSpeed(1000);
    stepper.setAcceleration(10000);

    Wire.begin();
    
    //CONTINUOUS_HIGH_RES_MODE Rango max: 55k lux, res: 1 lux
    //CONTINUOUS_LOW_RES_MODE Rango max: 100k lux, res: 4-8 lux
    if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
        Serial.println("BH1750 inicializado");
    } else {
        Serial.println("ERROR: BH1750 no responde");
        while(1);
    }

    delay(5000); // Dar tiempo para que Python se conecte
}

void loop() {
    if (!medicionCompleta) {
        for (int angle = 0; angle <= 360; angle += stepSize){
            moveToAngle(angle);
            delay(1000); // Esperar a que el motor se estabilice
            measureIntensity();
            delay(100);
        }
        moveToAngle(0); // Volver a la posición inicial en 0 grados
        medicionCompleta = true; // Marcar como completado
        Serial.println("FIN"); // Indicar que terminó
    }
    // No hacer nada más - el motor queda detenido en 0°
}

// Función para colocar el motor en un ángulo específico
void moveToAngle(int angle) {
    long targetPosition = map(angle, 0, 360, 0, stepsPerRevolution);
    stepper.moveTo(targetPosition);
    stepper.runToPosition();
    
    // DEBUG: Verificar posición después del movimiento
    // Serial.print("DEBUG - Target:");
    // Serial.print(targetPosition);
    // Serial.print(" Current:");
    // Serial.println(stepper.currentPosition());
}

// Función para medir la intensidad de luz y enviar los datos por serial
void measureIntensity() {
    float lux = lightMeter.readLightLevel();
    long currentPos = stepper.currentPosition();
    int currentAngle = map(currentPos, 0, stepsPerRevolution, 0, 360);

    //Redondear currentAngle al múltiplo más cercano a 5°
    int roundedAngle = round(currentAngle / 5.0) * 5;

    // DEBUG: Ver valores intermedios (DESCOMENTAR PARA PROBAR)
    // Serial.print("Pos:");
    // Serial.print(currentPos);
    // Serial.print(" RawAngle:");
    // Serial.print(currentAngle);
    // Serial.print(" Rounded:");
    // Serial.print(roundedAngle);
    // Serial.print(" -> ");

    // Enviar los datos obtenidos por serial
    Serial.print(roundedAngle); 
    Serial.print(",");
    Serial.println(lux);
}