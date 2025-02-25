      
// CODIGO DE PROGRAMACAO PARA ESP32 PARA PROJETO INTEGRADOR 2 ADAPTADO DO PROJETO ROCK AND METAL RACING - PROJETO DE EXTENSAO DO IFSC ITAJAI - SC - 2024

// Codigo adaptado por Kauan de Lima adaptado de NodeMCU esp8266(https://www.youtube.com/watch?v=gU7vlhU1HvU)
// Endereco HTML: http://192.168.4.1/?State=comandoquevaireceber
// Ex: http://192.168.4.1/?State=LEDON
// OBS: Letras maiusculas e minusculas devem ser feitas corretamente
// RESOLVENDO PROBLEMA DE ATUALIZAÇÃO DO ESP PARA VERSÃO 3 (https://forum.sunfounder.com/t/please-help-compilation-error-ledcsetup-was-not-declared-in-this-scope/1382/4)
// instale o ESP32, escolhendo a versão 2.0.14.
// the number of the LED pin



#define SERVODIRECAOPINO 23  // Pino do servo de direcao da reach stacker
#define SERVO_SPREADER_PINO 15  // Pino do servo trava do spreader da reach stacker
//#define BUZZER 15 // SIRENE DE RE
#define LEDAZUL 2 //LED AZUL DA PLACA ESP
#define BUTTONBOOT 0 //BOTAO DE BOOT NA PLACA DO ESP
//#define FAROLFRENTE 15 //LED FAROL DA FRENTE
//#define FAROLATRAS 18 //LED FAROL DE RE

//---------------------------------------
//CONFIGS PINOS DA PONTE 1 MOTORES DE TRAÇÃO
#define ENA 25   // Pino para o motor A
#define ENB 13   // Pino para o motor B
#define IN_1 26 // L298N in1 motors Right           GPIO15(D8)
#define IN_2 27 // L298N in2 motors Right           GPIO13(D7)
#define IN_3 14 // L298N in3 motors Left            GPIO2(D4)
#define IN_4 12  // L298N in4 motors Left            GPIO0(D3)

//---------------------------------------
//CONFIGS PINOS DA PONTE 2 CILINDROS LATERAIS
#define ENA_PONTE2 33   // Pino para o motor A da ponte 2
#define ENB_PONTE2 32   // Pino para o motor B da ponte 2
#define IN_1_PONTE2 21 // L298N in1 motors Right da ponte 2          
#define IN_2_PONTE2 19 // L298N in2 motors Right da ponte 2          
#define IN_3_PONTE2 18 // L298N in3 motors Left da ponte 2   
#define IN_4_PONTE2 5  // L298N in4 motors Left  da ponte 2


//---------------------------------------



bool ledState = false;        // Estado do LED AZUL DO ESP

unsigned long lastCommandTime = 0; // Tempo do último comando
const unsigned long debounceDelay = 100; // Atraso mínimo em milissegundos pa

#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>


Servo myServo; //cria um objeto myServo na classe Servo, esse daqui é o de direção

Servo ServoTrava; //cria um objeto ServoTrava na classe Servo, esse aqui é a trava do spreader

// substitua os dados entre parenteses com o nome da Rede e a senha desejados
const char *ssid = "ESP32"; // nome da rede que ira aparecer no wifi
const char *password = "carrinho"; //senha da rede
// demais configuracoes da rede para mais de um carrinho
int channel = 5;// canal do WIFI

/* Put IP Address details */
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);



WebServer server(80);

// setting PWM properties
const int freq = 250;
const int canal = 0; //canal interno do esp que sera utilizado para o pwm, vai de 0 - 15
const int resolution = 8;

String command;     // String to store app command state.

int speedCar = 50; // 0 - 255. velocidade dos motores de tracao
//int speed_Coeff = 2; // reduz a velocidade nas diagonais, nao sei pq

//int currentAngle = 90; // Inicializa o servo na posição neutra (reto para frente)

int angulodirecaoservo = 90; // Inicializa o servo na posição neutra (reto para frente), controlado por deslizador no app inventor, esse é o de direcao
int angulotravaservo = 0;

// Defina um canal PWM para o servo
int canalServo = 3;  // Canal de PWM para o servo

int canalServoTrava = 5;  // Canal de PWM para o servo da trava do spreader

int canalMotorA = 3;  // Canal de PWM para o motor A
int canalMotorB = 3;  // Canal de PWM para o motor B

