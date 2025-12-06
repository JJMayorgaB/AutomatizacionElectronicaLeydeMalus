import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import csv
from scipy.optimize import curve_fit
import time
import os

plt.rcParams.update({
    'text.usetex': True,
    'text.latex.preamble': r'\usepackage{amsmath}',
    'font.family': 'serif',
    'font.serif': ['Computer Modern'],
    'font.size': 24,
    'axes.labelsize': 22,
    'axes.titlesize': 24,
    'xtick.labelsize': 20,
    'ytick.labelsize': 20,
    'legend.fontsize': 20,
    'figure.titlesize': 22,
    'axes.facecolor': 'white',      # Fondo blanco
    'figure.facecolor': 'white',    # Figura con fondo blanco
    'axes.edgecolor': 'black',      # Borde negro
    'axes.linewidth': 1.0,          # Grosor del borde
    'grid.alpha': 0.3,              # Grid sutil
    'grid.color': 'gray',           # Color de la grid
    'axes.axisbelow': True,         # Grid detrás de los datos
})

# Crear carpeta de resultados si no existe
CARPETA_RESULTADOS = os.path.join(os.path.dirname(__file__), 'resultados')
if not os.path.exists(CARPETA_RESULTADOS):
    os.makedirs(CARPETA_RESULTADOS)
    print(f"Carpeta '{CARPETA_RESULTADOS}' creada.")

# Configuración del puerto serial
ser = serial.Serial('COM5', 9600, timeout=1)

# Esperar a que Arduino se reinicie
print("Esperando a que Arduino se inicialice...")
time.sleep(3)  # Esperar 3 segundos

# Limpiar buffer inicial
ser.reset_input_buffer()
print("Arduino listo. Iniciando lectura de datos...")

# Variables para almacenar datos
angulos = []
lux = []
angulo_max = 360  # Ángulo máximo de medición en grados

# Función para actualizar la gráfica en tiempo real
def actualizar_grafica(frame):
    global angulos, lux

    # Leer datos del puerto serial
    linea = ser.readline().decode('utf-8').strip()
    if linea:
        try:
            # Ignorar líneas que no contengan coma (no son datos)
            if ',' not in linea:
                # Verificar si es la señal de fin
                if linea == "FIN":
                    print("Medición completa detectada")
                    ani.event_source.stop()
                    guardar_datos()
                    return
                print(f"Línea ignorada: {linea}")
                return
            
            angulo, iluminancia = map(float, linea.split(','))
            
            # Validar que sean números razonables
            if 0 <= angulo <= 360 and iluminancia >= 0:
                angulos.append(angulo)
                lux.append(iluminancia)
                print(f"Recibido: Ángulo={angulo}°, Lux={iluminancia}")
            else:
                print(f"Datos fuera de rango ignorados: {linea}")

        except ValueError:
            print(f"Línea con formato inválido ignorada: {linea}")
            return

    # Actualizar la gráfica con los datos recibidos
    if len(angulos) > 0:
        ax.clear()
        ax.plot(angulos, lux, 'b-', marker='o', markersize=3, linewidth=1)
        ax.set_xlabel(r'Ángulo ($^\circ$)')
        ax.set_ylabel(r'Intensidad luminosa (lux)')
        ax.set_title(r'Medición de intensidad luminosa en tiempo real')
        ax.grid(True, alpha=0.3)
        ax.set_xlim(0, 360)
        
        # Ajustar límite Y dinámicamente
        if len(lux) > 0:
            max_lux = max(lux)
            ax.set_ylim(0, max_lux * 1.1)

