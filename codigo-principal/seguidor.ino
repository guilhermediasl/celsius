#define MOTOR_E1 9
#define MOTOR_E2 6
#define MOTOR_D1 3
#define MOTOR_D2 5
#define BTN 11
#define VELOCIDADE_MAXIMA 255
#define VELOCIDADE_BASE 100
#define NUMERO_DE_SENSORES 6
#define NUMERO_DE_VERIFICADORES 2
#define KP 10
#define KD 0
#define KI 0
#define BRANCA true
#define PRETA false
#define TEMPO_PARA_FINAL 40000 //milissegundos
#define ESQUERDA 0
#define DIREITA 1
#define TESTE_DE_MARCACAO 10
#define FINAL_POR_TEMPO 0
#define FINAL_POR_MARCACAO 1
#define FINAL_POR_CONTADOR 2
#define NUMERO_DE_CRUZAMENTOS 2
#define LIMITE_TEMPO_ESQUERDA 500

int sensores[NUMERO_DE_SENSORES] = {A0, A1, A2, A3, A4, A5};
int erros[NUMERO_DE_SENSORES] = {-4,-2,-1,1,2,4};
int verificadores[NUMERO_DE_VERIFICADORES] = {A6, A7};
int kp = KP;
int kd = KD;
int ki = KI;
int erro = 0;
int correcao = 0;
int erroAnterior = 0;
int somatorioDeErro = 0;
int velocidadeAtual = VELOCIDADE_BASE;
int limiar = 0;
int tipoDeFinal = -1;
int contadorDeFinal = 0;
bool corDaLinha = BRANCA;
bool marcacaoVista = false;
bool flagDeVerificador[NUMERO_DE_VERIFICADORES] = {false, false};
unsigned int t0 = 0;
unsigned int contadorDeVerificacaoEsquerda = 0;
unsigned int contadorDeMarcacao[NUMERO_DE_VERIFICADORES] = {0, 0};

void setup() {
	delay(1000);

	pinMode(MOTOR_E1, OUTPUT);
	pinMode(MOTOR_E2, OUTPUT);
	pinMode(MOTOR_D1, OUTPUT);
	pinMode(MOTOR_D2, OUTPUT);

	for(int i = 0; i < NUMERO_DE_SENSORES; i++) {
		pinMode(sensores[i], INPUT);
	}
	for(int i = 0; i < NUMERO_DE_VERIFICADORES; i++) {
		pinMode(verificadores[i], INPUT);
	}

	calibrarSensores(10);

	t0 = millis();
}

void loop() {

	erro = lerPontoAtual();
	correcao = (kp * erro) + (kd * (erro - erroAnterior)) + (ki * somatorioDeErro);

	correcaoConvencional();

	//if(devePararPorTempo() || devePararPorMarcacao() || devePararPorContador()) {
	//	para();
	//} 

	erroAnterior = erro;
	somatorioDeErro += erro;
	prevenirWindUp();
}

bool verificaMarcacao(int lado) {
	bool viuMarcacao = lerSensor(verificadores[lado]);

	if(viuMarcacao) {
		contadorDeMarcacao[lado]++;
	}
	else {
		flagDeVerificador[lado] = false;
		contadorDeMarcacao[lado] = 0;
	}

	if(contadorDeMarcacao[lado] > TESTE_DE_MARCACAO && !flagDeVerificador[lado]) {
		flagDeVerificador[lado] = true;
		return true;
	}
	else {
		return false;
	}
}

void prevenirWindUp() {
  if (erro == 0) {
    somatorioDeErro = 0;
  }
  else if (somatorioDeErro >= VELOCIDADE_MAXIMA) {
    somatorioDeErro = somatorioDeErro / 3;
  }
}

bool devePararPorTempo() {
	bool teste = (millis() - t0) >= TEMPO_PARA_FINAL;
	if(teste) {
		tipoDeFinal = FINAL_POR_TEMPO;
		return true;
	}
	else {
		return false;
	}
}

bool devePararPorMarcacao() {
	bool teste = false;
	
	if((millis() - t0) > 5000) {
		bool viuMarcacao = verificaMarcacao(DIREITA);
		
		if(viuMarcacao) {
		    marcacaoVista = true;
		}

		if(marcacaoVista && !verificaMarcacao(ESQUERDA)) {
			contadorDeVerificacaoEsquerda++;
		}
		else if(marcacaoVista) {
			marcacaoVista = false;
			contadorDeVerificacaoEsquerda = 0;
		}

		teste = (contadorDeVerificacaoEsquerda > LIMITE_TEMPO_ESQUERDA) && viuMarcacao;
	}
	else {
		teste = false;
	}

	if(teste) {
		tipoDeFinal = FINAL_POR_MARCACAO;
		return true;
	}
	else {
		return false;
	}
}

