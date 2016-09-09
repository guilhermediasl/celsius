#define MOTOR_E1    9
#define MOTOR_E2    6
#define MOTOR_D1    3
#define MOTOR_D2    5
#define BTN         11
#define VELOCIDADE_MAXIMA       255
#define VELOCIDADE_BASE         100
#define NUMERO_DE_SENSORES      6
#define NUMERO_DE_VERIFICADORES 2
#define KP      15
#define KD      0
#define KI      0
#define DEBUG       0
#define BRANCA      true
#define PRETA       false
#define TEMPO_PARA_FINAL  40000 //milissegundos
#define ESQUERDA    0
#define DIREITA     1
#define TESTE_DE_MARCACAO   10
#define FINAL_POR_TEMPO     0
#define FINAL_POR_MARCACAO  1
#define FINAL_POR_CONTADOR  2
#define NUMERO_DE_CRUZAMENTOS   2
#define LIMITE_TEMPO_ESQUERDA   500
#define N_CARACTERES    30

float sensores[NUMERO_DE_SENSORES]           = {A2, A3, A4, A5, A6, A7};
float erros[NUMERO_DE_SENSORES]              = {-1.55,-1.05,-0.55,0.50,1.00,1.50};
float verificadores[NUMERO_DE_VERIFICADORES] = {A0, A1};
float erro            = 0;
float correcao        = 0;
float erroAnterior    = 0;
float somatorioDeErro = 0;
int velocidadeAtual = VELOCIDADE_BASE;
int limiar          = 0;
int tipoDeFinal     =-1;
int contadorDeFinal = 0;
int parada          = false;
int direito         = 0;
int esquerdo        = 0;
bool corDaLinha     = BRANCA;
bool marcacaoVista  = false;
bool flagDeVerificador[NUMERO_DE_VERIFICADORES] = {false, false};
unsigned int ultimoProcesso = 0;
unsigned int contadorDeVerificacaoEsquerda = 0;
unsigned int contadorDeMarcacao[NUMERO_DE_VERIFICADORES] = {0, 0};
char buffer[N_CARACTERES+2];
float kp = KP;
float kd = KD;
float ki = KI;

void setup() {
  if(DEBUG){
     Serial.begin(9600);
     Serial.flush();
  }
  pinMode(MOTOR_E1, OUTPUT);
  pinMode(MOTOR_E2, OUTPUT);
  pinMode(MOTOR_D1, OUTPUT);
  pinMode(MOTOR_D2, OUTPUT);

  for(int i = 0; i < NUMERO_DE_SENSORES; i++)
    pinMode(sensores[i], INPUT);
  for(int i = 0; i < NUMERO_DE_VERIFICADORES; i++)
    pinMode(verificadores[i], INPUT);
  calibrarSensores(10);
  delay(1000);

  while(lerSensor(verificadores[0]) == true && lerSensor(verificadores[1]) == true){
    if(DEBUG)
      Serial.print("Esperando.");
  }
 
  for(int c=0; c<5 ; c++){
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    delay(500);
  }
}

void loop() {
    float deltaTime = (millis() - ultimoProcesso);
    ultimoProcesso = millis();
  
  erro = lerPontoAtual();
  correcao = (kp * erro) + (kd * (erro - erroAnterior)/deltaTime/1000.00) + (ki * somatorioDeErro*deltaTime/1000.00);
  
  if(erro == erros[0] || erro == erros[NUMERO_DE_SENSORES-1])
    correcaoBruta();
  else
    correcaoConvencional();      
  erroAnterior = erro;
  somatorioDeErro += erro;
  if (erro == 0) somatorioDeErro = 0;
  prevenirWindUp();
  
  if(parada) para();

  if(Serial.available() >0)
  {
      int indice = 0;
      delay(100); 
      int numChar = Serial.available();
      if(numChar> (N_CARACTERES-1)) numChar = (N_CARACTERES-1);
   
    while(numChar--) buffer[indice++] = Serial.read();
    dividirString(buffer);
    } 
}

/*################################# FUNÇÕES DE LEITURA ########################################### */
//Ler sensor. Recebe a porta que ele deve ler e retorna true se o sensor está fora da linha (na mesa)
void calibrarSensores(int numeroDeIteracoes) 
  {
  //Lê a media de luminosidade na mesa e na linha. Esses valores definem a cor da mesa
  unsigned int mediaDaMesa = 0;
  unsigned int mediaDaLinha = 0;

  digitalWrite(13, HIGH);
  delay(1500);
  digitalWrite(13, LOW);

  for (int i = 0; i < numeroDeIteracoes; i++) 
  {
    mediaDaMesa += (analogRead(verificadores[0]) + analogRead(verificadores[1])) / 2;
  }
 
 for (int i = 0; i < numeroDeIteracoes; i++)
 {
    mediaDaLinha += analogRead(sensores[2]);
 }

  mediaDaMesa /= numeroDeIteracoes;
  mediaDaLinha /= numeroDeIteracoes;
  //O limiar é a média das médias
  limiar = (mediaDaMesa + mediaDaLinha) / 2;
  if(DEBUG){ 
    Serial.print("Media da mesa : ");
    Serial.println(mediaDaMesa);
    Serial.print("Media da linha: ");   
    Serial.println(mediaDaLinha);
    Serial.print("Limite        : ");    
    Serial.println(limiar);
  }  
  if (mediaDaMesa > mediaDaLinha)  
  {
    corDaLinha = BRANCA;
  }
  else 
  {
    corDaLinha = PRETA;
  }
}
bool lerSensor(int porta) {
  int leitura = analogRead(porta);

  if ((corDaLinha == PRETA && leitura > limiar) || (corDaLinha == BRANCA && leitura < limiar)) {
    return true;
  }
  else {
    return false;
  }
}

