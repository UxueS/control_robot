#include <Arduino.h>
#include "BraccioV2.h"

Braccio arm;

#define PINZA_CERRADA 85
#define PINZA_ABIERTA 20

String comando = ""; // Para guardar lo que el usuario escribe por el terminal serial

// Declaramos las funciones
void Comando(String cmd);
void Pinza(int estado);

void setup() {
  Serial.begin(9600);
  arm.begin(true);

  arm.setJointCenter(pinza, 50);
  arm.setJointMax(pinza, 100);
  arm.setJointMin(pinza, 15);

  Serial.println("Listo para recibir comandos (#p0* o #p1*)");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    Serial.print(c);

    if (c == '*') {
      // Solo procesamos si el comando empieza con '#'
      if (comando.startsWith("#")) {
        Comando(comando);
      } else {
        Serial.println(" -> Comando erróneo (debe comenzar con #)");
      }
      comando = ""; // Reiniciamos el buffer del comando
    } else {
      comando += c;
    }
  }
}

void Comando(String cmd) {
  cmd.trim();

  // Verificar formato: debe comenzar con '#' y terminar con '*'
  if (!cmd.startsWith("#")) {
    Serial.println(" -> Comando erróneo (debe comenzar con #)");
    return;
  }

  // Eliminamos el '#' para detectar el comando
  cmd = cmd.substring(1);

  if (cmd.startsWith("p")) {
    char estadoChar = cmd.charAt(1);
    int estado = estadoChar - '0';
    if (estado == 0 || estado == 1) {
      Pinza(estado);
      Serial.print(" -> Pinza ");
      Serial.println(estado == 0 ? "CERRADA" : "ABIERTA");
    } else {
      Serial.println(" -> Comando erróneo");
    }
  } else {
    Serial.println(" -> Comando erróneo");
  }
}

void Pinza(int estado) {
  if (estado == 0) {
    arm.setOneAbsolute(pinza, PINZA_CERRADA);
  } else {
    arm.setOneAbsolute(pinza, PINZA_ABIERTA);
  }

  // Permitir movimiento físico de la pinza
  arm.update();
  arm.safeDelay(500);
}
