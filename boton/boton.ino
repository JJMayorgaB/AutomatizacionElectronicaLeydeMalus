// Pines de los botones
#define BTN_RAPIDO 11
#define BTN_EXHAUSTIVO 12
#define BTN_ALINEAR 13

const unsigned long debounceDelay = 50;

void setup() {
    Serial.begin(9600);
    
    // Configurar botones con pull-up interno
    pinMode(BTN_RAPIDO, INPUT_PULLUP);
    pinMode(BTN_EXHAUSTIVO, INPUT_PULLUP);
    pinMode(BTN_ALINEAR, INPUT_PULLUP);
    
    Serial.println("Test de botones iniciado");
    Serial.println("Presione cualquier botón...\n");
}

void loop() {
    // Verificar botón 7
    if (digitalRead(BTN_RAPIDO) == LOW) {
        delay(debounceDelay);
        if (digitalRead(BTN_RAPIDO) == LOW) {
            Serial.println("BOTON 7 SELECCIONADO");
            esperarLiberacion(BTN_RAPIDO);
        }
    }
    
    // Verificar botón 8
    if (digitalRead(BTN_EXHAUSTIVO) == LOW) {
        delay(debounceDelay);
        if (digitalRead(BTN_EXHAUSTIVO) == LOW) {
            Serial.println("BOTON 8 SELECCIONADO");
            esperarLiberacion(BTN_EXHAUSTIVO);
        }
    }
    
    // Verificar botón 9
    if (digitalRead(BTN_ALINEAR) == LOW) {
        delay(debounceDelay);
        if (digitalRead(BTN_ALINEAR) == LOW) {
            Serial.println("BOTON 9 SELECCIONADO");
            esperarLiberacion(BTN_ALINEAR);
        }
    }
}

// Función para esperar a que se libere el botón
void esperarLiberacion(int pinBoton) {
    while (digitalRead(pinBoton) == LOW) {
        delay(10);
    }
    delay(debounceDelay);
}