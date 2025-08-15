#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// =================================================================
//                      DEFINIÇÕES GLOBAIS
// =================================================================
LiquidCrystal_I2C lcd(0x27, 20, 4); // Configurado para LCD 20x4

const int PINO_BOTAO_ESQUERDA = 2;
const int PINO_BOTAO_DIREITA = 4;
const int PINO_BOTAO_CONFIRMA = 3;
const int PINO_BUZZER = 8;

// Comandos
#define COMANDO_NENHUM   0
#define COMANDO_ESQUERDA 1
#define COMANDO_DIREITA  2
#define COMANDO_CONFIRMA 3

// Definições de Notas Musicais
#define NOTE_D4 294
#define NOTE_C4 262
#define NOTE_B3 247
#define NOTE_A3 220
#define NOTE_G3 196
#define NOTE_E4 330
#define NOTE_G4 392
#define NOTE_C5 523
#define REST    0

// Estados e Menu do Jogo
enum EstadoDoJogo { MENU, BLACKJACK, SLOTS, ROLETA, FORCA, DINO };
EstadoDoJogo estadoAtual = MENU;

const int NUMERO_DE_JOGOS = 5;
const char* nomesDosJogos[] = {"Blackjack", "Slots", "Roleta", "Forca", "Dino Game"};
int jogoSelecionado = 0;
bool precisaRedesenharMenu = true;

// Funções
void jogarBlackjack();
void jogarSlots();
void jogarRoleta();
void jogarForca();
void jogarDino();
int lerAcaoDoBotao();
int lerAcaoDoBotaoRapido();

// =================================================================
//                          FUNÇÃO SETUP
// =================================================================
void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(PINO_BOTAO_ESQUERDA, INPUT_PULLUP);
  pinMode(PINO_BOTAO_DIREITA, INPUT_PULLUP);
  pinMode(PINO_BOTAO_CONFIRMA, INPUT_PULLUP);
  pinMode(PINO_BUZZER, OUTPUT);
  randomSeed(analogRead(A0));
  lcd.setCursor(5, 1);
  lcd.print("Fliperama");
  delay(1500);
}

// =================================================================
//                         FUNÇÕES DE APOIO
// =================================================================

void exibirMenuPrincipal() {
  lcd.clear();
  String jogoStr = "> " + String(nomesDosJogos[jogoSelecionado]) + " <";
  int pad = (20 - jogoStr.length()) / 2;
  lcd.setCursor(pad, 1);
  lcd.print(jogoStr);
  lcd.setCursor(2, 2);   
  lcd.print("Selecione o jogo");
}

void playGameOverSound() {
  int defeatMelody[] = { NOTE_D4, NOTE_C4, NOTE_B3, NOTE_A3, NOTE_G3 };
  for (int i = 0; i < 4; i++) {
    tone(PINO_BUZZER, defeatMelody[i], 200);
    delay(250);
  }
  delay(250);
  tone(PINO_BUZZER, defeatMelody[4], 200);
}

void playGameWONSound() {
  int melody[] = { NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5 };
  for (int i = 0; i < 4; i++) {
    tone(PINO_BUZZER, melody[i], 150);
    delay(200);
  }
}

int lerAcaoDoBotao(){
  static unsigned long ultimoAperto = 0;
  if (millis() - ultimoAperto < 250) return COMANDO_NENHUM;
  if (digitalRead(PINO_BOTAO_ESQUERDA) == LOW) { ultimoAperto = millis(); return COMANDO_ESQUERDA; }
  if (digitalRead(PINO_BOTAO_DIREITA) == LOW) { ultimoAperto = millis(); return COMANDO_DIREITA; }
  if (digitalRead(PINO_BOTAO_CONFIRMA) == LOW) { ultimoAperto = millis(); return COMANDO_CONFIRMA; }
  return COMANDO_NENHUM;
}

int lerAcaoDoBotaoRapido(){
  static unsigned long ultimoAperto = 0;
  if (millis() - ultimoAperto < 50) return COMANDO_NENHUM;
  if (digitalRead(PINO_BOTAO_ESQUERDA) == LOW) { ultimoAperto = millis(); return COMANDO_ESQUERDA; }
  if (digitalRead(PINO_BOTAO_DIREITA) == LOW) { ultimoAperto = millis(); return COMANDO_DIREITA; }
  if (digitalRead(PINO_BOTAO_CONFIRMA) == LOW) { ultimoAperto = millis(); return COMANDO_CONFIRMA; }
  return COMANDO_NENHUM;
}

