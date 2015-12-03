/* Teste Sensores
 * Código para leitura de sensores ópticos e análise via console serial
 * Autores: Gabriel Alves de Lima
 * última alteração: 02/12/2015 - Adição de comentários
 */

//Define a quantidade de sensores e tempo para legibilidade da saída serial
#define NUMERO_DE_SENSORES 9
#define TEMPO 800

//Define um vetor com todos os pinos dos sensores
int sensores[NUMERO_DE_SENSORES] = {A0,A1,A2,A3,A4,A5,A6,A7,A8};

//Define os pinos dos sensores como entradas e inicia um fluxo serial
void setup() {
	for(int i = 0; i < NUMERO_DE_SENSORES; i++){
		pinMode(sensores[i], INPUT);
	}

	Serial.begin(9600);
 }

//Lê todos os sensores e imprime no monitor serial
void loop() {
	for(int i = 0; i < NUMERO_DE_SENSORES; i++) {
		Serial.print("Sensor ");
		Serial.print(i + 1);
		Serial.print(": ");
		Serial.println(analogRead(sensores[i]));
	}
	Serial.println("");
	Serial.println("***********")
	delay(TEMPO);	//Delay para legibilidade do código. 
}