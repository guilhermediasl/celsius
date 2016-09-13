/* Teste Sensores
 * Código para leitura de sensores ópticos e análise via console serial
 * Autores: Gabriel Alves de Lima
 * última alteração: 02/12/2015 - Adição de comentários
 */

//Define a quantidade de sensores e tempo para legibilidade da saída serial
#define NUMERO_DE_SENSORES 6
#define NUMERO_DE_VERIFICADORES 2
#define TEMPO 800

//Define um vetor com todos os pinos dos sensores
int sensores[NUMERO_DE_SENSORES] = {A2,A3,A4,A5,A6,A7};
int verificadores[NUMERO_DE_VERIFICADORES] = {A0,A1};

//Define os pinos dos sensores como entradas e inicia um fluxo serial
void setup() {
		for(int i = 0; i < NUMERO_DE_SENSORES; i++)
			pinMode(sensores[i], INPUT);
		for(int n = 0; n < NUMERO_DE_VERIFICADORES; n++)
			pinMode(verificadores[n]);

	Serial.begin(9600);
 }

//Lê todos os sensores e imprime no monitor serial
void loop() {
	Serial.print(" VERIFICAROES: ");
	for(int n = 0; n<NUMERO_DE_VERIFICADORES; n++ ){
		if(n == NUMERO_DE_VERIFICADORES-1)
			Serial.println(analogRead(verificadores[n]));
		else{
			
			Serial.print(analogRead(verificadores[n]));
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