int canalMotorA_PONTE2 = 4;  // Canal de PWM para o motor A cilindros laterais
int canalMotorB_PONTE2 = 5;  // Canal de PWM para o motor B

//-----------------------CILINDROS LATERAIS--------------------------


void avancarCilindrolateral() //faz os cilindros laterais avançarem
{

    digitalWrite(IN_3_PONTE2, LOW);
    digitalWrite(IN_4_PONTE2, HIGH);
    ledcWrite(canalMotorB_PONTE2, 255);

    //digitalWrite(IN_3, LOW);
    //digitalWrite(IN_4, HIGH);
    //ledcWrite(canalMotorB_PONTE2, 250);
}

void retornarCilindrolateral() //faz os cilindros laterais retornarem
{

    digitalWrite(IN_3_PONTE2, HIGH);
    digitalWrite(IN_4_PONTE2, LOW);
    ledcWrite(canalMotorB_PONTE2, 255);

    //digitalWrite(FAROLATRAS, HIGH);


    //digitalWrite(IN_3_PONTE2, HIGH);
    //digitalWrite(IN_4_PONTE2, LOW);
    //ledcWrite(canalMotorB_PONTE2, 250);


}



void stopCilindrosLaterais()
{

    digitalWrite(IN_3_PONTE2, LOW);
    digitalWrite(IN_4_PONTE2, LOW);
    

    //digitalWrite(FAROLATRAS, LOW);

    //digitalWrite(IN_3_PONTE2, LOW);
    //digitalWrite(IN_4_PONTE2, LOW);
    //ledcWrite(canalMotorB_PONTE2, 0);
    ledcWrite(canalMotorB_PONTE2, 0);
}

//-----------------------CILINDRO LANÇA--------------------------


void avancarCilindroLanca() //faz os cilindros laterais avançarem
{

    digitalWrite(IN_1_PONTE2, LOW);
    digitalWrite(IN_2_PONTE2, HIGH);
    ledcWrite(canalMotorA_PONTE2, 255);

    //digitalWrite(IN_3, LOW);
    //digitalWrite(IN_4, HIGH);
    //ledcWrite(canalMotorB_PONTE2, 250);
}

void retornarCilindroLanca() //faz os cilindros laterais retornarem
{

    digitalWrite(IN_1_PONTE2, HIGH);
    digitalWrite(IN_2_PONTE2, LOW);
    ledcWrite(canalMotorA_PONTE2, 255);

    //digitalWrite(FAROLATRAS, HIGH);


    //digitalWrite(IN_3_PONTE2, HIGH);
    //digitalWrite(IN_4_PONTE2, LOW);
    //ledcWrite(canalMotorB_PONTE2, 250);


}



void stopCilindroLanca()
{

    digitalWrite(IN_1_PONTE2, LOW);
    digitalWrite(IN_2_PONTE2, LOW);
    

    //digitalWrite(FAROLATRAS, LOW);

    //digitalWrite(IN_3_PONTE2, LOW);
    //digitalWrite(IN_4_PONTE2, LOW);
    //ledcWrite(canalMotorB_PONTE2, 0);
    ledcWrite(canalMotorA_PONTE2, 0);
}






//---------MOTORES DE TRAÇÃO E SERVO DE DIREÇÃO-----------------------------------------

void goAhead() //faz ir para frente
{

    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    ledcWrite(canalMotorA, 150);

    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    ledcWrite(canalMotorB, 150);
}

void goBack() //faz ir para traz
{

    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    ledcWrite(canalMotorA, 150);

    //digitalWrite(FAROLATRAS, HIGH);


    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
    ledcWrite(canalMotorB, 150);


}

void goRight() // Faz ir para a direita
{
    // Remove o debounce e realiza o movimento
    if (angulodirecaoservo < 180) {
        angulodirecaoservo += 5; // Aumenta o ângulo em 5°, limitando a 180°
        AtualizarDirecaoServo();
        Serial.print("Servo girado para a direita: ");
        Serial.println(angulodirecaoservo);
    }
}

void goLeft() // Faz ir para a esquerda
{
    // Remove o debounce e realiza o movimento
    if (angulodirecaoservo > 0) {
        angulodirecaoservo -= 5; // Decrementa o ângulo em 5°, limitando a 0°
        AtualizarDirecaoServo();
        Serial.print("Servo girado para a esquerda: ");
        Serial.println(angulodirecaoservo);
    }
}


