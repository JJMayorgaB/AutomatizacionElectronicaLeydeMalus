#include <AccelStepper.h>

// Pines del motor
#define STEP_PIN 2
#define DIR_PIN 3

// Configuración del driver
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// Configuración de pasos
const int stepsPerRevolution = 200 *32;
bool invertDirection = false;
bool isMoving = false;  // Bandera para controlar si está en movimiento

void setup() {
    Serial.begin(9600);
    
    // Configurar velocidad y aceleración
    stepper.setMaxSpeed(1000);      
    stepper.setAcceleration(10000); 
    stepper.setCurrentPosition(0);
    
    Serial.println("=== Control de Motor Paso a Paso ===");
    Serial.println("Comandos disponibles:");
    Serial.println("- Numero positivo: mover en sentido horario (en pasos)");
    Serial.println("- Numero negativo: mover en sentido antihorario (en pasos)");
    Serial.println("- 'h': volver a posición inicial (home)");
    Serial.println("- 'p': mostrar posición actual");
    Serial.println("- 's[valor]': cambiar velocidad (ej: s200 para 200 pasos/seg)");
    Serial.println("- 'i': invertir dirección del motor");
    Serial.println("- 'x': detener movimiento actual");
    Serial.println();
}

void loop() {
    // Procesar comandos solo si NO está en movimiento
    if (Serial.available() > 0 && !isMoving) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        
        if (input.length() > 0) {
            processCommand(input);
        }
    }
    
    // Ejecutar movimiento continuamente
    if (stepper.distanceToGo() != 0) {
        isMoving = true;
        stepper.run();
    } else {
        if (isMoving) {
            // El movimiento acaba de terminar
            Serial.print("Movimiento completado. Posición: ");
            Serial.println(stepper.currentPosition());
            Serial.println("---");
            isMoving = false;
        }
    }
}

void processCommand(String cmd) {
    char firstChar = cmd.charAt(0);
    
    // Comando para detener
    if (firstChar == 'x' || firstChar == 'X') {
        stepper.stop();
        stepper.setCurrentPosition(stepper.currentPosition());
        Serial.println("Motor detenido");
        isMoving = false;
    }
    // Comando para invertir dirección
    else if (firstChar == 'i' || firstChar == 'I') {
        invertDirection = !invertDirection;
        Serial.print("Dirección invertida: ");
        Serial.println(invertDirection ? "SÍ" : "NO");
    }
    // Comando para cambiar velocidad
    else if (firstChar == 's' || firstChar == 'S') {
        float newSpeed = cmd.substring(1).toFloat();
        if (newSpeed > 0) {
            stepper.setMaxSpeed(newSpeed);
            Serial.print("Velocidad actualizada a: ");
            Serial.print(newSpeed);
            Serial.println(" pasos/seg");
        }
    }
    // Comando para volver a home
    else if (firstChar == 'h' || firstChar == 'H') {
        Serial.println("Volviendo a posición inicial...");
        stepper.moveTo(0);
        isMoving = true;
    }
    // Comando para mostrar posición
    else if (firstChar == 'p' || firstChar == 'P') {
        Serial.print("Posición actual: ");
        Serial.print(stepper.currentPosition());
        Serial.print(" pasos, Objetivo: ");
        Serial.print(stepper.targetPosition());
        Serial.print(", Distancia: ");
        Serial.print(stepper.distanceToGo());
        Serial.print(", En movimiento: ");
        Serial.println(isMoving ? "SÍ" : "NO");
    }
    // Mover número específico de pasos
    else {
        long steps = cmd.toInt();
        if (steps != 0 || cmd == "0") {
            // Invertir si está activado
            if (invertDirection) {
                steps = -steps;
            }
            
            Serial.print("Comando: ");
            Serial.print(cmd);
            Serial.print(" -> Moviendo ");
            Serial.print(steps);
            Serial.print(" pasos desde ");
            Serial.print(stepper.currentPosition());
            Serial.print(" hacia ");
            Serial.println(stepper.currentPosition() + steps);
            
            stepper.move(steps);
            isMoving = true;
        } else {
            Serial.println("Comando no reconocido");
        }
    }
}