// =================================================================
//                     JOGO 1: BLACKJACK
// =================================================================

byte baralho[52]; byte topoDoBaralho; byte maoJogador[5]; byte numCartasJogador; byte maoMesa[5]; byte numCartasMesa;
int valorDaCarta(byte carta){byte v=carta%13;if(v==1){return 11;}if(v==11||v==12||v==0||v==10){return 10;}return v;}
int somaDaMao(byte mao[],byte numCartas){int s=0;int n=0;for(byte i=0;i<numCartas;i++){s+=valorDaCarta(mao[i]);if((mao[i]%13)==1){n++;}}while(s>21&&n>0){s-=10;n--;}return s;}
void embaralharBaralho(){for(int i=0;i<52;i++){baralho[i]=i+1;}for(int i=51;i>0;i--){int j=random(i+1);byte t=baralho[i];baralho[i]=baralho[j];baralho[j]=t;}topoDoBaralho=0;}
byte comprarCarta(){return baralho[topoDoBaralho++];}
String nomeDaCarta(byte carta){byte v=carta%13;if(v==0)return"K";if(v==1)return"A";if(v==10)return"10";if(v==11)return"J";if(v==12)return"Q";return String(v);}
void exibirMesaBlackjack(byte maoJ[], byte numJ, byte maoM[], byte numM, bool esconderMesa){lcd.clear();String lJ="J:";for(int i=0;i<numJ;i++){lJ+=" "+nomeDaCarta(maoJ[i]);}String sJStr="("+String(somaDaMao(maoJ,numJ))+")";lcd.setCursor(0,0);lcd.print(lJ);if(sJStr.length()<=16-lJ.length()){lcd.setCursor(16-sJStr.length(),0);lcd.print(sJStr);}String lM="M:";if(esconderMesa){lM+=" [?]";if(numM>1){lM+=" "+nomeDaCarta(maoM[1]);}}else{for(int i=0;i<numM;i++){lM+=" "+nomeDaCarta(maoM[i]);}String sMStr="("+String(somaDaMao(maoM,numM))+")";if(sMStr.length()<=16-lM.length()){lcd.setCursor(16-sMStr.length(),1);lcd.print(sMStr);}}lcd.setCursor(0,1);lcd.print(lM);}

