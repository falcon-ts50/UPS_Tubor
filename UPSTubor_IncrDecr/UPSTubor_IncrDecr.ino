//создаём макроопределения для используемых пинов
#define OUTPUT_SIGNAL     A0
#define INPUT_TEMP        A2
#define INPUT_SHUNT       A4
#define INPUT_SUPPORT     A6

//Характеристика датчика температуры ТМР-36
int millivoltAtZeroDegrees = 500;
int changingMillivoltPerOneDegrees = 10;
//точка калибровки. Для датчика ТМР-36 задаём как минус 40 Цельсиев
int tempCalibrationDeg = -40;

//Характеристика входов/выходов MKR Zero

//точность входного сигнала, мВ, при 10 битном АЦП при analogReference(AR_INTERNAL1V65);
// 1650 мВ/ 1024 = 1,611
double accuracyInput = 1.611;

//точность выходного сигнала, мВ, при 10 битном ЦАП
// 3300mV/1024 = 3.223 mV,
double accuracyOutput = 3.223;

//Температурно-вольтовая характеристика аккумулятора Тубор

//Режим Float
// задайте минимальную температуру в градусах Цельсия, ниже которой напряжение заряда не изменяется
int minTempFloatDeg = 0;
//задайте первую точку в градусах Цельсия, при которой не изменяется напряжение
int tempFirstMidPointFloat = 15;
//Задайте вторую точку при которой температура не изменяется
int tempSecondMidFloatDeg = 35;
//Задайте максимальную температуру в градусах Цельсия, после которой напряжение заряда не изменяется
int maxTempFloatDeg = 50;

//Режим Boost
// задайте минимальную температуру в градусах Цельсия, ниже которой напряжение заряда не изменяется
int minTempBoostDeg = 30;
// задайте максимальную температуру, после которой происходит переход к графику Float
int maxTempBoostDeg = 45;

//задайте максимальное значение выходного напряжения в милливольтах для Boost и Float
int outputMaximum = 2660;
// задайте уровень напряжения средних температурных точек в милливольтах для режима Float
int outputMiddleFloat = 2550;
//задайте значение минимального выходного напряжения в милливольтах для режима Float
int outputFloatMinimum = 2468;
//задайте значение минимального выходного напряжения в милливольтах для режима Boost
int outputBoostMinimum = 2550;

//задайте коэффициент преобразования для базовой шкалы "мВ на элемент" для получения калиброванного напряжения

double coefficientOfCalibration = 1.108;

//Характеристика шунта

//Задайте максимальное напряжение шунта {U шунт макс}
int maxVoltShunt = 60;

//Задайте максимальный ток шунта {I шунт макс}
int maxCurrentShunt = 400;

//Введите паспортную ёмкость для марки испольуемых аккумуляторов {C}
int capacitanceOfBattery = 75;

//Укажите число параллельных цепочек из аккумуляторов {N}
int numberBattery = 4;

//Введите значение коэффициента передачи аналогового предусилителя {K}
int coeffAnalogueAmplifier = 25;

//Задайте ток заряда {S}
double chargingCurrent = 2.8;

//Задайте предельный ток заряда {P}
double maxChargCurrent = 3.5;

//Задайте порог применения ускоренного заряда {B}
double thresholdForBoost = 0.9;

//Задайте условие окончания ускоренного заряда {R}
double thresholdBoostEnding = 0.25;

//Задайте максимальное время работы в режиме boost в часах
byte timeInBoost = 8;

//Задайте время задержки между повторным включением режима boost в минутах
byte delayBoost = 15;

// велечина напряжения сброса Ureset

int voltageReset = 575;

/******************************************************************************************************************************************************************/
//ВЫЧИСЛЯЕМЫЕ ЗНАЧЕНИЯ

//Напряжение соответствующее номинальному току заряда мВ Uномин

double valueOfNominalCurrentOnVoltage = 0.1 * capacitanceOfBattery * numberBattery * coeffAnalogueAmplifier * maxVoltShunt / maxCurrentShunt;

//Напряжение, соответствующее выбранному току заряда U заряда
int voltageOfCharge = valueOfNominalCurrentOnVoltage * chargingCurrent;