void LEDON()
{

    digitalWrite(LEDAZUL, HIGH);
	  

}

void LEDOFF()
{

    digitalWrite(LEDAZUL, LOW);
    
}

void FAROLON()
{

//    digitalWrite(FAROLFRENTE, HIGH);
	  

}

void FAROLOFF()
{

//    digitalWrite(FAROLFRENTE, LOW);
    
}

void FAROLREON()
{

    //digitalWrite(FAROLATRAS, HIGH);
	  

}

void FAROLREOFF()
{

    //digitalWrite(FAROLATRAS, LOW);
    
}






void stopRobot()
{

    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, LOW);
    

    //digitalWrite(FAROLATRAS, LOW);

    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, LOW);
    ledcWrite(canalMotorB, 0);
    ledcWrite(canalMotorA, 0);
}

void AtualizarDirecaoServo()
{   
   myServo.write(angulodirecaoservo);
   //  ledcWrite(canalServo, angulodirecaoservo);  // Envia o valor PWM para o servo
     Serial.println(angulodirecaoservo);
      
}

void TravarServoSpreader()
{   
   ServoTrava.write(90);
   
     Serial.println("Spreader Travado em 90°");
      
}

void DestravarServoSpreader()
{   
   ServoTrava.write(0);
   
     Serial.println("Spreader Destravado em 0°");
      
}

// ---------------CONFIGURAÇÕES WEB-----------------------------