void jogarBlackjack() {
    lcd.clear(); 
    // AJUSTE: Centralizado nas linhas do meio
    lcd.setCursor(4, 1); lcd.print("Iniciando..."); 
    lcd.setCursor(5, 2); lcd.print("Blackjack"); 
    delay(1500);
   
    embaralharBaralho();
    numCartasJogador = 0; numCartasMesa = 0;
    maoJogador[numCartasJogador++] = comprarCarta(); maoMesa[numCartasMesa++] = comprarCarta();
    maoJogador[numCartasJogador++] = comprarCarta(); maoMesa[numCartasMesa++] = comprarCarta();

    bool jogadorParou = false;

    while(!jogadorParou) {
        exibirMesaBlackjack(maoJogador, numCartasJogador, maoMesa, numCartasMesa, true);
        lcd.setCursor(0,3); lcd.print("PARAR");
        lcd.setCursor(12,3); lcd.print("COMPRAR");

        if(somaDaMao(maoJogador, numCartasJogador)==21){ delay(1500); jogadorParou=true; break; }
       
        if (somaDaMao(maoJogador, numCartasJogador) > 21) {
            delay(1500);
            lcd.clear();
            // AJUSTE: Centralizado nas linhas do meio
            playGameOverSound();
            lcd.setCursor(2, 1); lcd.print("Voce estourou!");
            lcd.setCursor(4, 2); lcd.print("Voce Perdeu!"); 
            delay(1000);
            estadoAtual = MENU; precisaRedesenharMenu = true; return;
        }
        if (numCartasJogador == 5){
            playGameWONSound(); 
            lcd.clear(); 
            // AJUSTE: Centralizado nas linhas do meio
            lcd.setCursor(3, 1); lcd.print("5 cartas! WIN!");
            lcd.setCursor(4, 2); lcd.print("Voce Venceu!");
            delay(1000);
            estadoAtual = MENU; precisaRedesenharMenu = true; return;
        }
       
        int comando = COMANDO_NENHUM;
        while(comando == COMANDO_NENHUM) { comando = lerAcaoDoBotao(); }

        if(jogadorParou){ break; delay(1500); }

        if (comando == COMANDO_ESQUERDA) { maoJogador[numCartasJogador++] = comprarCarta(); }
        else if (comando == COMANDO_DIREITA) { jogadorParou = true; }
    }

    lcd.clear(); 
    // AJUSTE: Centralizado na linha do meio
    lcd.setCursor(2, 1); lcd.print("Turno da Mesa..."); 
    delay(1500);
   
    while(somaDaMao(maoMesa, numCartasMesa) < 17 && numCartasMesa < 5){
        maoMesa[numCartasMesa++] = comprarCarta();
        exibirMesaBlackjack(maoJogador, numCartasJogador, maoMesa, numCartasMesa, false);
        delay(1200);
    }

    exibirMesaBlackjack(maoJogador, numCartasJogador, maoMesa, numCartasMesa, false);
    delay(3000);
   
    lcd.clear();
    int somaFinalJogador = somaDaMao(maoJogador, numCartasJogador);
    int somaFinalMesa = somaDaMao(maoMesa, numCartasMesa);

    if (somaFinalMesa > 21){
      // AJUSTE: Centralizado nas linhas do meio
      lcd.setCursor(2, 1); lcd.print("Mesa Estourou!"); 
      lcd.setCursor(4, 2); lcd.print("Voce Venceu!"); 
      playGameWONSound(); 
    }
    else if (numCartasMesa == 5){ 
      // AJUSTE: Centralizado na linha do meio
      lcd.setCursor(4, 1); lcd.print("Mesa Venceu!"); 
      playGameOverSound();  
    }
    else if (somaFinalJogador > somaFinalMesa){
      // AJUSTE: Centralizado na linha do meio
      lcd.setCursor(4, 1); lcd.print("Voce Venceu!");
      playGameWONSound(); 
    }
    else if (somaFinalMesa > somaFinalJogador){ 
      // AJUSTE: Centralizado na linha do meio
      lcd.setCursor(4, 1); lcd.print("Voce Perdeu!"); 
      playGameOverSound();
    }
    else{ 
      // AJUSTE: Centralizado na linha do meio
      lcd.setCursor(6, 1); lcd.print("Empate!"); 
      playGameOverSound();
    }
   
    delay(3000);
    estadoAtual = MENU;
    precisaRedesenharMenu = true;
}

// =================================================================
//                        JOGO 2: SLOTS
// =================================================================
char matrizSlots[3][3];
void girarRoletasSlots(){for(int j=0;j<3;j++){char r_t[]={'@','#','$','&','7','*'};for(int i=5;i>0;i--){int r=random(i+1);char t=r_t[i];r_t[i]=r_t[r];r_t[r]=t;}for(int i=0;i<3;i++)matrizSlots[i][j]=r_t[i];}}
bool verificarVitoriaSlots(){for(int i=0;i<3;i++)if(matrizSlots[i][0]==matrizSlots[i][1]&&matrizSlots[i][1]==matrizSlots[i][2])return true;if(matrizSlots[0][0]==matrizSlots[1][1]&&matrizSlots[1][1]==matrizSlots[2][2])return true;if(matrizSlots[0][2]==matrizSlots[1][1]&&matrizSlots[1][1]==matrizSlots[2][0])return true;return false;}
void imprimirMatrizSlots(){String l;int p=(20-13)/2;for(int i=0;i<3;i++){l="| "+String(matrizSlots[i][0])+" | "+String(matrizSlots[i][1])+" | "+String(matrizSlots[i][2])+" |";lcd.setCursor(p,i);lcd.print(l);}}
void jogarSlots(){
  lcd.clear();
  // AJUSTE: Já estava centralizado nas linhas do meio
  lcd.setCursor(2,1);lcd.print("Aperte Confirmar");
  lcd.setCursor(4,2);lcd.print("para girar!");
  while(lerAcaoDoBotao()!=COMANDO_CONFIRMA);
  lcd.clear();
  for(int f=0;f<15;f++){girarRoletasSlots();imprimirMatrizSlots();delay(150);}
  lcd.setCursor(2,3);lcd.print("Resultado Final:");delay(3000);
  lcd.clear();
  // AJUSTE: Centralizado na linha do meio
  lcd.setCursor(4, 1);
  if(verificarVitoriaSlots()){lcd.print("Voce Venceu!");playGameWONSound();}
  else{lcd.print("Voce Perdeu!");playGameOverSound();}
  delay(3000);estadoAtual=MENU;precisaRedesenharMenu=true;
}