//Напряжение, соответствующее предельному току заряда U max (в мВ)
int limitVoltageOfCharge = valueOfNominalCurrentOnVoltage * maxChargCurrent;
//Umax в 10-битном исчислении
int limitVoltageChrgDAC = limitVoltageOfCharge/accuracyInput;

//Вычисляем значение Ustart при котором происходит начало ускоренного заряда (переключение с Float на Boost) Ustart

double switchFloatToBoost = valueOfNominalCurrentOnVoltage * thresholdForBoost;
//пересчёт в 10 бит
int switchFloatToBoostADC = switchFloatToBoost/accuracyInput;

//Вычисляем пороговое значение Ustop, при котором происходит переключения с функции Boost на Float (окончание ускоренного заряда) Ustop

double switchBoostToFloat = valueOfNominalCurrentOnVoltage * thresholdBoostEnding;
//пересчёт в 10 бит
int switchBoostToFloatADC = switchBoostToFloat/accuracyInput;

//режим работы
String mode;

//текущее событие
String event;

//таймер
String timerMode;

//Вычисляем температуры для 10-битного АЦП

//Вычисление минимальной температуры графика Float для 10-битного АЦП
int minTempFloatADC = (millivoltAtZeroDegrees + minTempFloatDeg*changingMillivoltPerOneDegrees) / accuracyInput;
// вычисляем первую среднюю точку для 10-битного АЦП
int tempFirstPointFloatADC = (millivoltAtZeroDegrees + tempFirstMidPointFloat*changingMillivoltPerOneDegrees) / accuracyInput;
//вычисляем вторую среднюю точку для 10-битного АЦП
int tempSecondPointFloatADC = (millivoltAtZeroDegrees + tempSecondMidFloatDeg*changingMillivoltPerOneDegrees) / accuracyInput;
//вычисляем максимальну температуру для 10-битного АЦП
int maxTempFloatADC = (millivoltAtZeroDegrees + maxTempFloatDeg*changingMillivoltPerOneDegrees) / accuracyInput;

//Вычисление минимальной температуры графика Boost для 10-битного АЦП
int minTempBoostADC = (millivoltAtZeroDegrees + minTempBoostDeg*changingMillivoltPerOneDegrees) / accuracyInput;
//Вычисление максимальной температуры графика Boost после которого происходит переход на Float для 10 битного АЦП
int maxTempBoostADC = (millivoltAtZeroDegrees + maxTempBoostDeg*changingMillivoltPerOneDegrees) / accuracyInput;

//Перевод температуры -40 в 10 бит
int minTempCalibr = (millivoltAtZeroDegrees - 40 * changingMillivoltPerOneDegrees) / accuracyInput;

//Вычисляем значения выходного напряжения управления для 10 битного ЦАП

//вычисляем значение выходного напряжения для 10-битного ЦАП, увеличиваем на единицу, т.к. в данной версии для Тубор это необходимо
//из-за грубого округления при отбрасывании десятых долей
int outputMaxDAC = outputMaximum * coefficientOfCalibration / accuracyOutput + 1;
//вычисляем значение выходного напряжение средних точек Float для 10-битного ЦАП
int outputMidFloatDAC = outputMiddleFloat * coefficientOfCalibration / accuracyOutput;
//вычисляем значение выходно напряжения макисмальной точки для Boost, но в данном случае оно равно значению средних точек Float
int outputMaxBoostDAC = outputMidFloatDAC;
//вычисляем значение выходного напряжения при максимальной температуре для 10-битного ЦАП
int outputMinDAC = outputFloatMinimum * coefficientOfCalibration / accuracyOutput;

//вычисляем точку калибровки для 10-битного АЦП
int tempCalibrationADC = (millivoltAtZeroDegrees + tempCalibrationDeg*changingMillivoltPerOneDegrees) / accuracyInput;

//Переменные времени

//время в миллисекундах в boost
unsigned long timeInBoostMillis = timeInBoost * 60 * 60 * 1000;
//время в миллисекундах задержка между boost и float
unsigned long delayBoostMillis = delayBoost * 60 * 1000;

//Перменные, расчитываемые при старте программы

//Созадём массив на 10 точек для скользящей средней по температуре без инициализации
int arrayTemp[10];
int arrayCurrent[10];
int arraySupport[20];

