# Tarea 2: 
**Asignatura:** IPD439 - Seminario Avanzado de Computadores  
**Institución:** Universidad Técnica Federico Santa María  
**Alumno:** Cristián Ayancán  
**Plataforma:** STM32L476RG (Nucleo-64)

---

## Descripción del Proyecto

Este proyecto tiene como objetivo estudiar el comportamiento de sistemas embebidos basados en **FreeRTOS**, analizando la ejecución concurrente de tareas, su planificación (*scheduling*) y los mecanismos de comunicación entre ellas.

En la **Pregunta 1**, se implementan tareas periódicas utilizando distintos métodos de temporización, evaluando su exactitud, precisión y el efecto del jitter. Además, se introduce una tarea de alta prioridad con carga computacional variable para analizar cómo afecta al desempeño temporal del sistema.

En la **Pregunta 2**, se desarrolla un sistema más complejo basado en múltiples tareas y colas, el cual incluye adquisición de datos mediante ADC, procesamiento de la información y control de salidas (LEDs). Adicionalmente, se implementa comunicación UART para el envío y validación de datos mediante un script en Python.

---

## Estructura del Repositorio

### 📁 Pregunta_1/
Contiene la implementación de tareas periódicas en FreeRTOS:

- Generación de señales periódicas mediante GPIO.
- Uso de `osDelay()` (retardo relativo) y `osDelayUntil()` (retardo absoluto).
- Medición de período, ancho de pulso y jitter utilizando analizador lógico.
- Implementación de una tercera tarea con mayor prioridad para analizar el efecto de la carga computacional en el sistema.
- Evaluación del impacto en la planificación y pérdida de periodicidad.

---

### 📁 Pregunta_2/
Contiene la implementación de un sistema concurrente con comunicación entre tareas:

- **Subsistema de comunicación UART:**
  - Recepción de datos desde un script en Python.
  - Uso de colas para almacenamiento de datos.
  - Cálculo de promedio y desviación estándar.

- **Subsistema de adquisición y control:**
  - Lectura periódica del ADC.
  - Procesamiento de datos y decisión de control.
  - Activación de LEDs según rangos definidos.

- Uso de múltiples tareas (`AdqTask`, `ProcTask`, `CtrlTask`) con distintas prioridades.
- Implementación de colas (`QueueADCHandle`, `QueueLEDHandle`) para desacoplar las etapas del sistema.

En esta carpeta también se incluye el script en Python utilizado para enviar 100 muestras al sistema mediante UART.

---

## Instrucciones para Replicar el Proyecto

### 🔹 Clonar repositorio
```bash
1. git clone https://github.com/cris0d/IPD-439-Tarea-1.git
2. Abrir STM32CubeIDE y seleccionar un nuevo Workspace.
3. Ir a `File > Import... > General > Existing Projects into Workspace`.
4. Seleccionar la carpeta `Pregunta_1` o `Pregunta_2`.