// =================================================================
//                       JOGO 3: ROLETA
// =================================================================
bool ehNumeroPreto(int n){const int p[]={2,4,6,8,10,11,13,15,17,20,22,24,26,28,29,31,33,35};for(int i=0;i<18;i++)if(p[i]==n)return true;return false;}
bool ehNumeroVermelho(int n){const int v[]={1,3,5,7,9,12,14,16,18,19,21,23,25,27,30,32,34,36};for(int i=0;i<18;i++)if(v[i]==n)return true;return false;}
bool vitoriaRoleta(int t,int eN,char eC,int s){if(s==0)return(t==1&&eN==0);if(t==1)return(eN==s);if(t==2){if(eC=='p')return ehNumeroPreto(s);if(eC=='v')return ehNumeroVermelho(s);}return false;}
void fazerApostaRoleta(int t,int& eN,char& eC){bool r=true;lcd.clear();if(t==1){eN=1;while(true){if(r){lcd.setCursor(2,0);lcd.print("Aposte no Numero:");String s="< "+String(eN)+" >";lcd.setCursor(0,2);lcd.print("                    ");lcd.setCursor((20-s.length())/2,2);lcd.print(s);r=false;}int c=lerAcaoDoBotao();if(c==COMANDO_ESQUERDA){eN=(eN<36)?eN+1:0;r=true;}if(c==COMANDO_DIREITA){eN=(eN>0)?eN-1:36;r=true;}if(c==COMANDO_CONFIRMA)break;}}else if(t==2){eC='p';while(true){if(r){lcd.setCursor(3,0);lcd.print("Aposte na Cor:");String s=(eC=='p')?"<   Preto    >":"<  Vermelho  >";lcd.setCursor(0,2);lcd.print("                    ");lcd.setCursor((20-s.length())/2,2);lcd.print(s);r=false;}int c=lerAcaoDoBotao();if(c==COMANDO_ESQUERDA||c==COMANDO_DIREITA){eC=(eC=='p')?'v':'p';r=true;}if(c==COMANDO_CONFIRMA)break;}}}
void jogarRoleta(){
  int t=1;const char* ts[]={"Num Unico","Cor"};bool r=true;lcd.clear();
  while(true){
    if(r){
      // AJUSTE: Já estava bem posicionado, apenas garantindo
      lcd.setCursor(2,1);lcd.print("Tipo de Aposta:");
      String s="< "+String(ts[t-1])+" >";
      lcd.setCursor(0,2);lcd.print("                    ");
      lcd.setCursor((20-s.length())/2,2);lcd.print(s);
      r=false;
    }
    int c=lerAcaoDoBotao();if(c==COMANDO_ESQUERDA||c==COMANDO_DIREITA){t=(t==1)?2:1;r=true;}if(c==COMANDO_CONFIRMA)break;
  }
  int eN=-1;char eC=' ';fazerApostaRoleta(t,eN,eC);
  lcd.clear();
  // AJUSTE: Centralizado na linha do meio
  lcd.setCursor(1,1);lcd.print("Girando roleta...");
  delay(2000);
  int s=random(37);
  lcd.clear();
  // AJUSTE: Centralizado nas linhas do meio
  lcd.setCursor(1,1);lcd.print("Numero Sorteado:");
  lcd.setCursor(7,2);lcd.print(">> "+String(s)+" <<");
  delay(3000);
  lcd.clear();
  // AJUSTE: Centralizado na linha do meio
  lcd.setCursor(4,1);
  if(vitoriaRoleta(t,eN,eC,s)){lcd.print("Voce Venceu!");playGameWONSound();}
  else{lcd.print("Voce Perdeu!");playGameOverSound();}
  delay(1500);estadoAtual=MENU;precisaRedesenharMenu=true;
}

