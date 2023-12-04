#include <ESP8266WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient


//defines de id mqtt e tópicos para publicação e subscribe
#define TOPICO_PUBLISH "sfrancisco"     //tópico MQTT para publicar
#define TOPICO_SUBSCRIBE "teste"     //tópico MQTT de escuta

#define ID_MQTT  "ESP12e"     //id mqtt (para identificação de sessão)




#define trigPin D1 
#define echoPin D2  
 
unsigned long previousMillis = 0; // Guarda a última vez que o sinal foi enviado
const long interval = 5000; // Intervalo desejado (5 segundos em milissegundos)

// WIFI
const char* SSID = "COM241.1"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "12345678"; // Senha da rede WI-FI que deseja se conectar

// MQTT
const char* BROKER_MQTT = "192.168.4.106"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT

//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient



long duracao = 0;
float distancia, aux;
float altura = 0;

void initWiFi();
void initMQTT();
void reconectWiFi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void trocarIP();
 
void setup()
{
  
  Serial.begin(9600);
  initWiFi();
  initMQTT();
  Serial.println("Lendo dados do sensor...");

  pinMode(trigPin, OUTPUT); // pino D1 como saída para TRIGGER
  pinMode(echoPin, INPUT);  // pino D2 como entrada para ECHO
  digitalWrite(trigPin, LOW);    // Limpa trigPin
}

//Função: inicializa e conecta-se na rede WI-FI desejada
void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
     
    reconectWiFi();
}

void trocarIP() {
  IPAddress novoIP(192, 168, 4, 108);  // Novo endereço IP desejado
  IPAddress gateway(192, 168, 4, 1);   // Endereço do gateway
  IPAddress mascaraSubrede(255, 255, 255, 0);  // Máscara de sub-rede
  
  WiFi.config(novoIP, gateway, mascaraSubrede);
}


//Função: inicializa parâmetros de conexão MQTT(endereço do broker, porta e seta função de callback)      
void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada 
} 

//Função: função de callback 
//        esta função é chamada toda vez que uma informação de um dos tópicos subscritos chega)     
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
 
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }

    int valor = atoi(msg.c_str());
    if(strcmp(topic,TOPICO_SUBSCRIBE)==0){
      Serial.println(valor);
    }   
     
}

//Função: Para reconectar ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}

//Função: Para reconectar a  WiFi
void reconectWiFi() 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
    if (WiFi.localIP().toString() == String(BROKER_MQTT)) {
    Serial.println("O IP local corresponde ao BROKER_MQTT. Procurando outro IP...");
    trocarIP();
  }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}

//Função: verifica o estado das conexões WiFI e ao broker MQTT. 
//        Em caso de desconexão (qualquer uma das duas), a conexão é refeita.        
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
     
     reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}
 
//Função: envia ao Broker o estado atual do output 
void EnviaEstadoOutputMQTT(float distancia)
{
    
    String strDistancia = String(distancia);  // Converte float para String
    const char* charDistancia = strDistancia.c_str();  // Converte String para vetor de caracteres

    MQTT.publish(TOPICO_PUBLISH, charDistancia);  // Publica no tópico MQTT
    delay(1000);
}


void loop()
{
  //garante funcionamento das conexões WiFi e ao broker MQTT
  VerificaConexoesWiFIEMQTT();
  
  unsigned long currentMillis = millis(); 

  
  if (currentMillis - previousMillis >= interval) {
    // Salva o momento do último envio do sinal
    previousMillis = currentMillis;

    digitalWrite(trigPin, HIGH);   // Seta trigPin HIGH aguarda 
    delayMicroseconds(10);         // aguada 10 microsegundos
    digitalWrite(trigPin, LOW);    // Seta trigPin LOW

    // Leitura do echoPin, retorna a onda de som em microsegundos
    duracao = pulseIn(echoPin, HIGH);
    aux = duracao;
    distancia= aux * 0.034 / 2;

    if (distancia > 30){  // leitura mínima. 
      distancia = 30;
    }
      if (distancia < 0){  // leitura máxima. 
      distancia = 0;
    }
    altura = 30 - distancia; 
    // Remapeia o range de leitura  

    EnviaEstadoOutputMQTT(altura);
  }


}