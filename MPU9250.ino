// Librería para MPU9250 por asukia
#include <MPU9250_asukiaaa.h>
#include <Wire.h>

// Se crea una instancia del sensor
MPU9250_asukiaaa mpu;

// Declaración de variables
float angulo_de_salida = 45; // Relleno porque este valor viene de otro código
unsigned long tiempo_anterior = 0; // Variable para el tiempo anterior
float velocidad_inicial = 0; // Velocidad inicial
float velocidad_actual = 0; // Velocidad actual
float distancia_recorrida = 0.0; // Distancia recorrida

// Se inicia la comunicación con el sensor
void setup(void) {
  Serial.begin(115200);

  if (!mpu.begin()) {
    Serial.println("MPU9250 no respondió!");
    while (1);
  }
  Serial.println("MPU9250 está listo!");
}

void loop() {
  // Imprime la aceleración
  float aceleracion = mpu.getAccelX_mss();
  Serial.print("Aceleración: ");
  Serial.print(aceleracion);
  Serial.println("m/s^2");

  // Imprime velocidad
  velocidad_actual = velocidad();
  Serial.print("Velocidad: ");
  Serial.print(velocidad_actual);
  Serial.println("m/s");

  // Imprime distancia recorrida
  distancia_recorrida = distanciaRecorrida();
  Serial.print("Distancia recorrida: ");
  Serial.print(distancia_recorrida);
  Serial.println("m");

  float X_re = coordenadasActualesX();
  float Y_re = coordenadasActualesY();
  Serial.print("Coordenadas, X: ");
  Serial.print(X_re);
  Serial.print("m");
  Serial.print(", Y: ");
  Serial.print(Y_re);
  Serial.println("m");

  float ErrorA = alertaAceleracion();
  float ErrorD = alertaDesvio();
  Serial.println("La señal ""1"" notificará error");
  Serial.print("Alerta frenado brusco: ");
  Serial.print(ErrorA);
  Serial.print(" Alerta desvío: ");
  Serial.println(ErrorD);

  delay(500);
}

// Función que entrega la variable de velocidad actual
float velocidad() {
  float aceleracion = mpu.getAccelX_mss();

  // Obtén el tiempo actual
  unsigned long tiempo_actual = millis();

  // Calcula el cambio en el tiempo en segundos
  float delta_tiempo = (tiempo_actual - tiempo_anterior) / 1000.0;  // Convierte a segundos

  // Calcula el cambio en la velocidad usando la regla del trapecio
  float delta_velocidad = aceleracion * delta_tiempo;

  // Actualiza la velocidad actual
  velocidad_actual += delta_velocidad;

  // Actualiza el tiempo anterior para la próxima iteración
  tiempo_anterior = tiempo_actual;

  return velocidad_actual;
}

// Función para calcular la distancia recorrida a partir de la velocidad
float distanciaRecorrida() {
  // Obtén el tiempo actual
  unsigned long tiempo_actual = millis();

  // Calcula el cambio en el tiempo en segundos
  float delta_tiempo = (tiempo_actual - tiempo_anterior) / 1000.0;  // Convierte a segundos

  // Calcula el cambio en la distancia usando la regla del trapecio
  float delta_distancia = velocidad_actual * delta_tiempo;

  // Actualiza la distancia recorrida
  distancia_recorrida += delta_distancia;

  // Actualiza el tiempo anterior para la próxima iteración
  tiempo_anterior = tiempo_actual;

  return distancia_recorrida;
}

float coordenadasActualesY() {
  // Ángulo en radianes
  float anguloRadianes = radians(angulo_de_salida);

  // Calcula el cateto opuesto en función de la distancia recorrida
  float catetoOpuesto = distancia_recorrida * sin(anguloRadianes);

  return catetoOpuesto;
}

float coordenadasActualesX() {
  // Ángulo en radianes
  float anguloRadianes = radians(angulo_de_salida);

  // Calcula el cateto adyacente en función de la distancia recorrida
  float catetoAdyacente = distancia_recorrida * cos(anguloRadianes);

  return catetoAdyacente;
}

// Función de alerta por cambio brusco de aceleración (estrellarse)
int alertaAceleracion() {
  // Obtén la aceleración actual
  float aceleracion = mpu.getAccelX_mss();

  if (aceleracion < -15) {
    return 1;  // Alerta de frenado brusco
  } else {
    return 0;
  }
}

// Función de alerta por desvío de trayectoria
int alertaDesvio() {
  // Obtener los datos del sensor de giroscopio
  float girosX = mpu.getGyroX_rads();

  // Lógica para determinar el desvío y generar alerta
  if (girosX > 0.5 || girosX < -0.5) {
    return 1;  // Alerta de desvío
  } else {
    return 0;
  }
}