// =================================================================
//                        JOGO 4: FORCA
// =================================================================
const char* palavras[] = {"ARDUINO", "FLIPER", "CODIGO", "JOGADOR", "BOTAO", "DISPLAY"};
const int NUM_PALAVRAS = 6;
String palavraSecreta;
String palavraOculta;
int chances;
String letrasUsadas;
String letrasErradas; // NOVO: Variável para guardar apenas as letras erradas

// FUNÇÃO DE DESENHO MELHORADA E CORRIGIDA
void desenharForca(int erros) {
  if (erros == 0) { // Desenha a forca base apenas uma vez
    lcd.setCursor(0, 0); lcd.print(" ____ ");
    lcd.setCursor(0, 1); lcd.print("|/   ");
    lcd.setCursor(0, 2); lcd.print("|    ");
    lcd.setCursor(0, 3); lcd.print("|___ ");
  }
  // Desenha as partes do corpo progressivamente
  if (erros >= 1) { lcd.setCursor(4, 1); lcd.print("|"); }  // Corda
  if (erros >= 2) { lcd.setCursor(4, 2); lcd.print("O"); }  // Cabeça
  if (erros >= 3) { lcd.setCursor(3, 2); lcd.print("/"); }  // Braço Esquerdo
  if (erros >= 4) { lcd.setCursor(5, 2); lcd.print("\\"); } // Braço Direito
  if (erros >= 5) { lcd.setCursor(3, 3); lcd.print("/"); }  // Perna Esquerda
  if (erros >= 6) { lcd.setCursor(5, 3); lcd.print("\\"); } // Perna Direita
}

// ATUALIZADO: Mostra apenas as letras ERRADAS
void exibirPainelForca() {
  lcd.setCursor(7, 0);
  lcd.print("USADAS:          "); // Limpa a área
  lcd.setCursor(14, 0);
  lcd.print(letrasErradas);

  lcd.setCursor(8, 1);
  lcd.print("Chances: " + String(chances) + " ");

  lcd.setCursor(8, 2);
  lcd.print("                    "); // Limpa a linha da palavra
  lcd.setCursor(7, 2);
  for (unsigned int i = 0; i < palavraOculta.length(); i++){
    lcd.print(palavraOculta[i]);
    lcd.print(" ");
  }
}

void jogarForca() {
  lcd.clear();
  lcd.setCursor(7, 1);
  lcd.print("Forca!");
  delay(1500);
  lcd.clear();

  palavraSecreta = palavras[random(NUM_PALAVRAS)];
  palavraSecreta = "ARDUINO";
  palavraOculta = "";
  for (unsigned int i = 0; i < palavraSecreta.length(); i++) {
    palavraOculta += "_";
  }
  chances = 6;
  letrasUsadas = "";
  letrasErradas = ""; // NOVO: Inicializa a string de letras erradas
  char letraAtual = 'A';

  desenharForca(0);
  exibirPainelForca();

  while (chances > 0 && palavraOculta.indexOf('_') != -1) {
    lcd.setCursor(8, 3);
    lcd.print("Letra: < >");
    lcd.setCursor(16, 3);
    lcd.print(letraAtual);

    int comando;
    do {
      comando = lerAcaoDoBotao();
      if (comando == COMANDO_ESQUERDA) {
        letraAtual++;
        if (letraAtual > 'Z') letraAtual = 'A';
        lcd.setCursor(16, 3);
        lcd.print(letraAtual);
      }
      if (comando == COMANDO_DIREITA) {
        letraAtual--;
        if (letraAtual < 'A') letraAtual = 'Z';
        lcd.setCursor(16, 3);
        lcd.print(letraAtual);
      }
    } while (comando != COMANDO_CONFIRMA);

    if (letrasUsadas.indexOf(letraAtual) == -1) {
      letrasUsadas += letraAtual;
      
      bool achou = false;
      for (unsigned int i = 0; i < palavraSecreta.length(); i++) {
        if (palavraSecreta[i] == letraAtual) {
          palavraOculta[i] = letraAtual;
          achou = true;
        }
      }
      if (!achou) {
        chances--;
        letrasErradas += letraAtual; // ATUALIZADO: Adiciona a letra à lista de erradas
        desenharForca(6 - chances);
      }
      exibirPainelForca();
    }
  }

  delay(1000);
  lcd.clear();
  if (palavraOculta.indexOf('_') == -1) {
    lcd.setCursor(5, 1);
    lcd.print("Voce Venceu!");
    playGameWONSound();
  } else {
    lcd.setCursor(5, 1);
    lcd.print("Voce Perdeu!");
    lcd.setCursor(2, 2);
    lcd.print("Palavra: " + palavraSecreta);
    playGameOverSound();
  }
  delay(3000);
  estadoAtual = MENU;
  precisaRedesenharMenu = true;
}