bool devePararPorContador() {
	if(verificaMarcacao(DIREITA)) {
		contadorDeFinal++;
	}
	
	bool teste = contadorDeFinal >= (2 + 2 * NUMERO_DE_CRUZAMENTOS) ;

	if(teste) {
		tipoDeFinal = FINAL_POR_CONTADOR;
		return true;
	}
	else {
		return false;
	}
}

void para() {
	while(true) {
		motorEsquerdo(0);
		motorDireito(0);

		if(tipoDeFinal == FINAL_POR_TEMPO) {
			digitalWrite(13, HIGH);
			delay(200);
			digitalWrite(13, LOW);
			delay(200);
			digitalWrite(13, HIGH);
			delay(200);
			digitalWrite(13, LOW);
			delay(800);
		}
		else if(tipoDeFinal == FINAL_POR_CONTADOR) {
			digitalWrite(13, HIGH);
			delay(200);
			digitalWrite(13, LOW);
			delay(200);
		}
		else {
			digitalWrite(13, HIGH);
		}
	}
}

void correcaoBruta() {
	if(correcao > 0) {
		motorEsquerdo(velocidadeAtual + correcao);
		motorDireito(- correcao);
	}
	else if(correcao < 0) {
		motorEsquerdo(correcao);
		motorDireito(velocidadeAtual - correcao);
	}
	else {
		motorEsquerdo(velocidadeAtual);
		motorDireito(velocidadeAtual);
	}
}

void correcaoConvencional() {
	motorEsquerdo(velocidadeAtual + correcao);
	motorDireito(velocidadeAtual - correcao);
}

//*********************** Funções dos sensores ******************************

//Calibra os sensores
void calibrarSensores(int numeroDeIteracoes) {
  //Lê a media de luminosidade na mesa e na linha. Esses valores definem a cor da mesa
  unsigned int mediaDaMesa = 0;
  unsigned int mediaDaLinha = 0;

  digitalWrite(13, HIGH);
  delay(1500);
  digitalWrite(13, LOW);

  for (int i = 0; i < numeroDeIteracoes; i++) {
  	mediaDaMesa += (analogRead(verificadores[0]) +
  		analogRead(verificadores[1])) / 2;
  }

  for (int i = 0; i < numeroDeIteracoes; i++) {
  	mediaDaLinha += analogRead(sensores[2]);
  }

  mediaDaMesa /= numeroDeIteracoes;
  mediaDaLinha /= numeroDeIteracoes;

  //O limiar é a média das médias
  limiar = (mediaDaMesa + mediaDaLinha) / 2;
  if (mediaDaMesa > mediaDaLinha) {
  	corDaLinha = BRANCA;
  }
  else {
  	corDaLinha = PRETA;
  }
}

//Ler sensor. Recebe a porta que ele deve ler e retorna true se o sensor está fora da linha (na mesa)
bool lerSensor(int porta) {
	int leitura = analogRead(porta);

	if ((corDaLinha == PRETA && leitura > limiar) ||
		(corDaLinha == BRANCA && leitura < limiar)) {
		return true;
	}
	else {
		return false;
	}
}

//Lê os sensores guias e retorna valores maiores que 0 para direita e menor que zero para esquerda
int lerPontoAtual() {
	int numerador = 0;
	int denominador = 0;

	for (int i = 0; i < NUMERO_DE_SENSORES; i++) {
		if (lerSensor(sensores[i])) {
			numerador += erros[i];
			denominador++;
		}
	}

	if (denominador == 0) {
		denominador = 1;
	}

	return numerador / denominador;
}


//*********************** Funções dos Motores ******************************


//Função para permitir apenas potências abaixo da máxima
int limitadorPotencia(int potencia) {
	int novaPotencia = potencia;
	if (abs(potencia) > VELOCIDADE_MAXIMA) {
		novaPotencia = (potencia / abs(potencia)) * VELOCIDADE_MAXIMA;
	}
	return novaPotencia;
}

//Controla o motor esquerdo para uma dada potência após ser limitada
void motorEsquerdo(int potencia) {
	int pwr = limitadorPotencia(potencia);

	if (pwr < 0) {
		analogWrite(MOTOR_E1, abs(pwr));
		digitalWrite(MOTOR_E2, LOW);
	}
	else {
		analogWrite(MOTOR_E2, abs(pwr));
		digitalWrite(MOTOR_E1, LOW);
	}
}

//Controla o motor direito para uma dada potência após ser limitada
void motorDireito(int potencia) {
	int pwr = limitadorPotencia(potencia);

	if (pwr < 0) {
		analogWrite(MOTOR_D1, abs(pwr));
		digitalWrite(MOTOR_D2, LOW);
	}
	else {
		analogWrite(MOTOR_D2, abs(pwr));
		digitalWrite(MOTOR_D1, LOW);
	}
}
