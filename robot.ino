#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

/* ================== КОНФИГУРАЦИЯ ОБОРУДОВАНИЯ ================== */
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Параметры сервоприводов
#define SERVOMIN 100   // 0° (минимальный импульс)
#define SERVOMAX 600   // 180° (максимальный импульс)
const int headServo = 0; // Канал головы
const int jawServo = 1;  // Канал челюсти

// Текущие положения
int currentHeadAngle = 50; // Диапазон 0-180°
int currentJawAngle = 50;  // Диапазон 0-80°

/* ================== СИСТЕМНЫЕ ФУНКЦИИ ================== */
void setup() {
  Serial.begin(115200);
  pwm.begin();
  pwm.setPWMFreq(60);
  
  // Инициализация сервоприводов
  moveServo(headServo, currentHeadAngle);
  moveServo(jawServo, currentJawAngle);

  // Вывод инструкций
  printHelp();
}

void loop() {
  handleSerialInput();
}

/* ================== ФУНКЦИИ УПРАВЛЕНИЯ ================== */
// Основная функция обработки команд
void handleSerialInput() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("dens ")) {
      handleDensCommand(command);
    } 
    else switch(command[0]) {
      case 'H': moveHeadInteractive(); break;
      case 'J': moveJawInteractive(); break; // Исправлено на moveJawInteractive
      case 'O': moveJawTo(70); break;
      case 'C': moveJawTo(55); break;
      case 'S': speak("Тревога! Обнаружена проблема."); break; // Исправлено на speak
      case '?': printHelp(); break;
      default: 
        Serial.println("ERROR: Unknown command");
        printHelp();
    }
  }
}

// Плавное движение головой по заданным углам
void handleDensCommand(String cmd) {
  int angles[3];
  parseAngles(cmd, angles);
  
  // Медленные циклы
  for(int i=0; i<2; i++) {
    for(int angle : angles) {
      smoothMove(headServo, angle, true); // Добавлено true для медленного движения
    }
  }
  
  // Быстрые циклы
  for(int i=0; i<2; i++) {
    for(int angle : angles) {
      smoothMove(headServo, angle, false); // Добавлено false для быстрого движения
    }
  }
}

/* ================== ФУНКЦИИ ДВИЖЕНИЯ ================== */
// Универсальная функция управления сервой
void moveServo(int servo, int angle) {
  int pulse = map(constrain(angle, 0, 180), 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(servo, 0, pulse);
}

// Плавное движение с заданной скоростью
void smoothMove(int servo, int targetAngle, bool slow) {
   int stepDelay = slow ? 50 : 20; // Задержка между шагами

   while (currentHeadAngle != targetAngle) {
     if (currentHeadAngle < targetAngle) {
       currentHeadAngle++;
     } else {
       currentHeadAngle--;
     }
     
     moveServo(servo, currentHeadAngle);
     delay(stepDelay);
     
     Serial.print("Голова перемещена в ");
     Serial.print(currentHeadAngle);
     Serial.println(" градусов");
   }
}

// Управление головой через терминал
void moveHeadInteractive() {
  Serial.print("Введите угол для головы (-5/+5): ");
  
  while (!Serial.available()); // Ожидаем ввода данных
  int step = Serial.parseInt(); // Читаем шаг от пользователя

  if (validateAngle(currentHeadAngle + step, 0, 180)) {
    currentHeadAngle += step;
    moveServo(headServo, currentHeadAngle);
    printPosition("Голова", currentHeadAngle);
  }
}

// Аналогичная функция для челюсти
void moveJawInteractive() {
   Serial.print("Введите угол для челюсти (-5/+5): ");
  
   while (!Serial.available()); // Ожидаем ввода данных
   int step = Serial.parseInt(); // Читаем шаг от пользователя

   if (validateAngle(currentJawAngle + step, 0, 80)) { 
     currentJawAngle += step; 
     moveServo(jawServo, currentJawAngle);
     printPosition("Челюсть", currentJawAngle);
   } else {
     Serial.println("Ошибка: угол вне допустимого диапазона (0-80 градусов).");
   }
}

// Валидация углов
bool validateAngle(int angle, int minVal, int maxVal) {
   return (angle >= minVal && angle <= maxVal);
}

// Вывод текущего положения сервопривода
void printPosition(String name, int angle) {
   Serial.print(name);
   Serial.print(" перемещена в ");
   Serial.print(angle);
   Serial.println(" градусов");
}

// Вывод справочной информации
void printHelp() {
   Serial.println("\n=== ROBOT CONTROL SYSTEM ===");
   Serial.println("H - Управление головой");
   Serial.println("J - Управление челюстью");
   Serial.println("O - Открыть челюсть (70°)");
   Serial.println("C - Закрыть челюсть (55°)");
   Serial.println("S - Сигнал тревоги");
   Serial.println("dens X Y Z - Программа движения");
   Serial.println("? - Эта справка");
}

// Парсинг углов для команды dens
void parseAngles(String cmd, int* angles) {
   int startIndex = cmd.indexOf(' ') + 1;
   for (int i = 0; i < 3; i++) {
       int endIndex = cmd.indexOf(' ', startIndex);
       if (endIndex == -1) endIndex = cmd.length();
       angles[i] = cmd.substring(startIndex, endIndex).toInt();
       startIndex = endIndex + 1;
       if (startIndex >= cmd.length()) break;
   }
}

// Функция для имитации речи с движением челюсти
void speak(String phrase) {
   Serial.println(phrase);

   for (int i = 0; i < 3; i++) { 
       moveJawTo(55); 
       delay(500);    
       moveJawTo(70); 
       delay(500);    
       
       moveJawTo(55); 
       delay(500);    
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

// Функция для перемещения челюсти в заданный угол
void moveJawTo(int targetAngle) {
   if (validateAngle(targetAngle, 0, 80)) { 
      currentJawAngle = targetAngle;
      moveServo(jawServo, currentJawAngle);
      Serial.print("Челюсть перемещена в ");
      Serial.print(currentJawAngle);
      Serial.println(" градусов");
   } else {
      Serial.println("Ошибка: угол вне допустимого диапазона (0-80 градусов).");
   }
}
