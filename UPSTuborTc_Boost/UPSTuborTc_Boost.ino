// Подключаем библиотеку реального времени
#include <RTC.h>
//создаём макроопределения для используемых пинов
#define OUTPUT_SIGNAL     A0
#define INPUT_TEMP        A2
#define INPUT_SHUNT       A4

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
// output max = 3300mV/1024 = 3.223 mV,
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
int outputMaximum= 2660;
// задайте уровень напряжения средних температурных точек в милливольтах для режима Float
int outputMiddleFloat = 2550;
//задайте значение минимального выходного напряжения в милливольтах для режима Float
int outputFloatMinimum = 2468;
//задайте значение минимального выходного напряжения в милливольтах для режима Boost
int outputBoostMinimum = 2550;

//задайте коэффициент преобразования для базовой шкалы "мВ на элемент" для получения калиброванного напряжения

double coefficientOfCalibration = 1.108;

//Характеристика шунта

//Задайте максимальное напряжение шунта
int maxVoltShunt = 60;

//Задайте максимальный ток шунта
int maxCurrentShunt = 400;

//Введите паспортную ёмкость для марки испольуемых аккумуляторов
int capacitanceOfBattery = 75;

//Укажите число параллельных цепочек из аккумуляторов
int numberBattery = 4;

//Введите значение коэффициента передачи аналогового предусилителя
int coeffAnalogueAmplifier = 25;

//Задайте ток запряда
double chargingCurrent = 2.8;

//Задайте предельный ток заряда
double maxChargCurrent = 3.5;

//Задайте порог применения ускоренного заряда
double thresholdForBoost = 0.9;

//Задайте условие окончания ускоренного заряда
double thresholdBoostEnding = 0.25;

//Задайте максимальное время работы в режиме boost в часах
byte timeInBoost = 8;

//Задайте время задержки между повторным включением режима boost в минутах
byte delayBoost = 15;

/******************************************************************************************************************************************************************/
//ВЫЧИСЛЯЕМЫЕ ЗНАЧЕНИЯ

//Напряжение соответствующее номинальному току заряда мВ

double valueOfNominalCurrentOnVoltage = 0.1*capacitanceOfBattery*numberBattery*coeffAnalogueAmplifier*maxVoltShunt/maxCurrentShunt;

//Напряжение, соответствующее выбранному току заряда
int voltageOfCharge = valueOfNominalCurrentOnVoltage*chargingCurrent;

//Напряжение, соответствующее предельному току заряда
int limitVoltageOfCharge = valueOfNominalCurrentOnVoltage*maxChargCurrent;

//Вычисляем значение при котором происходит начало ускоренного заряда (переключение с Float на Boost)

int switchFloatToBoost = valueOfNominalCurrentOnVoltage*thresholdForBoost;

//Вычисляем пороговое значение, при котором происходит переключения с функции Boost на Float (окончание ускоренного заряда)

int switchBoostToFloat = valueOfNominalCurrentOnVoltage*thresholdBoostEnding;

//Вычисляем температуры для 10-битного АЦП

//Вычисление минимальной температуры графика Float для 10-битного АЦП
int minTempFloatADC = (millivoltAtZeroDegrees + minTempFloatDeg*changingMillivoltPerOneDegrees)/accuracyInput;
// вычисляем первую среднюю точку для 10-битного АЦП
int tempFirstPointFloatADC = (millivoltAtZeroDegrees + tempFirstMidPointFloat*changingMillivoltPerOneDegrees)/accuracyInput;
//вычисляем вторую среднюю точку для 10-битного АЦП
int tempSecondPointFloatADC = (millivoltAtZeroDegrees + tempSecondMidFloatDeg*changingMillivoltPerOneDegrees)/accuracyInput;
//вычисляем максимальну температуру для 10-битного АЦП
int maxTempFloatADC= (millivoltAtZeroDegrees + maxTempFloatDeg*changingMillivoltPerOneDegrees)/accuracyInput;

