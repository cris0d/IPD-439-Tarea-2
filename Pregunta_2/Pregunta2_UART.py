import serial
import struct
import time
import random

# CONFIGURACIÓN: Ajusta el puerto COM 
PUERTO = 'COM4' 
BAUDIOS = 115200

try:
    ser = serial.Serial(PUERTO, BAUDIOS, timeout=3)

    # 1. Generar 100 valores reales aleatorios
    datos_enviar = [random.uniform(0.0, 10.0) for _ in range(100)]
    
    # Calcular valores esperados
    esperado_prom = sum(datos_enviar) / 100
    esperado_desv = (sum((x - esperado_prom)**2 for x in datos_enviar) / 100)**0.5

    print(f"Enviando los 100 valores al STM32...")
    
    # Envio de datos UART: Cada float se empaqueta en 4 bytes 
    for valor in datos_enviar:
        paquete = struct.pack('<f', valor)
        ser.write(paquete)
        
    print("Datos enviados. Esperando respuesta (8 bytes)...")

    # 2. Leer la respuesta (2 floats de 4 bytes cada uno)
    respuesta_binaria = ser.read(8)
    
    if len(respuesta_binaria) == 8:
        # Desempaquetamos: '<' (Little Endian), 'ff' (dos floats)
        stm32_prom, stm32_desv = struct.unpack('<ff', respuesta_binaria)
        
        # Calcular diferencias entre PC y STM32
        diff_prom = abs(esperado_prom - stm32_prom)
        diff_desv = abs(esperado_desv - stm32_desv)

        print("\n" + "="*45)
        print("         RESULTADOS PROCESADOS EN PC")
        print("="*45)
        print(f"{'Métrica':<15} | {'STM32':<12} | {'PC (Ref)':<12}")
        print("-" * 45)
        print(f"{'Promedio':<15} | {stm32_prom:<12.4f} | {esperado_prom:<12.4f}")
        print(f"{'Desv. Est.':<15} | {stm32_desv:<12.4f} | {esperado_desv:<12.4f}")
        print("-" * 45)
        print(f"Diferencia Promedio:  {diff_prom:.2e}")
        print(f"Diferencia Desv. Est: {diff_desv:.2e}")
        print("="*45)
        
        if diff_prom < 1e-4 and diff_desv < 1e-4:
            print(">>> VALIDACIÓN EXITOSA: Los cálculos coinciden.")
        else:
            print(">>> ADVERTENCIA: Hay discrepancias en los cálculos.")
            
    else:
        print(f"Error: Se esperaban 8 bytes, pero se recibieron {len(respuesta_binaria)}.")
        print("Contenido recibido (hex):", respuesta_binaria.hex())

    ser.close()

except Exception as e:
    print(f"Error crítico: {e}")