//CONFIG DE LEITURA DE COMANDOS PELO APP INVENTOR
void handleRoot() {
String html = "<!DOCTYPE html>";
html += "<html lang=\"pt-BR\">";
html += "<head>";
html += "<meta charset=\"UTF-8\">";
html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\">";
html += "<title>Controle da Reach Stacker</title>";
html += "<style>";
html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
html += "body { text-align: center; font-family: Arial, sans-serif; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; width: 100vw; background-color: #f4f4f4; overflow: hidden; padding: 10px; }";
html += "h1 { font-size: 22px; margin-bottom: 10px; }";
html += ".container { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; width: 100%; max-width: 350px; }";
html += ".button { padding: 12px; font-size: 16px; font-weight: bold; cursor: pointer; border-radius: 8px; border: none; background-color: #4CAF50; color: white; width: 100%; min-width: 90px; }";


//html += ".button:hover { background-color: #45a049; }";
html += ".button:hover { opacity: 0.8; }";


html += ".up { grid-column: 2; } .left { grid-column: 1; } .right { grid-column: 3; } .down { grid-column: 2; } .stop { grid-column: 2; }";
html += ".cilindros { display: flex; flex-direction: column; margin-top: 15px; gap: 10px; width: 100%; max-width: 350px; }";

html += ".travar { grid-column: 3; background-color: #232422; color: #ff0000}";
html += ".destravar { grid-column: 1; background-color: #232422; color: #48ff00 }";

html += ".vermelho { background-color: #ff6347; }"; //cor vermelha dos botoes
html += ".cilindros { display: flex; flex-direction: column; margin-top: 15px; gap: 10px; width: 100%; max-width: 350px; }";

//html += "";
html += ".slider-container { width: 100%; max-width: 350px; margin-top: 20px; }";
html += ".slider { width: 100%; }";

html += "</style></head><body>";
html += "<h1>Controle da Reach Stacker</h1>";


html += "<div class=\"container\">";

// Botão de movimento frente (pressionado = ação, solto = parar)
html += "<button class=\"button up\" onmousedown=\"sendCommand('F')\" onmouseup=\"sendCommand('S')\" ontouchstart=\"sendCommand('F')\" ontouchend=\"sendCommand('S')\">Frente</button>";

// Botões de direcao do servo

html += "<button class=\"button left\" onclick=\"sendCommand('L')\" >Esquerda</button>";
html += "<button class=\"button right\" onclick=\"sendCommand('R')\">Direita</button>";

// Botão de movimento para tras (pressionado = ação, solto = parar)
html += "<button class=\"button down\" onmousedown=\"sendCommand('B')\" onmouseup=\"sendCommand('S')\" ontouchstart=\"sendCommand('B')\" ontouchend=\"sendCommand('S')\">Trás</button>";

html += "</div>";

html += "<div class=\"slider-container\">";
html += "<label for=\"servoSlider\">Ângulo de direção:</label>";
html += "<input type=\"range\" id=\"servoSlider\" class=\"slider\" min=\"0\" max=\"180\" value=\"90\" step=\"15\" oninput=\"updateServo(this.value)\">";
html += "<span id=\"sliderValue\">90</span>";
html += "</div>";

html += "<div class=\"container\">";
// Parada manual
//html += "<button class=\"button stop\" onclick=\"sendCommand('S')\">Parar</button>";

//botoes para travar o servo do spreader
html += "<button class=\"button destravar\" onclick=\"sendCommand('DestravarServoSpreader')\">Destravar</button>";
html += "<button class=\"button travar\" onclick=\"sendCommand('TravarServoSpreader')\">Travar</button>";

html += "</div>";

html += "<div class=\"cilindros\">";

 

// Cilindros também enviam parar ao soltar o botão //PRA ELEVAR CILINDROS LATERAIS
html += "<button class=\"button up\" onmousedown=\"sendCommand('EstendercilindroLateral')\" onmouseup=\"sendCommand('PararcilindroLateral')\" ontouchstart=\"sendCommand('EstendercilindroLateral')\" ontouchend=\"sendCommand('PararcilindroLateral')\">Elevar</button>";

html += "<button class=\"button vermelho\" onmousedown=\"sendCommand('EstendercilindroLanca')\" onmouseup=\"sendCommand('PararcilindroLanca')\" ontouchstart=\"sendCommand('EstendercilindroLanca')\" ontouchend=\"sendCommand('PararcilindroLanca')\">Estender Cilindro</button>";
html += "<button class=\"button vermelho\" onmousedown=\"sendCommand('RecolhercilindroLanca')\" onmouseup=\"sendCommand('PararcilindroLanca')\" ontouchstart=\"sendCommand('RecolhercilindroLanca')\" ontouchend=\"sendCommand('PararcilindroLanca')\">Recolher Cilindro</button>";

//botao pra parar os cilindros manualmente, por enquanto desativado mas quando quiser eh soh reativar
//html += "<button class=\"button\" onclick=\"sendCommand('PararCilindro')\">Parar cilindro</button>"; 

//PRA DESCER CILINDROS LATERAIS
html += "<button class=\"button up\" onmousedown=\"sendCommand('RecolhercilindroLateral')\" onmouseup=\"sendCommand('PararcilindroLateral')\" ontouchstart=\"sendCommand('RecolhercilindroLateral')\" ontouchend=\"sendCommand('PararcilindroLateral')\">Abaixar</button>";





html += "</div>";
html += "<script>";
html += "function sendCommand(command) { var xhr = new XMLHttpRequest(); xhr.open('GET', '/?State=' + command, true); xhr.send(); }";//comando pra dar função aos botoes de açao

html += "function updateServo(value) { var invertedValue = 180 - value; document.getElementById(\"sliderValue\").innerText = invertedValue; sendCommand('angulo' + invertedValue); }";

html += "</script></body></html>";

server.send(200, "text/html", html);




    // Processa comandos enviados via State
    if (server.hasArg("State")) {
        String command = server.arg("State");
        Serial.println(command);

        // Processa os comandos de movimento (não interfere no ângulo do servo)
        if (command == "F")
            goAhead();
        else if (command == "B")
            goBack();
        else if (command == "L")
            goLeft();
        else if (command == "R")
            goRight();
        else if (command == "S")
            stopRobot();
          //-----CILINDROS LATERAIS-----
        else if (command == "EstendercilindroLateral")
            avancarCilindrolateral();
        else if (command == "RecolhercilindroLateral")
            retornarCilindrolateral();
        else if (command == "PararcilindroLateral")
            stopCilindrosLaterais();
        //-----CILINDRO LANÇA-----
        else if (command == "EstendercilindroLanca")
            avancarCilindroLanca();
        else if (command == "RecolhercilindroLanca")
            retornarCilindroLanca();
        else if (command == "PararcilindroLanca")
            stopCilindroLanca();

        // Processa os comandos de velocidade (não afetam o servo)
        else if (command == "0")
            speedCar = 25;
        else if (command == "1")
            speedCar = 50;
        else if (command == "2")
            speedCar = 75;
        else if (command == "3")
            speedCar = 100;
        else if (command == "4")
            speedCar = 125;
        else if (command == "5")
            speedCar = 150;
        else if (command == "6")
            speedCar = 175;
        else if (command == "7")
            speedCar = 200;
        else if (command == "8")
            speedCar = 225;
        else if (command == "9")
            speedCar = 255;

        // Processa comandos LED
        if (command == "LEDON")
            LEDON();
        if (command == "LEDOFF")
            LEDOFF();

        // Processa os comandos de farol (DESATIVADO)
        if (command == "FAROLON")
            FAROLON();
        if (command == "FAROLOFF")
            FAROLOFF();
        if (command == "FAROLREON")
            FAROLREON();
        if (command == "FAROLREOFF")
            FAROLREOFF();

        // comandos pra travar e destravar o servo do spreader
        if (command == "TravarServoSpreader")
            TravarServoSpreader();
        if (command == "DestravarServoSpreader")
            DestravarServoSpreader();

        // Processa os comandos de angulação do servo de direção
        else if (command.startsWith("angulo")) {
            int angle = command.substring(6).toInt(); // Extrai o valor do ângulo 
            angulodirecaoservo = angle;
            AtualizarDirecaoServo();
        }
    }
}