//Lê os sensores guias e retorna valores maiores que 0 para direita e menor que zero para esquerda
float lerPontoAtual() {
  int numerador = 0;
  int denominador = 0;

  for (int i = 0; i < NUMERO_DE_SENSORES; i++) {
    if (lerSensor(sensores[i])) {
      numerador += erros[i];
      denominador++;
    }
  }

  if (denominador == 0) 
    denominador = 1;

  return numerador / denominador;
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
/*################################### FUNÇÕES DE CONTROLE ############################################### */
void prevenirWindUp() {
    if (somatorioDeErro >= VELOCIDADE_MAXIMA)
        somatorioDeErro = somatorioDeErro / 3;
}

void correcaoBruta() {
  if(correcao > 0) {
    esquerdo = velocidadeAtual + correcao;
    direito = velocidadeAtual - correcao;
  }
  else if(correcao < 0) {
    esquerdo = velocidadeAtual + correcao;
    direito = velocidadeAtual - correcao;
  }
  else {
    esquerdo = velocidadeAtual;
    direito = velocidadeAtual;
  }
  motorEsquerdo(esquerdo);
  motorDireito(direito);
  if(DEBUG)
  {
    Serial.print("Motor Esquerdo: ");
    Serial.println(esquerdo);    
    Serial.print("Motor Direito : ");
    Serial.println(direito);
  }
}

void correcaoConvencional() {
  if(correcao > 0) {
    direito  = velocidadeAtual;
    esquerdo = velocidadeAtual - correcao;
  }
  else if(correcao < 0) {
    direito = velocidadeAtual;    
    esquerdo = velocidadeAtual - correcao;
  }
  motorEsquerdo(esquerdo);
  motorDireito(direito);
  if(DEBUG)
  {
    Serial.print("Motor Esquerdo: ");
    Serial.println(esquerdo);    
    Serial.print("Motor Direito : ");
    Serial.println(direito);
  }  
}

/*#################################### FUNÇÕES DE PARADA ##########################################*/
/*bool devePararPorMarcacao() {
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
*/
bool devePararPorContador() {
  if(verificaMarcacao(DIREITA)) {
    contadorDeFinal++;
  }
  
bool teste = contadorDeFinal >= (2 + 2 * NUMERO_DE_CRUZAMENTOS);

  if(teste) {
    tipoDeFinal = FINAL_POR_CONTADOR;
    return true;
  }
  else {
    return false;
  }
}


void para() 
{
    for(int x = 150 ; x !=0 ; x = x-50)
    { 
      motorEsquerdo(x);
      motorDireito(x);
      delay(250);
    }
}
//############################### FUNÇÕES DOS MOTORES ##########################################

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
//############################## FUNÇÕES DE STRING ##################################

void dividirString(char* data) {
      if(DEBUG) {
      Serial.print("Entrada de dados: ");
      Serial.println(data);
    }
    char* parameter;
    
    parameter = strtok (data, " ,");
    while(parameter != NULL)
    {
      serial_radical(data);
      parameter = strtok(NULL, " ,");
    }
    for(int x=0; x<16; x++)
    {
      buffer[x]='\0';
    }
    Serial.flush();
}
void serial_radical(char* data) {
    if((((data[0] == 'P' && data[1] == 'A') && data[2]=='R') && data[3] == 'A')) {
    para();
    if(DEBUG) {
          Serial.println("Parando...");
          parada = 1;
          }
    }
  
    if(data[0] == 'P' && data[1]!= 'A') {
       kp = strtol(data+1, NULL, 10);
       kp = constrain(kp, -500, 500);
         if(DEBUG) {
          Serial.print("kp setado para: ");
          Serial.println(kp);
          parada = 0;
          }
    }
  
    if(data[0] == 'I') {
       ki = strtol(data+1, NULL, 10);
       ki = constrain(ki, -500, 500);
         if(DEBUG){
          Serial.print("ki setado para: ");
          Serial.println(ki);
          parada = 0;
          }
     }
  
     if(data[0] == 'D') {
       kd = strtol(data+1, NULL, 10);
       kd = constrain(kd, -500, 500);
          if(DEBUG) {
           Serial.print("kd setado para: ");
           Serial.println(kd);
           parada = 0;
           }
     }
  
      if(data[0] == 'V') {
        velocidadeAtual = strtol(data+1, NULL, 10);
        velocidadeAtual = constrain(velocidadeAtual, -500, 500);
          if(DEBUG) {
           Serial.print("Velocidade atual está: ");
           Serial.println(velocidadeAtual);
           parada = 0;
          }
     }
     if(data[0] == 'G' && data[1] == 'O')
     {
      Serial.println("GO !");
      parada = 0;
     }
}