//Создаём переменные для выходного сигнала и средней температуры для их использования в функции отображения на экране.
int outputSignal;
int averageTemperature;
int shuntCurrent;
int valueOfCurrent;
int voltageSupport;
int voltageTemperature;

boolean isUstart = false;
boolean isLastSignalBoost = false;

//переменные для хранения времени таймеров
unsigned long timerFloatBoost;
unsigned long timerBoost;
unsigned long timerVoltageShunt;
unsigned long timerTemperature;
unsigned long timerVoltageSupport;
unsigned long timerComparator;
unsigned long timerDisplaying;


/****************************************************************************************************************************************************************/

void setup() {

  // put your setup code here, to run once:
  pinMode(INPUT_TEMP, INPUT);
  pinMode(INPUT_SHUNT, INPUT);
  pinMode(INPUT_SUPPORT, INPUT);
  //Устанавливаем разрешение для работы с входным сигналом
  analogReference(AR_INTERNAL1V65);

  analogWriteResolution(10);
  analogReadResolution(10);

  // инициализация массива Температуры (ниже блок 2) для скользящей средней
  for (int i = 0; i < 10; i++)
  {
    arrayTemp[i] = analogRead(INPUT_TEMP);
    delay(10);
  }

  // инициализация массива Шунта (ниже блок 1) для скользящей средней
  for (int j = 0; j < 10; j++)
  {
    arrayCurrent[j] = analogRead(INPUT_SHUNT);
    delay(10);
  }
  // инициализация массива Суппорта (ниже блок 3) для скользящей средней

  for (int k = 0; k < 20; k++)
  {
    arraySupport[k] = analogRead(INPUT_SUPPORT);
    delay(10);
  }

  // делаем первоначальную иницаиализацию выходного сигнала
  outputSignal = voltageReset;

  //инициализация таймеров
  timerFloatBoost = millis();
  timerBoost = millis();
  timerVoltageShunt = millis();
  timerTemperature = millis();
  timerVoltageSupport = millis();
  timerComparator = millis();
  timerDisplaying = millis();

  //открываем передачу данных для мониторинга
  Serial.begin(9600);
}

/*************************************************************************************************************************************************************************************/

