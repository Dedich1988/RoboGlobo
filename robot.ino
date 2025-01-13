#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Инициализация PCA9685
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Пределы для сервоприводов
#define SERVOMIN 100 // Минимальный сигнал (0 градусов)
#define SERVOMAX 600 // Максимальный сигнал (180 градусов)

// Каналы сервоприводов
const int headServo = 0; // Канал для головы
const int jawServo = 1;  // Канал для челюсти

// Начальные значения углов
int currentHeadAngle = 50; // Начальное положение головы
int currentJawAngle = 50;   // Начальное положение челюсти

void setup() {
  Serial.begin(115200);
  
  // Инициализация PCA9685
  pwm.begin();
  pwm.setPWMFreq(60); // Частота 60 Гц

  // Установка начальных позиций
  moveServo(headServo, currentHeadAngle); // Установите голову в начальное положение
  moveServo(jawServo, currentJawAngle);   // Установите челюсть в начальное положение

  Serial.println("Сервоприводы готовы к работе.");
}

void loop() {
  if (Serial.available() > 0) { // Проверяем, есть ли данные в последовательном мониторе
    String command = Serial.readStringUntil('\n'); // Читаем команду до конца строки

    if (command.startsWith("dens ")) { // Проверяем команду dens
      handleDensCommand(command);
    } else if (command == "H") { // Управление головой
      moveHead();
    } else if (command == "J") { // Управление челюстью
      moveJaw();
    } else if (command == "O") { // Открыть челюсть до 70 градусов
      moveJawTo(111);
    } else if (command == "C") { // Закрыть челюсть до 55 градусов
      moveJawTo(55);
    } else if (command == "S") { // Имитация речи
      speak("Hello World");
    } else {
      Serial.println("Ошибка: недопустимая команда. Используйте 'H', 'J', 'O', 'C', 'S' или 'dens'.");
    }
  }
}

// Функция для перемещения челюсти в заданный угол
void moveJawTo(int targetAngle) {
   if (targetAngle >= 49 && targetAngle <= 111) { 
      currentJawAngle = targetAngle;
      moveServo(jawServo, currentJawAngle);
      Serial.print("Челюсть перемещена в ");
      Serial.print(currentJawAngle);
      Serial.println(" градусов");
   } else {
      Serial.println("Ошибка: угол вне допустимого диапазона (0-111 градусов).");
   }
}

// Обработка команды dens
void handleDensCommand(String command) {
  int angles[3];
  
  // Извлекаем углы из команды
  int startIndex = command.indexOf(' ') + 1;
  int endIndex = command.indexOf(' ', startIndex);
  
  for (int i = 0; i < 3; i++) {
    if (endIndex == -1) endIndex = command.length(); // Если это последний угол
    angles[i] = command.substring(startIndex, endIndex).toInt();
    startIndex = endIndex + 1;
    endIndex = command.indexOf(' ', startIndex);
    
    if (i == 2 && startIndex < command.length()) {
      angles[i] = command.substring(startIndex).toInt(); // Последний угол
    }
  }

  // Двигаем голову в указанные позиции с разной скоростью
  for (int i = 0; i < 2; i++) { // Два раза медленно
    for (int angle : angles) {
      moveToAngle(angle, true); // true для медленного движения
    }
  }

  for (int i = 0; i < 2; i++) { // Два раза чуть быстрее
    for (int angle : angles) {
      moveToAngle(angle, false); // false для быстрого движения
    }
  }
}

void moveToAngle(int targetAngle, bool slow) {
   int stepDelay = slow ? 50 : 20; // Задержка между шагами

   while (currentHeadAngle != targetAngle) {
     if (currentHeadAngle < targetAngle) {
       currentHeadAngle++;
     } else {
       currentHeadAngle--;
     }
     
     moveServo(headServo, currentHeadAngle);
     delay(stepDelay);
     
     Serial.print("Голова перемещена в ");
     Serial.print(currentHeadAngle);
     Serial.println(" градусов");
   }
}

void moveHead() {
   Serial.print("Введите угол для головы (-5 или +5): ");
  
   while (!Serial.available()); // Ожидаем ввода данных
   int step = Serial.parseInt(); // Читаем шаг от пользователя

   if (step == -5 && currentHeadAngle >= 5) {
     currentHeadAngle -= 5; // Уменьшаем угол на -5 градусов
     moveServo(headServo, currentHeadAngle);
     Serial.print("Голова перемещена в ");
     Serial.print(currentHeadAngle);
     Serial.println(" градусов");
   } else if (step == +5 && currentHeadAngle <=175) {
     currentHeadAngle +=5; // Увеличиваем угол на +5 градусов
     moveServo(headServo, currentHeadAngle);
     Serial.print("Голова перемещена в ");
     Serial.print(currentHeadAngle);
     Serial.println(" градусов");
   } else {
     Serial.println("Ошибка: угол вне допустимого диапазона (0-180 градусов).");
   }
}

void moveJaw() {
   Serial.print("Введите угол для челюсти (-5 или +5): ");
  
   while (!Serial.available()); // Ожидаем ввода данных
   int step = Serial.parseInt(); // Читаем шаг от пользователя

   if (step == -5 && currentJawAngle >=5) {
     currentJawAngle -=5; // Уменьшаем угол на -5 градусов
     moveServo(jawServo, currentJawAngle);
     Serial.print("Челюсть закрыта до ");
     Serial.print(currentJawAngle);
     Serial.println(" градусов");
   } else if (step == +5 && currentJawAngle <=80) { // Изменено на максимальное значение челюсти
     currentJawAngle +=5; // Увеличиваем угол на +5 градусов
     moveServo(jawServo, currentJawAngle);
     Serial.print("Челюсть открыта до ");
     Serial.print(currentJawAngle);
     Serial.println(" градусов");
   } else {
     Serial.println("Ошибка: угол вне допустимого диапазона (0-80 градусов).");
   }
}

// Функция для чтения текущего положения челюсти
void readJawPosition() {
   Serial.print("Текущее положение челюсти: ");
   Serial.print(currentJawAngle);
   Serial.println(" градусов");
}

// Функция для имитации речи с движением челюсти
void speak(String phrase) {
   Serial.println(phrase);

   for (int i = 0; i < 3; i++) { // Повторяем три раза для имитации речи
       moveJawTo(55); // Открываем челюсть до 55 градусов
       delay(500);    // Ждем полсекунды (имитация произнесения)
       moveJawTo(70); // Открываем челюсть до 70 градусов (больше открытие)
       delay(500);    // Ждем полсекунды (имитация произнесения)
       
       moveJawTo(55); // Закрываем челюсть до 55 градусов после "говорения"
       delay(500);    // Ждем полсекунды перед следующим повтором
   }

   for (int j = 0; j < 2; j++) { 
       moveJawTo(70); 
       delay(300); 
       moveJawTo(50); 
       delay(300); 
       moveJawTo(70); 
       delay(300); 
       moveJawTo(50); 
       delay(300); 
   }
}

// Функция для движения сервопривода
void moveServo(int servo, int angle) {
   int pulseLength = map(angle,0,180,SERVOMIN,SERVOMAX);
   pwm.setPWM(servo,0,pulseLength);
}