def graficar_polar():
    """Función para generar gráfica en coordenadas polares"""
    print("\nGenerando gráfica polar...")
    
    # Convertir ángulos a radianes
    angulos_rad = np.radians(angulos)
    
    # Crear la gráfica polar
    fig_polar = plt.figure(figsize=(12, 10))
    ax_polar = fig_polar.add_subplot(111, projection='polar')
    
    # Graficar los datos
    ax_polar.plot(angulos_rad, lux, 'bo-', linewidth=1, markersize=3, label='Iluminancia')
    
    # Configurar la gráfica
    ax_polar.set_title(r'Intensidad Luminosa vs Ángulo (Coordenadas Polares)', pad=20)
    
    # Configurar el label del eje radial con mejor posición
    ax_polar.set_ylabel(r'Iluminancia (lux)', labelpad=40)
    ax_polar.yaxis.set_label_coords(-0.1, 0.5)  # Mover el label más a la izquierda
    
    ax_polar.legend(loc='upper right')
    ax_polar.grid(True)
    
    # Guardar la gráfica
    ruta_polar = os.path.join(CARPETA_RESULTADOS, 'grafica_polar.png')
    fig_polar.savefig(ruta_polar, dpi=300, bbox_inches='tight')
    print(f"Gráfica polar guardada en '{ruta_polar}'")
    plt.show(block=True)  # Mostrar y esperar a que se cierre

def realizar_ajuste():
    """Función para realizar el ajuste con coseno cuadrado"""
    print("\nRealizando ajuste de datos...")
    
    # Convertir ángulos a radianes
    angulos_rad = np.radians(angulos)
    iluminancia = np.array(lux)
    
    # Definir la función de ajuste: I = A * cos²(x - x0)
    def modelo_coseno(x, A, x0):
        return A * np.cos(x - x0)**2
    
    # Valores iniciales para el ajuste
    max_idx = np.argmax(iluminancia)
    A_inicial = iluminancia[max_idx]
    x0_inicial = angulos_rad[max_idx]
    
    try:
        parametros, covarianza = curve_fit(
            modelo_coseno, 
            angulos_rad, 
            iluminancia,
            p0=[A_inicial, x0_inicial],
            maxfev=10000
        )
        
        A_ajustado, x0_ajustado = parametros
        errores = np.sqrt(np.diag(covarianza))
        x0_grados = np.rad2deg(x0_ajustado)
        
        print("=" * 50)
        print("RESULTADOS DEL AJUSTE")
        print("=" * 50)
        print(f"Función ajustada: I = A * cos²(θ - θ₀)")
        print(f"\nParámetros:")
        print(f"  A (Amplitud)    = {A_ajustado:.2f} ± {errores[0]:.2f} lux")
        print(f"  θ₀ (Desfase)    = {x0_grados:.2f}° ({x0_ajustado:.4f} rad)")
        print(f"                  = {x0_grados:.2f} ± {np.rad2deg(errores[1]):.2f}°")
        print("=" * 50)
        
        # Crear un rango continuo de ángulos para la curva ajustada
        angulos_continuos = np.linspace(0, 360, 1000)
        angulos_continuos_rad = np.radians(angulos_continuos)
        iluminancia_ajustada = modelo_coseno(angulos_continuos_rad, A_ajustado, x0_ajustado)
        
        # Calcular R² usando los datos originales
        iluminancia_ajustada_original = modelo_coseno(angulos_rad, A_ajustado, x0_ajustado)
        residuos = iluminancia - iluminancia_ajustada_original
        ss_res = np.sum(residuos**2)
        ss_tot = np.sum((iluminancia - np.mean(iluminancia))**2)
        r_cuadrado = 1 - (ss_res / ss_tot)
        print(f"R² = {r_cuadrado:.4f}")
        print("=" * 50)
        
        # Crear gráfica de ajuste
        fig_ajuste = plt.figure(figsize=(12, 8))
        
        plt.plot(angulos, iluminancia, 'b.', label='Datos experimentales', markersize=6)
        plt.plot(angulos_continuos, iluminancia_ajustada, 'r-', 
                 label=rf'Ajuste: $I = {A_ajustado:.0f} \cos^2(\theta - {x0_grados:.1f}^\circ)$', linewidth=1)
        
        plt.xlabel(r'Ángulo ($^\circ$)')
        plt.ylabel(r'Iluminancia (lux)')
        plt.title(r'Ajuste de Iluminancia con función $A\cos^2(\theta-\theta_p)$', fontweight='bold')
        plt.legend(loc='upper right')
        plt.grid(True, alpha=0.3)
        
        # Guardar gráfica
        ruta_ajuste = os.path.join(CARPETA_RESULTADOS, 'ajuste_coseno.png')
        fig_ajuste.savefig(ruta_ajuste, dpi=300, bbox_inches='tight')
        print(f"\nGráfica de ajuste guardada en '{ruta_ajuste}'")
        plt.show(block=True)  # Mostrar y esperar a que se cierre
        
        # Guardar resultados en archivo de texto
        ruta_resultados = os.path.join(CARPETA_RESULTADOS, 'resultados_ajuste.txt')
        with open(ruta_resultados, 'w', encoding='utf-8') as f:
            f.write("RESULTADOS DEL AJUSTE\n")
            f.write("=" * 50 + "\n")
            f.write(f"Función ajustada: I = A * cos²(θ - θ₀)\n\n")
            f.write(f"Parámetros:\n")
            f.write(f"  A (Amplitud)    = {A_ajustado:.2f} ± {errores[0]:.2f} lux\n")
            f.write(f"  θ₀ (Desfase)    = {x0_grados:.2f} ± {np.rad2deg(errores[1]):.2f}°\n")
            f.write(f"  θ₀ (radianes)   = {x0_ajustado:.4f} ± {errores[1]:.4f} rad\n\n")
            f.write(f"R² = {r_cuadrado:.4f}\n")
        
        print(f"Resultados guardados en '{ruta_resultados}'")
        
    except Exception as e:
        print(f"Error durante el ajuste: {e}")