void loop() {

  //БЛОК 1. получение данных по напряжению на шунте

  if (!isTimerWork(timerVoltageShunt, 15)) {
    valueOfCurrent = 2*getMovAverageCurrent(arrayCurrent);
    timerVoltageShunt = millis();
  }

  //БЛОК 2. получение данных по температуре
  if (!isTimerWork(timerTemperature, 1000)) {
    averageTemperature = getMovAverageTemp(arrayTemp);
    timerVoltageShunt = millis();
  }

  //БЛОК 3. получение данных по Usupport
  if (!isTimerWork(timerVoltageSupport, 15)) {
    voltageSupport = getMovAverageSupport(arraySupport);
    timerVoltageSupport = millis();
  }

  //БЛОК 4. Выбор графика boost/float
  //условие калибровки (если температура ниже минус 40, то включаем режим выходного сигнала, равный среднему значению при Флоат)
  if (!isTimerWork(timerComparator, 330)) {
    
      //4.2 проверка на 15-ти мнунтный таймер
      if (isTimerWork(timerFloatBoost, delayBoostMillis)) {
        mode = "4.2 Float";
        voltageTemperature = outputFloat(averageTemperature);
        timerMode = "15-ти минутный таймер работает";
        
      }
      //4.3 проверка на значение выше 45 градусов Цельсия
      else if (averageTemperature > maxTempBoostADC) {
        mode = "4.3 Float";
        voltageTemperature = outputFloat(averageTemperature);
        timerFloatBoost = millis();
        timerMode = "15-ти минутный таймер запущен";
        isUstart = false;
        isLastSignalBoost = false;
      }
      //4.4 проверка Ushunt <= Ustop
      else if (valueOfCurrent <= switchBoostToFloatADC) {
        if (isLastSignalBoost) {
          mode = "4.4 Float";
          voltageTemperature = outputFloat(averageTemperature);
          timerFloatBoost = millis();
          timerMode = "15-ти минутный таймер запущен";
          isUstart = false;
          isLastSignalBoost = false;
        }
        else {
          mode = "4.4 else Float";
          voltageTemperature = outputFloat(averageTemperature);
          timerMode = "таймеры не работают";
          isUstart = false;
          isLastSignalBoost = false;
        }
      }
      //4.5 проверка Ushunt >= Ustart
      else if (valueOfCurrent >= switchFloatToBoostADC) {
        mode = "4.5 Boost";
        voltageTemperature = outputBoost(averageTemperature);
        timerMode = "таймеры не работают";
        isUstart = true;
        isLastSignalBoost = true;
      }
      //4.6 проверка момента перехода Ustart в сторону Ustop
      else if (isUstart) {
        isUstart = false;
        mode = "4.6 Boost";
        timerBoost = millis();
        timerMode = "запущен 8-ми часовой таймер";
        voltageTemperature = outputBoost(averageTemperature);
        isLastSignalBoost = true;
      }
      //4.7 проверка на работу 8 часового таймера
      else if (isTimerWork(timerBoost, timeInBoostMillis)) {
        mode = "4.7 Boost";
        voltageTemperature = outputBoost(averageTemperature);
        timerMode = "8 часовой таймер работает";
        isLastSignalBoost = true;
      }
      //4.8 проверка последний сигнал был Boost?
      else if(isLastSignalBoost) {
         mode = "4.8 Float";
         voltageTemperature = outputFloat(averageTemperature);
         timerFloatBoost = millis();
         timerMode = "15-ти минутный таймер запущен";
         isLastSignalBoost = false;
      }
      //4.9 предохранительная ветка
      else {
      mode = "4.9 Float";
      voltageTemperature = outputFloat(averageTemperature);
      timerMode = "таймеры не работают";
      isLastSignalBoost = false;
      }
        
      //БЛОК 5. Блок сравнения
      //5.1 проверка на Ushunt > Umax
      if (valueOfCurrent > limitVoltageChrgDAC) {
        outputSignal = voltageReset;
        event = "5.1 Ushunt > Umax; Uout = Ureset";
      }
      //5.2 проверка на Uout>Ut
      else if (outputSignal > voltageTemperature) {
        outputSignal--;
        event = "5.2 Uout > Ut; Uout--";
      }
      //5.3 проверка на Uout=Ut
      else if (outputSignal = voltageTemperature) {
        outputSignal = voltageTemperature;
        event = "5.3 Uout = Ut";
      }
      //5.4 проверка на Ushunt>Usuppl
      else if (valueOfCurrent > voltageOfCharge) {
        //пустая операция
        event = "5.4 Ushunt > Usuppl; пустая операция";
      }
      //5.5 проверка на Usupport <= Ureset и проверка через либо 5.6 проверка на Uout >= Usupport
      else if (voltageSupport <= voltageReset || outputSignal >= voltageSupport) {
        outputSignal++;
        event = "5.5/5.6 Usupport <= Ureset или Uout >= Usupport; Uout ++ ";
      }
      //5.7 последнее иначе
      else {
        outputSignal = voltageSupport;
        event = "5.7 Uout = Usupport";
      }
    

    timerComparator = millis();

  }


  analogWrite (OUTPUT_SIGNAL, outputSignal);

  if (!isTimerWork(timerDisplaying, 5000)) {
    displayingDataTemp ();
    timerDisplaying = millis();
  }

}
/*********************************************************************************************************************************************************************************/

//вычисление выходного сигнала по графику Float

int outputFloat(int tempLevel) {
  int outputSignalFloat;

  if (tempLevel < minTempCalibr) {
    outputSignalFloat = outputMidFloatDAC;
    mode = "Calibration";
  }
  else if (tempLevel <= minTempFloatADC) {
    outputSignalFloat = outputMaxDAC;
  }
  else if (tempLevel > minTempFloatADC && tempLevel <= tempFirstPointFloatADC) {
    outputSignalFloat = map(tempLevel, minTempFloatADC, tempFirstPointFloatADC, outputMaxDAC, outputMidFloatDAC);
  }
  else if (tempLevel > tempFirstPointFloatADC && tempLevel <= tempSecondPointFloatADC) {
    outputSignalFloat = outputMidFloatDAC;
  }
  else if (tempLevel > tempSecondPointFloatADC && tempLevel <= maxTempFloatADC) {
    outputSignalFloat = map(tempLevel, tempSecondPointFloatADC, maxTempFloatADC, outputMidFloatDAC, outputMinDAC);
  }
  else if (tempLevel > maxTempFloatADC) {
    outputSignalFloat = outputMinDAC;
  }
  return outputSignalFloat;

}

