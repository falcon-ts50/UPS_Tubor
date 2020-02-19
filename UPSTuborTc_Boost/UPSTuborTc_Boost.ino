#define OUTPUT_SIGNAL     A0
#define INPUT_TEMP        A2
#define INPUT_SHUNT       A4

//Характеристика датчика температуры ТМР-36
int millivoltAtZeroDegrees = 500;
int changingMillivoltAtOneDegrees = 10;

//Характеристика входов/выходов MKR Zero

//точность входного сигнала, мВ, при 10 битном АЦП
// 1650 мВ/ 1024 = 1,611
double accuracyInput = 1.611;

//точность выходного сигнала, мВ, при 10 битном ЦАП
// output max = 3300mV/1024 = 3.223 mV,
double accuracyOutput = 3.223;

//Температурно-вольтовая характеристика аккумулятора Тубор

// задайте минимальную температуру в градусах Цельсия, ниже которой напряжение заряда не изменяется
int minimalTemperature = 0;
//задайте первую точку в градусах Цельсия, при которой не изменяется напряжение
int tempFirstMiddlePoint = 15;
//Задайте вторую точку при которой температура не изменяется
int tempSecondMiddlePoint = 35;
//Задайте максимальную температуру в градусах Цельсия, после которой напряжение заряда не изменяется
int maximumTemperature = 50; 

//задайте максимальное значение выходного напряжения в милливольтах!!
int outputMaximum= 2660;
// задайте уровень напряжения средних температурных точек в милливольтах
int outputMiddleFloat = 2550;
//задайте значение минимального выходного напряжения в милливольтах
int  outputMinimum = 2468;

//Характеристика шунта

//Задайте номинальное значение напряжения шунта для тока Iном в мВ

double valueOfNominalCurrentOnVoltage = 30.0;


//Вычисляемые значения

//Вычисляем значение показаний шунта для 10-битного АЦП, пока не округляя

double nominalCurrentADC = valueOfNominalCurrentOnVoltage/accuracyInput;

//Вычисляем пороговое значение, при котором происходит переключения с функции boost на Термокомпенсацию

int switchBoostTcThresholdValue = nominalCurrentADC/2;

//Вычисляем температуры для 10-битного АЦП

//Вычисление минимальной температуры для 10-битного АЦП
int minTempAtADC = (millivoltAtZeroDegrees + minimalTemperature*changingMillivoltAtOneDegrees)/accuracyInput;
// вычисляем первую среднюю точку для 10-битного АЦП
int tempFirstPointAtADC = (millivoltAtZeroDegrees + tempFirstMiddlePoint*changingMillivoltAtOneDegrees)/accuracyInput;
//вычисляем вторую среднюю точку для 10-битного АЦП
int tempSecondPointAtADC = (millivoltAtZeroDegrees + tempSecondMiddlePoint*changingMillivoltAtOneDegrees)/accuracyInput;
//вычисляем максимальну температуру для 10-битного АЦП
int maxTempAtADC= (millivoltAtZeroDegrees + maximumTemperature*changingMillivoltAtOneDegrees)/accuracyInput;

//Вычисляем значения выходного напряжения управления для 10 битного ЦАП

//вычисляем значение выходного напряжения для 10-битного ЦАП, увеличиваем на единицу, т.к. в данной версии для Тубор это необходимо
//из-за грубого округления при отбрасывании десятых долей
int outputMaxDAC = outputMaximum/accuracyOutput + 1;
//вычисляем значение выходного напряжение средних точек для 10-битного ЦАП
int outputMidFloatDAC = outputMiddleFloat/accuracyOutput;
//вычисляем значение выходного напряжения при максимальной температуре для 10-битного ЦАП
int outputMinDAC = outputMinimum/accuracyOutput;

//Перменные

//Созадём массив на 10 точек для скользящей средней по температуре без инициализации
int arrayTemp[10];

//Создаём переменные для выходного сигнала и средней температуры для их использования в функции отображения на экране.
int outputSignal;
int averageTemperature;
int shuntCurrent;

void setup() {
  // put your setup code here, to run once:
  pinMode(INPUT_TEMP, INPUT);
  pinMode(INPUT_SHUNT, INPUT);
  analogReference(AR_INTERNAL1V65);
  analogWriteResolution(10);
  analogReadResolution(10);
  //first initialisation for array for Moving average
  
  for (int i=0; i<10; i++)
  {
  arrayTemp[i] = analogRead(INPUT_TEMP);
  delay(100);
  }
  outputSignal = outputMidFloatDAC;
   Serial.begin(9600);
}


void loop() {

  
  averageTemperature = getMovingAverageTen(arrayTemp);
  int valueOfCurrent = analogRead(INPUT_SHUNT);
  if(valueOfCurrent < switchBoostTcThresholdValue){
    if(outputSignal > outputTc(averageTemperature) + 10){
      
    }
    else outputSignal = outputTc(averageTemperature);
  }
  else {
    if(outputSignal < outputBoost() - 10) {
      
    }
    else 
    outputSignal = outputBoost();
  }
  
  displayingDataTemp (); 
  
  analogWrite (OUTPUT_SIGNAL, outputSignal);
  
  delay(1000);

}

int outputTc(int tempLevel){
   int outputSignal;
   if(tempLevel <= minTempAtADC){
    outputSignal = outputMaxDAC;
  } 
  else if (tempLevel > minTempAtADC && tempLevel <= tempFirstPointAtADC){
    outputSignal = map(tempLevel, minTempAtADC, tempFirstPointAtADC, outputMaxDAC, outputMidFloatDAC); 
  }
  else if (tempLevel > tempFirstPointAtADC && tempLevel <= tempSecondPointAtADC) {
    outputSignal = outputMidFloatDAC;
  }
  else if (tempLevel > tempSecondPointAtADC && tempLevel <= maxTempAtADC){
    outputSignal = map(tempLevel, tempSecondPointAtADC, maxTempAtADC, outputMidFloatDAC, outputMinDAC);
  }
  else if (tempLevel > maxTempAtADC){
    outputSignal = outputMinDAC;
  }
  return outputSignal;
  
}

int outputBoost(){
  
}

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