// =================================================================
//                      JOGO 5: DINO GAME
// =================================================================
byte runner_char[8]={B01110,B01100,B01110,B01100,B01110,B00101,B01010,B01010};byte obstacle_char[8]={B00100,B01110,B11111,B01110,B01110,B00100,B00100,B00100};int dL,dCO,dLO,dS,dV;
void jogarDino(){
  lcd.createChar(0,runner_char);lcd.createChar(1,obstacle_char);
  lcd.clear();
  // AJUSTE: Já estava centralizado nas linhas do meio
  lcd.setCursor(5,1);lcd.print("Dino Game!");
  lcd.setCursor(2,2);lcd.print("Aperte p/ Pular");
  while(lerAcaoDoBotao()==COMANDO_NENHUM);
  dL=1;dCO=19;dS=0;dV=200;dLO=random(1,3);lcd.clear();
  while(true){
    lcd.setCursor(0,0);lcd.print("Score:"+String(dS)+"      ");lcd.setCursor(1,dL);lcd.write((byte)0);lcd.setCursor(dCO,dLO);lcd.write((byte)1);delay(dV);lcd.setCursor(1,dL);lcd.print(" ");lcd.setCursor(dCO,dLO);lcd.print(" ");
    if(lerAcaoDoBotaoRapido()==COMANDO_CONFIRMA)dL=(dL==1)?2:1;
    dCO--;
    if(dCO==1&&dL==dLO){
      lcd.clear();
      // AJUSTE: Já estava centralizado nas linhas do meio
      lcd.setCursor(5,1);lcd.print("Game Over!");
      lcd.setCursor(5,2);lcd.print("Score:"+String(dS));
      delay(500);playGameOverSound();delay(3000);estadoAtual=MENU;precisaRedesenharMenu=true;return;
    }
    if(dCO<-1){dS++;dCO=19;dLO=random(1,3);if(dS%1==0&&dV>50)dV-=25;}
  }
}

// =================================================================
//                      FUNÇÃO LOOP PRINCIPAL
// =================================================================
void loop() {
  if (estadoAtual == MENU) {
    if (precisaRedesenharMenu) {
      exibirMenuPrincipal();
      precisaRedesenharMenu = false;
    }
    int comando = lerAcaoDoBotao();
    if (comando == COMANDO_ESQUERDA) {
      jogoSelecionado = (jogoSelecionado > 0) ? jogoSelecionado - 1 : NUMERO_DE_JOGOS - 1;
      precisaRedesenharMenu = true;
    } else if (comando == COMANDO_DIREITA) {
      jogoSelecionado = (jogoSelecionado < NUMERO_DE_JOGOS - 1) ? jogoSelecionado + 1 : 0;
      precisaRedesenharMenu = true;
    } else if (comando == COMANDO_CONFIRMA) {
      if (jogoSelecionado == 0) estadoAtual = BLACKJACK;
      else if (jogoSelecionado == 1) estadoAtual = SLOTS;
      else if (jogoSelecionado == 2) estadoAtual = ROLETA;
      else if (jogoSelecionado == 3) estadoAtual = FORCA;
      else if (jogoSelecionado == 4) estadoAtual = DINO;
    }
  }
  else if (estadoAtual == BLACKJACK) {
    jogarBlackjack();
  }
  else if (estadoAtual == SLOTS) {
    jogarSlots();
  }
  else if (estadoAtual == ROLETA) {
    jogarRoleta(); 
  }
  else if (estadoAtual == FORCA) {
    jogarForca();
  }
  else if (estadoAtual == DINO) {
    jogarDino();
  }
}
