#include <SoftwareSerial.h>

// Escolha 2 pinos para o SoftwareSerial
SoftwareSerial rSerial(2, 3); // RX, TX

const int tagTam = 16; //Tamanho total da tag;
const int idTam = 13; //Tamanho da parte que contem o ID;
const int cTags = 4; //Numero total de tags que queremos verificar (referente a tagsConhecidas)

// Inserir tags conhecidas
char tagsConhecidas[cTags][idTam] = {
  "51008FD71C15",
  "520000214A39",
  "555555555555",
  "7A005B0FF8D6"
};

char novaTag[idTam];

int ledLiberado = 5;
int ledNegado = 6;

void setup() {
  pinMode(ledLiberado, OUTPUT);
  pinMode(ledNegado, OUTPUT);
  // Starta as portas seriais (hardware e software)
  Serial.begin(9600);
  rSerial.begin(9600);
}

void loop() {
  int i = 0; // Contador para o array novaTag
  int readByte; // Variavel para ler a cada byte do buffer 
  boolean tag = false; // Flag para sabermos quando uma tag está posicionada

  // Isso garante que toda a tag está no buffer do serial antes de ler, 
  // o Arduino pode ler mais rápido do que o módulo ID pode fornecer
  if (rSerial.available() == tagTam) {
    tag = true;
  }

  if (tag == true) {
    while (rSerial.available()) {
      readByte = rSerial.read(); // Pega cada byte do buffer, um por vez.

      /* Verificação para registrar apenas a parte unica de cada tag,
        retirando os primeiros e os últimos bytes */
      if (readByte != 2 && readByte != 13 && readByte != 10 && readByte != 3) {
        novaTag[i] = readByte;
        i++;
      }

      // Se for o fim da tag, ETX, a variavel recebe falso, finalizando o loop.
      if (readByte == 3) {
        tag = false;
      }

    }
  }


  // nao faz nada se o array novaTag estiver cheio de zeros
  if (strlen(novaTag) == 0) {
    return;
  } else {
    int total = 0;

    for (int x = 0; x < cTags; x++) {
      total += verificarTag(novaTag, tagsConhecidas[x]);
    }

    // Se a novaTag existe em tagsConhecidas,
    // total recebe 1 entra na validacao.
    if (total > 0) {

      digitalWrite(ledLiberado, HIGH);
      Serial.println("Acesso liberado!");
      Serial.println();
      delay(3000);
      digitalWrite(ledLiberado, LOW);

    }else{
      Serial.println("Acesso negado!");
      Serial.print("Tag desconhecida: ");
      Serial.print(novaTag);
      Serial.println();
      for (int i= 1; i<5 ; i++){
        digitalWrite(ledNegado, HIGH);
        delay(200);
        digitalWrite(ledNegado, LOW);
        delay(200);
      }
    }
  }

  // Uma vez que a novaTag foi verificada, preenche o array com zeros
  // deixando a variavel pronta para ler a proxima tag.
  for (int c = 0; c < idTam; c++) {
    novaTag[c] = 0;
  }
}


int verificarTag(char nTag[], char aTag[]) {
  for (int i = 0; i < idTam; i++) {
    if (nTag[i] != aTag[i]) {
      return 0;
    }
  }
  return 1;
}
