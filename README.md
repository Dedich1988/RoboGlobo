# RoboGlobo
Описание функций
cpp
// Инициализация PCA9685
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
PCA9685 — это специальный чип, который помогает управлять сервоприводами. Мы создаем объект pwm, чтобы использовать его в нашем коде.
cpp
void setup() {
setup() — это функция, которая выполняется один раз, когда мы включаем устройство. Здесь мы настраиваем все необходимые вещи.
cpp
  Serial.begin(115200);
Serial.begin(115200) — эта строка запускает последовательный монитор, чтобы мы могли видеть сообщения от робота на компьютере.
cpp
  pwm.begin();
  pwm.setPWMFreq(60);
pwm.begin() — инициализирует чип PCA9685.
pwm.setPWMFreq(60) — устанавливает частоту работы сервоприводов на 60 Гц.
cpp
void loop() {
loop() — это функция, которая выполняется постоянно после setup(). Здесь происходит основная работа нашего робота.
cpp
if (Serial.available() > 0) {
Проверяет, есть ли данные, которые мы ввели в последовательном мониторе.
cpp
String command = Serial.readStringUntil('\n');
Считывает введенную команду до конца строки (до нажатия Enter).
cpp
void moveJawTo(int targetAngle) {
moveJawTo(int targetAngle) — эта функция перемещает челюсть в заданный угол. Если угол находится в допустимом диапазоне (от 0 до 80 градусов), она перемещает челюсть и обновляет текущее значение угла.
cpp
void handleDensCommand(String command) {
handleDensCommand(String command) — эта функция обрабатывает команду dens, извлекая три угла из команды и перемещая голову в эти углы с разной скоростью.
cpp
void moveToAngle(int targetAngle, bool slow) {
moveToAngle(int targetAngle, bool slow) — эта функция медленно или быстро перемещает голову в заданный угол. Она использует задержку между шагами для плавного движения.
cpp
void moveHead() {
moveHead() — эта функция позволяет пользователю управлять головой робота, увеличивая или уменьшая угол на 5 градусов.
cpp
void moveJaw() {
moveJaw() — эта функция позволяет пользователю управлять челюстью робота, увеличивая или уменьшая угол на 5 градусов.
cpp
void readJawPosition() {
readJawPosition() — эта функция выводит текущее положение челюсти на последовательный монитор.
cpp
void speak(String phrase) {
speak(String phrase) — эта функция имитирует речь, открывая и закрывая челюсть несколько раз при произнесении заданной фразы.
cpp
void moveServo(int servo, int angle) {
moveServo(int servo, int angle) — эта функция отправляет сигнал на сервопривод для перемещения его в заданный угол.
