#include <Wire.h>

void setup() {
    Serial.begin(9600);
    Serial.println("=== Diagnóstico BH1750 Avanzado ===\n");
    
    Wire.begin();
    Wire.setClock(100000); // Velocidad estándar I2C
    delay(500);
    
    Serial.println("Configuración I2C:");
    Serial.println("- Velocidad: 100kHz");
    Serial.println("- Pull-ups: Internas activadas\n");
    
    // Probar ambas direcciones con más detalle
    Serial.println("Probando direcciones conocidas:");
    probarDireccion(0x23, "0x23 (ADDR a GND o flotante)");
    probarDireccion(0x5C, "0x5C (ADDR a VCC)");
    
    // Escaneo completo detallado
    Serial.println("\n--- Escaneo completo I2C ---");
    byte dispositivos = 0;
    
    for (byte addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        byte error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.print("✓ Dispositivo encontrado en: 0x");
            if (addr < 16) Serial.print("0");
            Serial.print(addr, HEX);
            Serial.print(" (decimal: ");
            Serial.print(addr);
            Serial.println(")");
            dispositivos++;
        } else if (error == 4) {
            Serial.print("⚠ Error en: 0x");
            if (addr < 16) Serial.print("0");
            Serial.println(addr, HEX);
        }
        delay(5);
    }
    
    Serial.print("\nTotal de dispositivos encontrados: ");
    Serial.println(dispositivos);
    
    if (dispositivos == 0) {
        Serial.println("\n❌ PROBLEMA DETECTADO");
        Serial.println("\nVerifica:");
        Serial.println("1. VCC conectado a 5V (NO 3.3V)");
        Serial.println("2. GND conectado correctamente");
        Serial.println("3. SDA en pin A4");
        Serial.println("4. SCL en pin A5");
        Serial.println("5. Cables en buen estado");
        Serial.println("6. Protoboard funcionando");
        Serial.println("\nSi todo está bien, el módulo puede estar dañado");
    }
}

void probarDireccion(byte addr, String descripcion) {
    Serial.print("  ");
    Serial.print(descripcion);
    Serial.print(" ... ");
    
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
        Serial.println("✓ RESPONDE");
        
        // Intentar leer datos
        Wire.requestFrom(addr, (byte)2);
        if (Wire.available() == 2) {
            Serial.println("    → Sensor lee datos correctamente");
        }
    } else if (error == 2) {
        Serial.println("✗ NACK en dirección");
    } else if (error == 3) {
        Serial.println("✗ NACK en datos");
    } else if (error == 4) {
        Serial.println("✗ Error desconocido");
    } else {
        Serial.println("✗ No responde");
    }
}

void loop() {
    // Monitoreo continuo cada 3 segundos
    delay(3000);
    Serial.println("\n--- Reescaneando ---");
    
    for (byte addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.print("0x");
            if (addr < 16) Serial.print("0");
            Serial.print(addr, HEX);
            Serial.print(" ");
        }
    }
    Serial.println("(fin)");
}