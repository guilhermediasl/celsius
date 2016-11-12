
#define NUMERO_DE_SENSORES 7
#define NUMERO_DE_VERIFICADORES 2
#define TEMPO 800

//Define um vetor com todos os pinos dos sensores
int sensores[NUMERO_DE_SENSORES] = {A6,A5,A4,A3,A2,A1,A0};
int verificadorEsquerdo = A7;
int verificadorDireito = 4;
//Define os pinos dos sensores como entradas e inicia um fluxo serial
void setup() {
		for(int i = 0; i < NUMERO_DE_SENSORES; i++)
			pinMode(sensores[i], INPUT);
		pinMode (verificadorEsquerdo, INPUT);
                pinMode (verificadorDireito, INPUT);

	Serial.begin(9600);
 }

//Lê todos os sensores e imprime no monitor serial
void loop() {
	Serial.print(" VERIFICAROES: ");
	for(int n = 0; n<NUMERO_DE_VERIFICADORES; n++ ){
		if(n == NUMERO_DE_VERIFICADORES-1)
			Serial.println(analogRead(verificador));
		else{
			
			Serial.print(analogRead(verificador));
			Serial.print(" - ");
			
		}
	}
	Serial.print(" SENSORES: ");
	for(int n = 0; n<NUMERO_DE_SENSORES; n++ ){
		if(n == NUMERO_DE_SENSORES-1)
			Serial.println(analogRead(sensores[n]));
		else{
			
			Serial.print(analogRead(sensores[n]));
			Serial.print(" - ");
			
		}
	}
	delay(TEMPO);	//Delay para legibilidade do código. 
}
