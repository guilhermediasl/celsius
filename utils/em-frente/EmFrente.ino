//EmFrente.ino
//Código para robô com dois motores andar para frente
//Autor: Gabriel Alves de Lima, Kewim Lima da silva

//Constantes para uso durante o programa
//Define as portas dos motores e velocidades
#define MOTOR_D1 7	//O D1 e o E1, quando ligados, devem mandar o robô para frente
#define MOTOR_D2 8
#define MOTOR_E1 9
#define MOTOR_E2 10
#define VELOCIDADE_BASE 100
#define VELOCIDADE_MAXIMA 255

//Define os pinos dos motores como saída
void setup() {
	pinMode(MOTOR_D2, OUTPUT);
	pinMode(MOTOR_D1, OUTPUT);
	pinMode(MOTOR_E2, OUTPUT);
	pinMode(MOTOR_E1, OUTPUT);
}

//Ordena os motores a andar para frente
void loop() {
	motorDireito(VELOCIDADE_BASE);
	motorEsquerdo(VELOCIDADE_BASE);
}

//Função que limita a potência para a velocidade máxima
int limitadorDePotencia(int potencia) {
	if(abs(potencia) > VELOCIDADE_MAXIMA) {
		return (potencia/abs(potencia)) * VELOCIDADE_MAXIMA;
	}
	else {
		return potencia;
	}
}

//Função que controla o motor direito
void motorDireito(int potencia) {
	int novaPotencia = limitadorDePotencia(potencia);

	if(potencia > 0) {
		analogWrite(MOTOR_D1, novaPotencia);
		analogWrite(MOTOR_D2, 0);
	}
	else {
		analogWrite(MOTOR_D2, -novaPotencia);
		analogWrite(MOTOR_D1, 0);
	}
}

//Função que controla o motor esquerdo
void motorEsquerdo(int potencia) {
	int novaPotencia = limitadorDePotencia(potencia);

	if(potencia > 0) {
		analogWrite(MOTOR_E1, novaPotencia);
		analogWrite(MOTOR_E2, 0);
	}
	else {
		analogWrite(MOTOR_E2, -novaPotencia);
		analogWrite(MOTOR_E1, 0);
	}
}