//Вычисление минимальной температуры графика Boost для 10-битного АЦП
int minTempBoostADC = (millivoltAtZeroDegrees + minTempBoostDeg*changingMillivoltPerOneDegrees)/accuracyInput;
//Вычисление максимальной температуры графика Boost после которого происходит переход на Float для 10 битного АЦП
int maxTempBoostADC = (millivoltAtZeroDegrees + maxTempBoostDeg*changingMillivoltPerOneDegrees)/accuracyInput;

//Вычисляем значения выходного напряжения управления для 10 битного ЦАП

//вычисляем значение выходного напряжения для 10-битного ЦАП, увеличиваем на единицу, т.к. в данной версии для Тубор это необходимо
//из-за грубого округления при отбрасывании десятых долей
int outputMaxDAC = outputMaximum/accuracyOutput + 1;
//вычисляем значение выходного напряжение средних точек Float для 10-битного ЦАП
int outputMidFloatDAC = outputMiddleFloat/accuracyOutput;
//вычисляем значение выходно напряжения макисмальной точки для Boost, но в данном случае оно равно значению средних точек Float
int outputMaxBoostDAC = outputMidFloatDAC;
//вычисляем значение выходного напряжения при максимальной температуре для 10-битного ЦАП
int outputMinDAC = outputFloatMinimum/accuracyOutput;

//вычисляем точку калибровки для 10-битного АЦП
int tempCalibrationADC = (millivoltAtZeroDegrees + tempCalibrationDeg*changingMillivoltPerOneDegrees)/accuracyInput;

//Перменные

//Созадём массив на 10 точек для скользящей средней по температуре без инициализации
int arrayTemp[10];

//Создаём переменные для выходного сигнала и средней температуры для их использования в функции отображения на экране.
int outputSignal;
int averageTemperature;
int shuntCurrent;

//создаём объект реального времени
RTCZero rtc;

/* Change these values to set the current initial time */
const byte seconds = 0;
const byte minutes = 0;
const byte hours = 0;

/* Change these values to set the current initial date */
const byte day = 1;
const byte month = 6;
const byte year = 20;

/****************************************************************************************************************************************************************/

void setup() {

  //запускаем таймер реального времени
  rtc.begin();
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);
  // put your setup code here, to run once:
  pinMode(INPUT_TEMP, INPUT);
  pinMode(INPUT_SHUNT, INPUT);
  //Устанавливаем разрешение для работы с входным сигналом
  analogReference(AR_INTERNAL1V65);
  
  analogWriteResolution(10);
  analogReadResolution(10);
  
  // инициализация массива для скользящей средней
  for (int i=0; i<10; i++)
  {
  arrayTemp[i] = analogRead(INPUT_TEMP);
  delay(100);
  }

  // делаем первоначальную иницаиализацию выходного сигнала
  outputSignal = outputMidFloatDAC;
   Serial.begin(9600);
}

/*************************************************************************************************************************************************************************************/

void loop() {

  

 //присваиваем переменной температуры значение скользящей средней, взятой из функции
  averageTemperature = getMovingAverageTen(arrayTemp);

  // получаем данные по напряжению от шунта
  int valueOfCurrent = analogRead(INPUT_SHUNT);

//условие калибровки (если температура ниже минус 40, то включаем режим выходного сигнала, равный среднему значению при Флоат)
if(averageTemperature <= tempCalibrationADC) {
  outputSignal = outputMidFloatDAC;
}
else {
  
  //Если ток на шунте меньше, чем порог перехода от Буст к Флоат
  if(valueOfCurrent < switchBoostToFloat){
    //если прошлый сигнал был Буст, делаем переход от Буст к Флоат.
    if(outputSignal > outputFloat(averageTemperature) + 10){
      int numberOfStepsFB = (outputSignal - outputFloat(averageTemperature))/3;
      for(int i = 0; i < numberOfStepsFB; i++){
        outputSignal -= 3;
        analogWrite (OUTPUT_SIGNAL, outputSignal);
        delay(1000);  
      }
    }
    //иначе присваиваем выходному сигналу новое значение Флоат
    else
    outputSignal = outputFloat(averageTemperature);
   
  }
  
  else {
    //проверяем условие: если выходной сигнал в прошлый раз был Float и одновременно значение тока было выше чем порог перехода от Флоат к Буст, то делаем переход
    if(outputSignal < outputBoost(averageTemperature) - 10 && valueOfCurrent > switchFloatToBoost) {
      int numberOfSteps = (outputBoost(averageTemperature)-outputSignal)/3;
      for(int i=0; i<numberOfSteps; i++) {
        outputSignal += 3;
        analogWrite (OUTPUT_SIGNAL, outputSignal);
        delay(1000);      
      }
    }
    //иначе если проверяем условие: если в прошлый раз был Флоат и текущее значение тока меньше, чем порог перехода от Флоата к Буст, то приравниваем выходной сигнал новому значению Флоат.
    else if(outputSignal < outputBoost(averageTemperature) - 10 && valueOfCurrent <= switchFloatToBoost)
    outputSignal = outputFloat(averageTemperature);
    else
    outputSignal = outputBoost(averageTemperature);
  }

}
  
  displayingDataTemp (); 
  
  analogWrite (OUTPUT_SIGNAL, outputSignal);
  
  delay(1000);

}
/*********************************************************************************************************************************************************************************/