def guardar_datos():
    """Función para guardar datos y cerrar conexión"""
    print("\n" + "="*60)
    print("MEDICIÓN COMPLETA - Guardando datos...")
    print("="*60)
    
    # Guardar datos en CSV primero
    try:
        ruta_csv = os.path.join(CARPETA_RESULTADOS, 'lux_data.csv')
        with open(ruta_csv, 'w', newline='', encoding='utf-8') as archivo_csv:
            csvwriter = csv.writer(archivo_csv)
            csvwriter.writerow(['Ángulo (°)', 'Iluminancia (lux)'])
            for a, l in zip(angulos, lux):
                csvwriter.writerow([a, l])
        print(f"✓ Datos guardados en '{ruta_csv}' ({len(angulos)} puntos)")
    except Exception as e:
        print(f"Error al guardar CSV: {e}")
    
    # Guardar la figura actual
    try:
        ruta_grafica = os.path.join(CARPETA_RESULTADOS, 'lux_vs_angle.png')
        fig.savefig(ruta_grafica, dpi=300, bbox_inches='tight')
        print(f"✓ Gráfica cartesiana guardada en '{ruta_grafica}'")
    except Exception as e:
        print(f"Error al guardar gráfica: {e}")
    
    # Cerrar conexión serial al final
    try:
        if ser.is_open:
            ser.close()
        print("✓ Conexión serial cerrada")
    except Exception as e:
        print(f"Error al cerrar serial: {e}")
    
    print("\n" + "="*60)
    print("Cierre esta ventana para continuar con el análisis...")
    print("="*60)

# Función para ejecutar después de cerrar la ventana
def procesar_despues_de_cerrar(event):
    """Se ejecuta cuando se cierra la ventana de tiempo real"""
    if len(angulos) > 0 and len(lux) > 0:
        # Asegurar que los datos están guardados
        if not os.path.exists(os.path.join(CARPETA_RESULTADOS, 'lux_data.csv')):
            guardar_datos()
        
        print("\n" + "="*60)
        print("INICIANDO ANÁLISIS ADICIONAL")
        print("="*60)
        graficar_polar()
        realizar_ajuste()
        print("\n" + "="*60)
        print("ANÁLISIS COMPLETADO")
        print("="*60)
        print("\nArchivos generados en la carpeta 'resultados':")
        print("  • lux_data.csv")
        print("  • lux_vs_angle.png")
        print("  • grafica_polar.png")
        print("  • ajuste_coseno.png")
        print("  • resultados_ajuste.txt")

# Iniciar la gráfica
fig, ax = plt.subplots(figsize=(10, 6))
fig.canvas.mpl_connect('close_event', procesar_despues_de_cerrar)  # Conectar evento de cierre
ani = animation.FuncAnimation(fig, actualizar_grafica, interval=100, cache_frame_data=False)
plt.show()

