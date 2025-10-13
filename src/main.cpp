#include <Arduino.h>
#include "BraccioV2.h"

Braccio arm;

#define PINZA_CERRADA 85
#define PINZA_ABIERTA 20

String comando = ""; 


int ejeMin[6] = {0, 0, 0, 0, 0, 15};
int ejeMax[6] = {180, 180, 180, 180, 180, 100};
int posicionActual[6] = {90, 90, 90, 90, 90, 50};
int offset[6] = {0, 81, 90, 90, 90, 50};
int signo[6] = {-1, -1, -1, -1, -1, 1};
void Comando(String cmd);
void Pinza(int estado);
void MoverEje(int eje, int angulo);
void MoverTodosLosEjes(int angulos[6]);
void getPosicion();

void setup()
{
  Serial.begin(9600);
  
  for (int i = 0; i < 6; i++) {
    arm.setJointCenter(i, offset[i]);
    arm.setJointMax(i, ejeMax[i]);
    arm.setJointMin(i, ejeMin[i]);
    arm.setDelta(i, 1);
  }
  arm.begin(true);

  Serial.println("Listo para recibir comandos (#p0*, #p1*, #m0-90*, #a90-80-70-100-90-50*, etc.)");
}

void loop()
{
  while (Serial.available())
  {
    char c = Serial.read();
    Serial.print(c);

    if (c == '*')
    {
     int indexInicio = comando.indexOf('#');
      if (indexInicio>=0)
      {
        Comando(comando.substring(indexInicio));
      }
      else
      {
        Serial.println(" -> Comando erróneo (debe comenzar con #)");
      }
      comando = ""; 
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

  
  if (!cmd.startsWith("#"))
  {
    Serial.println(" -> Comando erróneo (debe comenzar con #)");
    return;
  }

  
  cmd = cmd.substring(1);

  
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
  }

  
  else if (cmd.startsWith("m"))
  {
    
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

  
  else if (cmd.startsWith("a"))
  {
    
    cmd = cmd.substring(1); 

    int angulos[6];
    int lastIndex = 0;
    for (int i = 0; i < 6; i++)
    {
      int nextIndex = cmd.indexOf('-', lastIndex);
      if (nextIndex == -1 && i < 5)
      {
        Serial.println(" -> Comando erróneo (usa formato #a90-100-80-120-90-40*)");
        return;
      }

      String valor = (nextIndex == -1) ? cmd.substring(lastIndex) : cmd.substring(lastIndex, nextIndex);
      angulos[i] = valor.toInt();
      lastIndex = nextIndex + 1;
    }

    MoverTodosLosEjes(angulos);
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

  arm.setOneAbsolute(eje, angulo*signo[eje] + offset[eje]);
  arm.update();
  arm.safeDelay(4000, 20);
  posicionActual[eje] = angulo;
  Serial.print(" -> Eje ");
  Serial.print(eje);
  Serial.print(" movido a ");
  Serial.println(angulo);
}

void MoverTodosLosEjes(int angulos[6])
{
  for (int i = 0; i < 6; i++)
  {
    angulos[i] = constrain(angulos[i], ejeMin[i], ejeMax[i]);
    arm.setOneAbsolute(i, signo[i]*angulos[i] + offset[i]);
    posicionActual[i] = angulos[i];
  }

  arm.update();
  arm.safeDelay(4000, 20);

  Serial.println(" -> Movimiento de todos los ejes");

}

void getPosicion()
{
    Serial.print("Posicion de los ejes:" );

      for (int i = 0; i < 6; i++)
  {
    Serial.print(" ");
    Serial.print(posicionActual[i]);
  }
    Serial.println("°");
}