//--------------CONFIG INICIAL SETUP-----------------
void setup()
{
    myServo.attach(SERVODIRECAOPINO); // Use a GPIO disponível no ESP32 para o servo de direcao
   myServo.write(90); // Inicializa o servo na posição neutra (reto para frente) para o servo de direcao

   ServoTrava.attach(SERVO_SPREADER_PINO); // Use a GPIO disponível no ESP32 para o servo da trava do spreader
   ServoTrava.write(0); // Inicializa o servo na posição destravada (virado pra esquerda) para o servo da trava do spreader
    
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN_1, OUTPUT);
    pinMode(IN_2, OUTPUT);
    pinMode(IN_3, OUTPUT);
    pinMode(IN_4, OUTPUT);

    pinMode(ENA_PONTE2, OUTPUT);
    pinMode(ENB_PONTE2, OUTPUT);
    pinMode(IN_1_PONTE2, OUTPUT);
    pinMode(IN_2_PONTE2, OUTPUT);
    pinMode(IN_3_PONTE2, OUTPUT);
    pinMode(IN_4_PONTE2, OUTPUT);



    pinMode(LEDAZUL, OUTPUT);
    pinMode(BUTTONBOOT, INPUT_PULLUP);
    //pinMode(BUZZER, OUTPUT);
    //pinMode(FAROLFRENTE, OUTPUT);
    //pinMode(FAROLATRAS, OUTPUT);
    Serial.begin(115200);
    
//----------------CONFIGURAÇÕES PWM CILINDROS LATERAIS-------------

 // Configura PWM para o motor A 
  ledcSetup(canalMotorA_PONTE2, 250, 8);  // Frequência de 5 kHz, resolução de 8 bits
  ledcAttachPin(ENA_PONTE2, canalMotorA_PONTE2);  // Pino do motor A

  // Configura PWM para o motor B cilindro lança
  ledcSetup(canalMotorB_PONTE2, 250, 8);  // Frequência de 5 kHz, resolução de 8 bits
  ledcAttachPin(ENB_PONTE2, canalMotorB_PONTE2);  // Pino do motor B




    //--------------CONFIGURAÇÕES PWM MOTORES DE TRAÇÃO-------------
 // Configura PWM para o motor A d
  ledcSetup(canalMotorA, 250, 8);  // Frequência de 5 kHz, resolução de 8 bits
  ledcAttachPin(ENA, canalMotorA);  // Pino do motor A

  // Configura PWM para o motor B cilindro lança
  ledcSetup(canalMotorB, 250, 8);  // Frequência de 5 kHz, resolução de 8 bits
  ledcAttachPin(ENB, canalMotorB);  // Pino do motor B
//-------------------------------------------------------
 



 //----------- Configurando o Wi-Fi como Access Point--------------------
    Serial.println("Configurando ponto de acesso Wi-Fi...");
    WiFi.softAP(ssid, password); // Nome da rede e senha
    bool success = WiFi.softAPConfig(local_ip, gateway, subnet);
    if (!success) {
        Serial.println("Falha na configuração do AP!");
    } else {
        Serial.println("AP configurado com sucesso.");
    }

    Serial.print("IP do AP: ");
    Serial.println(WiFi.softAPIP());
//-----------------------------------------------------------



  // Configuração do servidor
    server.on("/", HTTP_GET, handleRoot); // Uma única rota para tudo
    server.begin();
    Serial.println("Servidor iniciado!");
 
 
}

void loop() {
    server.handleClient(); // Processa as requisições HTTP
    
}