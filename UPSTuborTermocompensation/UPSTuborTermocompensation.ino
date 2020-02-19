#define OUTPUT_SIGNAL     A0
#define INPUT_TEMP        A2

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
int outputMiddle = 2550;
//задайте значение минимального выходного напряжения в милливольтах
int  outputMinimum = 2468;

//Вычисляем температуры для 10-битного АЦП

//Вычисление минимальной температуры для 10-битного АЦП
int minTempAtTenBitsADC = (millivoltAtZeroDegrees + minimalTemperature*changingMillivoltAtOneDegrees)/accuracyInput;
// вычисляем первую среднюю точку для 10-битного ЦАП
int tempFirstPointAtADC = (millivoltAtZeroDegrees + tempFirstMiddlePoint*changingMillivoltAtOneDegrees)/accuracyInput;
//вычисляем вторую среднюю точку для 10-битного АЦП
int tempSecondPointAtADC = (millivoltAtZeroDegrees + tempSecondMiddlePoint*changingMillivoltAtOneDegrees)/accuracyInput;
//вычисляем максимальну температуру для 10-битного АЦП
int maxTempAtADC= (millivoltAtZeroDegrees + maximumTemperature*changingMillivoltAtOneDegrees)/accuracyInput;

//Вычисляем значения выходного напряжения для 10 битного ЦАП

//вычисляем значение выходного напряжения для 10-битного ЦАП, увеличиваем на единицу, т.к. в данной версии для Тубор это необходимо
//из-за грубого округления при отбрасывании десятых долей
int outputMaxAtDAC = outputMaximum/accuracyOutput + 1;
//вычисляем значение выходного напряжение средних точек для 10-битного ЦАП
int outputMidAtDAC = outputMiddle/accuracyOutput;
//вычисляем значение выходного напряжения при максимальной температуре для 10-битного ЦАП
int outputMinAtDAC = outputMinimum/accuracyOutput;

int arrayTemp[10];

void setup() {
  // put your setup code here, to run once:
  pinMode(INPUT_TEMP, INPUT);
  analogReference(AR_INTERNAL1V65);
  analogWriteResolution(10);
  analogReadResolution(10);
  //first initialisation for array for Moving average
  
  for (int i=0; i<10; i++)
  {
  arrayTemp[i] = analogRead(INPUT_TEMP);
  delay(100);
  }
   Serial.begin(9600);
}


void loop() {


   int averageTemperature = getMovingAverageTen(arrayTemp);
   int outputSignal = outputLevel(averageTemperature);

   

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
  
  analogWrite (OUTPUT_SIGNAL, outputSignal);
  
  delay(1000);

}

int outputLevel(int tempLevel){
   int outputSignal;
   if(tempLevel <= minTempAtTenBitsADC){
    outputSignal = outputMaxAtDAC;
  } 
  else if (tempLevel > minTempAtTenBitsADC && tempLevel <= tempFirstPointAtADC){
    outputSignal = map(tempLevel, minTempAtTenBitsADC, tempFirstPointAtADC, outputMaxAtDAC, outputMidAtDAC); 
  }
  else if (tempLevel > tempFirstPointAtADC && tempLevel <= tempSecondPointAtADC) {
    outputSignal = outputMidAtDAC;
  }
  else if (tempLevel > tempSecondPointAtADC && tempLevel <= maxTempAtADC){
    outputSignal = map(tempLevel, tempSecondPointAtADC, maxTempAtADC, outputMidAtDAC, outputMinAtDAC);
  }
  else if (tempLevel > maxTempAtADC){
    outputSignal = outputMinAtDAC;
  }
  return outputSignal;
  
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
