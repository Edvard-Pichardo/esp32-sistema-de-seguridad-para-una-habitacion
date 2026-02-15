# Sistema de Seguridad Doméstico IoT con ESP32 y Telegram

Sistema de seguridad, monitoreo ambiental y control de iluminación basado en ESP32 con comunicación remota mediante bot privado de Telegram.

Proyecto desarrollado en el curso Introducción al Internet de las Cosas, Facultad de Ciencias, UNAM.

## Descripción

El sistema implementa una arquitectura IoT donde el ESP32 actúa como nodo central conectado a sensores ambientales y de seguridad. El usuario interactúa con el sistema en tiempo real mediante comandos enviados a un bot privado de Telegram.

El sistema permite activar alarmas, recibir notificaciones de intrusión, consultar variables ambientales y controlar iluminación de forma remota.

## Funcionalidades implementadas

- Detección de movimiento mediante sensor PIR

- Monitoreo de apertura de compartimento mediante sensor infrarrojo

- Monitoreo de temperatura y humedad con DHT11

- Control automático de iluminación basado en presencia y nivel de luz

- Control manual de iluminación vía Telegram

- Sistema de alarma sonora mediante buzzer

- Alertas automáticas por temperatura elevada

- Autenticación de usuario mediante chat ID

- Comunicación segura HTTPS con Telegram

## Lista de componentes 
- ESP32

- Sensor PIR (detección de movimiento)

- Sensor infrarrojo

- Sensor DHT11

- Fotoresistencia (LDR)

- 5 LEDs

- Buzzer

- Resistencias varias

## Comandos del bot

- Comando para iniciar el bot:

/start

- Comando para revisar la configuración general del sistema:

/status

- Comandos para activar/desactivar la alarma de seguridad de la habitación: 

/alarma_on

/alarma_off

- Comandos para activar/desactivar la alarma de seguridad de la caja fuerte:

/caja_fuerte_on

/caja_fuerte_off

- Comandos para encender/apagar las luces automaticas, en presencia de alguna persona, dentro de la habitación:

/luz_automatica_on

/luz_automatica_off

- Comandos para encender/apagar las luces de la habitación:

/luces_on

/luces_off

- Comando para medir la temperatura y humedad de la habitación:

/clima

## Arquitectura del sistema

El ESP32 adquiere datos de sensores y ejecuta lógica local de control. La comunicación con el usuario se realiza mediante la API de Telegram usando conexión WiFi segura.

## Seguridad

El acceso al sistema está restringido mediante validación del ID de usuario autorizado en Telegram.

## Posibles extensiones

- Interfaz web local

- Registro histórico de eventos

- Integración de cámara

- Automatización avanzada

## Notas finales
- Las credenciales deben configurarse manualmente antes de compilar.
