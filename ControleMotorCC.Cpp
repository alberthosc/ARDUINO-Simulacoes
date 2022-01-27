// DCA0301 - UFRN
// Albertho Síziney Costa.
// Controle de Posição e Rotação de um motor CC com CI acionador de motor ponte H L293D.
// 08/12/2021.

#define potPin A0
#define chavePin 2
#define in1PtHPin 10
#define in2PtHPin 11
#define enph 12

volatile bool estadoEnPh = 0;
int canalAPin = 3;
int canalBPin = 4;
unsigned long constEnc = 195000;
int rpmMaximoMotor = 16;
float pulsosPorVolta = constEnc/rpmMaximoMotor;
int periodo = 100;
volatile long pulsosCanalA = 0;
volatile long pulsosPorPeriodo = 0;
volatile static unsigned long ultimoTempo = 0;

void setup()
{
  pinMode(canalAPin, INPUT_PULLUP);
  pinMode(canalBPin, INPUT_PULLUP);
  pinMode(in1PtHPin, OUTPUT);
  pinMode(in2PtHPin, OUTPUT);
  pinMode(enph, OUTPUT);
  
  Serial.begin(9600);
  digitalWrite(enph,1);
  attachInterrupt(digitalPinToInterrupt(canalAPin), atualizaEncoderA, CHANGE);
}

void loop()
{
  int valPot = analogRead(potPin);
  int vel    = map(valPot,0,1023,-255,255);
  int PWM    = min(abs(vel), 255);
  if (vel >= 0)
  {
  	digitalWrite (in1PtHPin, HIGH);
    analogWrite  (in2PtHPin, (255 - PWM));
  }
  else
  {
  	digitalWrite (in1PtHPin, LOW);
    analogWrite  (in2PtHPin, PWM);
  }
  
  unsigned long tempo = millis();
  
  if(tempo - ultimoTempo >= periodo)
  {
  	ultimoTempo = tempo;
    detachInterrupt (digitalPinToInterrupt(chavePin));
    detachInterrupt (digitalPinToInterrupt(canalAPin));
    float theta = pulsosCanalA / pulsosPorVolta * 360;
    float omega = ((pulsosPorPeriodo / pulsosPorVolta)*(60000 / periodo));
    pulsosPorPeriodo = 0;
    attachInterrupt(digitalPinToInterrupt(canalAPin), atualizaEncoderA, CHANGE);
  	attachInterrupt(digitalPinToInterrupt(chavePin), habilitaPonteH, RISING);
 	mostraDadosMotor(1,theta,omega);
  }  
}

void atualizaEncoderA()
{						//ISR para contar pulsos do encoder canal A
  if (digitalRead(canalAPin)==digitalRead(canalBPin))
  {
  	pulsosCanalA++;
  	pulsosPorPeriodo++;
  }
  else
  {
  	pulsosCanalA--;
  	pulsosPorPeriodo--;
  }  
}

void mostraDadosMotor(int op, float theta, float omega)
{
  switch (op)
  {
  	case 1:
    	Serial.print("Tempo: ");
    	Serial.print((float)ultimoTempo/1000);
   	 	Serial.print("s");
    	Serial.print("\t");
    	Serial.print("Posicao angular do Motor: ");
    	Serial.print(theta);
    	Serial.print(" Graus");
    	Serial.print("\t");
    	Serial.print("Velocidade angular do Motor: ");
    	Serial.print(omega, 0);
    	Serial.println(" RPM");
    	break;  
    case 2:
    	Serial.println(theta);
    	break;
    case 3:
    	Serial.println(omega, 0);
    	break;
  }
}

void habilitaPonteH()
{
	volatile static unsigned long ultimoDebounceTime = 0;
	unsigned long debounceTime = millis();
  	if (debounceTime - ultimoDebounceTime > 200)
  	{
  		estadoEnPh =! estadoEnPh;
  	}
  
	ultimoDebounceTime = debounceTime;
	digitalWrite(enph, estadoEnPh);
}
