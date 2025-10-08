#include <Arduino.h>
#include "BraccioV2.h"

Braccio arm;

#define PINZA_CERRADA 85
#define PINZA_ABIERTA 20

String comando = ""; // Guarda el texto recibido por el puerto serial

// Límites de movimiento por eje
int ejeMin[6] = {0, 0, 0, 0, 0, 15};
int ejeMax[6] = {180, 180, 180, 180, 180, 100};
int posicionActual[6] = {90, 90, 90, 90, 90, 50};
int offset[6] = {0,80 ,90 , 90, 90, 50};

void Comando(String cmd);
void Pinza(int estado);
void MoverEje(int eje, int angulo);
void getPosicion();
void setup()
{
  Serial.begin(9600);
  arm.begin(true);

  // Configurar límites de la pinza
  for (int i = 0; i < 6; i++){
  arm.setJointCenter(i, offset[i]);
  arm.setJointMax(i, ejeMax[i]);
  arm.setJointMin(i, ejeMin[i]);
  
}
  Serial.println("Listo para recibir comandos (#p0*, #p1*, #m0-90*, etc.)");
}

void loop()
{
  while (Serial.available())
  {
    char c = Serial.read();
    Serial.print(c);

    if (c == '*')
    {
      // Solo procesar si el comando empieza con '#'
      if (comando.startsWith("#"))
      {
        Comando(comando);
      }
      else
      {
        Serial.println(" -> Comando erróneo (debe comenzar con #)");
      }
      comando = ""; // limpiar el buffer
    }
    else
    {
      comando += c;
    }
  }
}

void Comando(String cmd)
{
  cmd.trim();

  // Verificar formato
  if (!cmd.startsWith("#"))
  {
    Serial.println(" -> Comando erróneo (debe comenzar con #)");
    return;
  }

  // Quitamos el '#'
  cmd = cmd.substring(1);

  // --- COMANDO DE PINZA ---
  if (cmd.startsWith("p"))
  {
    char estadoChar = cmd.charAt(1);
    int estado = estadoChar - '0';
    if (estado == 0 || estado == 1)
    {
      Pinza(estado);
    }
    else
    {
      Serial.println(" -> Comando erróneo (pinza debe ser 0 o 1)");
    }

    // --- COMANDO DE MOVER EJE ---
  }
  else if (cmd.startsWith("m"))
  {
    // Formato esperado: m<eje>-<angulo>
    int separador = cmd.indexOf('-');
    if (separador > 1)
    {
      int eje = cmd.substring(1, separador).toInt();
      int angulo = cmd.substring(separador + 1).toInt();
      MoverEje(eje, angulo);
    }
    else
    {
      Serial.println(" -> Comando erróneo (usa formato #m0-90*)");
    }
  }
  else if (cmd.startsWith("g"))
  {
    getPosicion();
  }

  else
  {
    Serial.println(" -> Comando erróneo");
  }
}

// --- FUNCIONES DE MOVIMIENTO ---

void Pinza(int estado)
{
  if (estado == 0)
  {
    arm.setOneAbsolute(pinza, PINZA_CERRADA);
    Serial.println(" -> Pinza CERRADA");
  }
  else
  {
    arm.setOneAbsolute(pinza, PINZA_ABIERTA);
    Serial.println(" -> Pinza ABIERTA");
  }

  arm.update();
  arm.safeDelay(500);
}

void MoverEje(int eje, int angulo)
{
  if (eje < 0 || eje > 5)
  {
    Serial.println(" -> Eje inválido (usa 0 a 5)");
    return;
  }

  angulo = constrain(angulo, ejeMin[eje], ejeMax[eje]);

  arm.setOneAbsolute(eje, angulo+offset[eje]);
  arm.update();
  arm.safeDelay(4000);
  posicionActual[eje] = angulo;
  Serial.print(" -> Eje ");
  Serial.print(eje);
  Serial.print(" movido a ");
  Serial.println(angulo);
}
void getPosicion()
{
  Serial.println("\n--- Posición actual de los ejes ---");
  for (int i = 0; i < 6; i++)
  {
    Serial.print("Eje ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(posicionActual[i]);
    Serial.println("°");
  }
  Serial.println("-------------------------------\n");
}