//вычисление выходного сигнала по графику Float

int outputFloat(int tempLevel){
   int outputSignalFloat;
   if(tempLevel <= minTempFloatADC){
    outputSignalFloat = outputMaxDAC;
  } 
  else if (tempLevel > minTempFloatADC && tempLevel <= tempFirstPointFloatADC){
    outputSignalFloat = map(tempLevel, minTempFloatADC, tempFirstPointFloatADC, outputMaxDAC, outputMidFloatDAC); 
  }
  else if (tempLevel > tempFirstPointFloatADC && tempLevel <= tempSecondPointFloatADC) {
    outputSignalFloat = outputMidFloatDAC;
  }
  else if (tempLevel > tempSecondPointFloatADC && tempLevel <= maxTempFloatADC){
    outputSignalFloat = map(tempLevel, tempSecondPointFloatADC, maxTempFloatADC, outputMidFloatDAC, outputMinDAC);
  }
  else if (tempLevel > maxTempFloatADC){
    outputSignalFloat = outputMinDAC;
  }
  return outputSignalFloat;
  
}

//вычисление выходного сигнала по графику Boost

int outputBoost(int tempLevel){
   int outputSignalBoost;
   if(tempLevel <= minTempBoostADC) {
    outputSignalBoost = outputMaxDAC;
   }
   else if(tempLevel > minTempBoostADC && tempLevel <= maxTempBoostADC) {
     outputSignalBoost = map (tempLevel, minTempBoostADC, maxTempBoostADC, outputMaxDAC, outputMaxBoostDAC);    
   }
   else outputSignalBoost = outputFloat (tempLevel);
   
   return outputSignalBoost;
}

//вычисление скользящей средней на 10 точек

int getMovingAverageTen (int arrayTemp[10]) {
  for(byte j = 0; j < 9; j++){
    arrayTemp[j] = arrayTemp[j+1];
  }

  arrayTemp[9] = analogRead(INPUT_TEMP);

  int sum = 0;
  for(byte i = 0; i < 10 ; i++){
    sum += arrayTemp[i];
   }
  
  return sum/10;
}

//функция показа данных в мониторе порта

void displayingDataTemp () {
  
  Serial.print("Output signal: ");
  Serial.println(outputSignal);
  
  Serial.print("Выходной сигнал в миллиВольтах:");
  Serial.println(outputSignal*accuracyOutput);
  Serial.print("Temperature in 10 bit: ");
  Serial.println(averageTemperature);
  double tempDeg = (averageTemperature*1650.0/1023.0-500.0)/10.0;
  Serial.print("Temperature in grad C: ");
  Serial.println(tempDeg);

  
  /*
  for(int i = 0 ; i < 10 ; i ++) {
  Serial.print("элемент массива: ");
  Serial.println(i);
  Serial.println(arrayTemp[i]);
  }
  */
  Serial.println(" ");
}
