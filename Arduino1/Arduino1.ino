#include <Ultrasonic.h> //Carrega a biblioteca do sensor ultrassonico
#include "SoftwareSerial.h" //Biblioteca utilizada na comunicação serial
#include <LiquidCrystal.h>  //Biblioteca utilizada para o LCD
 
//Define os pinos para o trigger e echo
#define pino_trigger 5
#define pino_echo 4
 
//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);

SoftwareSerial Serial1(6, 7); //Criando nova comunicação serial para o Esp-01
unsigned long previousMillis = 0; // Guarda a última vez que o sinal foi enviado
const long interval = 5000; // Intervalo desejado (5 segundos em milissegundos)

String inputString; // Criando o objeto String da linguagem C++

const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Declarando pinos do Arduino para o LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Referenciando os pinos do Arduino com o LCD
String linha1;
String linha2;

float altura = 0;
float valor = 0;
 
void setup()
{
  Serial1.begin(9600);//Iniciando serial que comunica esp 01
  Serial.begin(9600);
  lcd.begin(16, 2);//Iniciando  LCD
  //Animação inicial pro LCD
  delay(100);      
  linha1 = "    Iniciando   ";  //Mensagem no LCD
  linha2 = "................";  //Mensagem no LCD
  exibeMensagemLCD();  // Chamando a função visor para mostrar a mensagem
  delay(1000); // Tempo que a mensagem fica em exibição

  linha1=" COM241.1 Adm e "; // Mensagem designada no LCD para início
  linha2="Gerencia  Redes "; // do projeto
  exibeMensagemLCD();
  delay(2000);

}
 
void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Salva o momento do último envio do sinal
    previousMillis = currentMillis;

    //Le as informacoes do sensor, em cm e pol
    float cmMsec;
    long microsec = ultrasonic.timing();
    cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);

    // Limita a leitura mínima e máxima
    if (cmMsec > 30) {
      cmMsec = 30;
    }
    if (cmMsec < 0) {
      cmMsec = 0;
    }

    altura = 30 - cmMsec;

    
    Serial1.println(altura);
  }

   if (Serial1.available()) { // Verifica se há dados disponíveis na porta serial
        inputString = ""; // Cria uma string vazia para armazenar os dados recebidos
        while (Serial1.available()) { // Enquanto houver dados disponíveis na porta serial
            char inChar = (char)Serial1.read(); // Lê um caractere da porta serial
            inputString += inChar; // Adiciona o caractere lido à string de entrada
            delay(2); // Aguarda 2 milissegundos para permitir que mais dados cheguem pela porta serial
        }
        //Serial.println(inputString); // Mostra na serial do computador para conferencia
        valor = atof(inputString.c_str()); // Converte o valor recebido para tipo int
        // Trata os dados de acordo com o valor que chegar
        
        

  }
  visor(altura, valor);
}

void visor(float valorLocal1, float valorLocal2) {
    // Definindo linha1 e linha2 como objetos String
    String linha1 =  "Local 1: " + String(valorLocal1, 2) + "   "; 
    String linha2 =  "Local 2: " + String(valorLocal2, 2) + "   ";

    lcd.setCursor(0, 0);
    lcd.print(linha1);
    lcd.setCursor(0, 1);
    lcd.print(linha2);
}

void exibeMensagemLCD ()
{   
  lcd.setCursor(0, 0);// Define que a impressão seja na linha de baixo do LCD
  lcd.print(linha1);
  lcd.setCursor(0, 1);// Define que a impressão seja na linha de cima do LCD
  lcd.print(linha2);
}