//вычисление выходного сигнала по графику Boost

int outputBoost(int tempLevel) {
  int outputSignalBoost;
  if (tempLevel < minTempCalibr) {
    outputSignalBoost = outputMidFloatDAC;
    mode = "Calibration";
  }
  else if (tempLevel <= minTempBoostADC) {
    outputSignalBoost = outputMaxDAC;
  }
  else if (tempLevel > minTempBoostADC && tempLevel <= maxTempBoostADC) {
    outputSignalBoost = map (tempLevel, minTempBoostADC, maxTempBoostADC, outputMaxDAC, outputMaxBoostDAC);
  }
  else outputSignalBoost = outputFloat (tempLevel);

  return outputSignalBoost;
}

//вычисление скользящей средней на 10 точек для данных по температуре

int getMovAverageTemp (int arrayTemp[10]) {
  for (byte j = 0; j < 9; j++) {
    arrayTemp[j] = arrayTemp[j + 1];
  }

  arrayTemp[9] = analogRead(INPUT_TEMP);

  int sum = 0;
  for (byte i = 0; i < 10 ; i++) {
    sum += arrayTemp[i];
  }

  return sum / 10;
}

//вычисление скользящей средней на 10 точек для данных по току Шунта

int getMovAverageCurrent (int arrayCur[10]) {
  for (byte j = 0; j < 9; j++) {
    arrayCur[j] = arrayCur[j + 1];
  }

  arrayCur[9] = analogRead(INPUT_SHUNT);

  int sum = 0;
  for (byte i = 0; i < 10 ; i++) {
    sum += arrayCur[i];
  }

  return sum / 10;
}

// вычисление сокльзящей средней на 20 точек для Usupport
int getMovAverageSupport(int arraySup[20]) {
  for (byte j = 0; j < 19; j++) {
    arraySup[j] = arraySup[j + 1];
  }

  arraySup[19] = analogRead(INPUT_SUPPORT);

  int sum = 0;
  for (byte i = 0; i < 20 ; i++) {
    sum += arraySup[i];
  }

  return sum / 20;
}

//проверка сколько времени прошло с момента старта программы, с обработкой переполнения

boolean isTimerWork(unsigned long timeWork, unsigned long timeLimit) {

  if (millis() - timeWork < timeLimit) {
    return true;
  }
  else return false;
}



//функция показа данных в мониторе порта

void displayingDataTemp () {
  //6.1 10 битный блок
  Serial.print("Выходной сигнал в разрядности 10 бит: ");
  Serial.println(outputSignal);

  Serial.print("Температура в 10 битах: ");
  Serial.println(averageTemperature);

  Serial.print("Напряжение шунта в 10 битах: ");
  Serial.println(valueOfCurrent);

  Serial.print("Опорное напряжение в 10 битах: ");
  Serial.println(voltageSupport);
  Serial.println(" ");

  //6.2 блок фактических расчётных значений
  Serial.print("Выходной сигнал в милливольтах:");
  Serial.println(outputSignal * accuracyOutput);

  double tempDeg = (averageTemperature * 1650.0 / 1023.0 - 500.0) / 10.0;
  Serial.print("Температура в градусах C: ");
  Serial.println(tempDeg * accuracyInput);

  Serial.print("Напряжение шунта в милливольтах: ");
  Serial.println(valueOfCurrent * accuracyInput);

  Serial.print("Опорное напряжение в милливольтах: ");
  Serial.println(voltageSupport * accuracyInput);

  //6.3 блок
  //выбранный режим расчёта
  Serial.print("Текущий режим работы: ");
  Serial.println(mode);

  //текущее событие
  Serial.print("Текущиее событие: ");
  Serial.println(event);

  //статус таймеров
  Serial.print("Статус таймеров: ");
  Serial.println(timerMode);

  /*
    for(int i = 0 ; i < 10 ; i ++) {
    Serial.print("элемент массива: ");
    Serial.println(i);
    Serial.println(arrayTemp[i]);
    }
  */
  Serial.println(" ");
  Serial.println(" ");
}
