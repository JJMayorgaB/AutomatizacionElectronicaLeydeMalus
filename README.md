# Automatización Electrónica - Ley de Malus

Este proyecto implementa un sistema automatizado para verificar experimentalmente la **Ley de Malus** utilizando Arduino, un motor paso a paso NEMA17 y un sensor de luz BH1750.

## 📋 Descripción

La Ley de Malus describe cómo varía la intensidad de la luz polarizada cuando pasa a través de un polarizador rotatorio. Este proyecto automatiza la medición de intensidad luminosa en función del ángulo de rotación y ajusta los datos experimentales a la función:

$$I = A \cdot \cos^2(\theta - \theta_p)$$

Donde:
- $I$ es la intensidad luminosa (lux)
- $\theta$ es el ángulo de rotación
- $A$ es la amplitud o intensidad máxima
- $\theta_p$ es el angulo de polarización

## 🛠️ Componentes del Proyecto

### Hardware
- **Arduino UNO** (compatible con comunicación serial)
- **Motor paso a paso NEMA17** con driver DRV8825
- **Sensor de luz BH1750** (I2C)
- **Polarizadores** (para realizar el experimento óptico)

### Software
- **Python 3.13** para análisis de datos
- **Arduino IDE** para programación del microcontrolador

## 📁 Estructura del Repositorio

```
AutomatizaciónElectronicaLeydeMalus/
│
├── analisis.py                  # Script principal de análisis en Python
│
├── boton/
│   └── boton.ino               # Control básico con botón para debugging
│
├── lux/
│   └── lux.ino                 # Diagnóstico y prueba del sensor BH1750 para debugging
│
├── motor/
│   └── motor.ino               # Control del motor con medición de luz
│
├── prueba/
│   └── prueba.ino              # Código de prueba general para diagnóstico del motor
│
└── resultados/
    ├── lux_data.csv            # Datos experimentales (ángulo vs lux)
    └── resultados_ajuste.txt   # Parámetros del ajuste y estadísticas
│    
└── README.md                    # Este archivo
```

## 🚀 Instalación y Uso

### Requisitos de Python

```bash
pip install pyserial matplotlib numpy scipy
```

### Requisitos de Arduino

Librerías necesarias (instalar desde el Library Manager):
- `AccelStepper` - Control de motores paso a paso
- `BH1750` - Comunicación con sensor de luz
- `Wire` - Comunicación I2C (incluida por defecto)

### Pasos para ejecutar

1. **Configurar el hardware:**
   - Conecta el motor paso a paso a los pines 2 (STEP) y 3 (DIR)
   - Conecta el sensor BH1750 vía I2C (SDA/SCL)
   - Monta los polarizadores en el sistema mecánico

2. **Cargar el código Arduino:**
   ```bash
   # Abre motor/motor.ino en Arduino IDE
   # Selecciona tu placa y puerto COM
   # Sube el código
   ```

3. **Configurar el puerto serial:**
   - Edita `analisis.py` y ajusta el puerto COM:
   ```python
   ser = serial.Serial('COM5', 9600, timeout=1)  # Cambia COM5 por tu puerto
   ```

4. **Ejecutar el análisis:**
   ```bash
   python analisis.py
   ```

5. **Resultados:**
   - El script recopilará datos automáticamente
   - Generará gráficas de los datos y el ajuste
   - Guardará los resultados en la carpeta `resultados/`

## 📊 Salidas del Programa

El programa genera:

1. **Gráfica en tiempo real** durante la adquisición de datos
2. **Archivo CSV** (`resultados/lux_data.csv`) con los datos experimentales
3. **Archivo de texto** (`resultados/resultados_ajuste.txt`) con:
   - Parámetros del ajuste (A, θ₀)
   - Incertidumbres
   - Coeficiente de determinación (R²)
4. **Gráficas finales** mostrando datos experimentales y curva ajustada

## 🔧 Configuración Avanzada

### Ajustes del Motor

En `motor.ino`:
```cpp
const int stepSize = 5;  // Paso angular en grados (ajustable)
stepper.setMaxSpeed(1000);  // Velocidad máxima
stepper.setAcceleration(10000);  // Aceleración
```

### Modo del Sensor BH1750

```cpp
// CONTINUOUS_HIGH_RES_MODE: Rango max 55k lux, resolución 1 lux
// CONTINUOUS_LOW_RES_MODE: Rango max 100k lux, resolución 4-8 lux
lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
```

## 📈 Formato de LaTeX

El script de análisis genera gráficas con formato LaTeX para publicaciones científicas:
- Fuente Computer Modern
- Etiquetas en LaTeX
- Fondo blanco para impresión
- Grid sutil

## 🐛 Solución de Problemas

### El sensor BH1750 no responde
- Ejecuta `lux/lux.ino` para diagnóstico
- Verifica las conexiones I2C (SDA/SCL)
- Prueba las direcciones 0x23 o 0x5C

### Error de puerto serial
- Verifica que el puerto COM sea correcto
- Asegúrate de que Arduino IDE no esté usando el puerto
- En Windows, revisa el Administrador de Dispositivos

### Motor no se mueve correctamente
- Verifica la alimentación del driver
- Ajusta los micropasos del driver (debe coincidir con el código)
- Revisa las conexiones STEP y DIR

## 👥 Autores

Proyecto desarrollado para el curso de Electrónica Digital.

## 📄 Licencia

Este proyecto es de código abierto y está disponible para fines educativos.
