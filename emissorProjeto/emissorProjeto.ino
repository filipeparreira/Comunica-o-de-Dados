const int led =  12;
int taxa = 500;
boolean enviou = false;
int tipoDeCod = 0; //0 - NRZ-L; 1 - NRZ-I

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  while (!Serial) {
    ; // Aguarda a conexão
  }
  taxa = pedirValorInteiro("Digite o valor da taxa de transmissão.");
  Serial.println("Valor da Taxa: " + String(taxa));
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(2000);
  enviaTaxa(taxa);
  Serial.read();
  Serial.println("Digite uma frase.");
}

void loop() {
  if (Serial.available() > 0) {
    String mensagem = Serial.readStringUntil('\n'); // Lê a string até o caractere de nova linha
    mensagem = calculaCRC(mensagem);
    int tamanho = mensagem.length(); // Obtém o comprimento da string
    int escolha = -1;
    Serial.print("Você digitou: ");
    Serial.println(mensagem);
    
    Serial.print("Tamanho da frase: ");
    Serial.println(tamanho);
    while (escolha == -1){
      Serial.println("Selecione o meio de transmissão: ");
      escolha = Serial.readStringUntil('\n').toInt();
      Serial.print("Escolha: ");
      Serial.println(escolha);
    }
    
    
    if (escolha == 0){
      enviaTamanhoMensagem(tamanho);
      
      digitalWrite(led, HIGH);
      delay(taxa);
      digitalWrite(led, LOW);
      
      nrzL(mensagem);
    } else if (escolha == 1){
      enviaTamanhoMensagem(tamanho);
      
      digitalWrite(led, LOW);
      delay(taxa);

      nrzI(mensagem);
    }


       
    // Aguarda a próxima frase
    Serial.println("Digite outra frase e pressione Enter:");
  }
}

int pedirValorInteiro(String mensagem){
  int numeroInt = 0;
  Serial.println(mensagem);
  Serial.setTimeout(5000); // Define o timeout para 5 segundos
  
  boolean lido = false;
  
  while (!lido) {
    if (Serial.available() > 0){
      numeroInt = Serial.parseInt();
      lido = true;
    }
  }

  return numeroInt;
}

String pedirMensagem(String mensagem){
  String texto = "";
  Serial.println(mensagem);
  //Serial.setTimeout(5000); // Define o timeout para 5 segundos
  delay(5000);
  boolean lido = false;
  
  while (!lido) {
    if (Serial.available() > 0){
      texto = Serial.read();
      lido = true;
    }
  }

  return texto;
}

//Manda a taxa em sinal luminoso
void enviaTaxa(int taxa){
  String taxaBin = String(taxa, BIN);
  
  taxaBin = padronizaBits(taxaBin, 10);
  Serial.println("Taxa: " + taxaBin);
  for (int i = 0; i < 10; i++){
    if (taxaBin.charAt(i) == '0'){
      digitalWrite(led, LOW);
    } else if (taxaBin.charAt(i) == '1'){
      digitalWrite(led, HIGH);
    } else {
      Serial.println("ERRO NA CODIFICAÇÃO!!!!");
      Serial.println("LETRA: " + taxaBin.charAt(i));
    }
    delay(500);
  }
  digitalWrite(led, LOW);
}

// Faz a palavra ter um tamanho padrão de bits
String padronizaBits(String bits, int padrao){
  int diferenca = padrao - bits.length(); 
  String zeros = "";
  for (int i = 0; i < diferenca; i++){
    zeros += "0";
  }
  return zeros + bits;
}



void nrzL(String mensagem){
  String mensagemBits = stringToBinary(mensagem);
  int tamanhoMensagem = mensagemBits.length();
  digitalWrite(led, HIGH);
  delay(taxa);
  digitalWrite(led, LOW);
  Serial.print("Enviando mensagem: ");
  Serial.println(mensagemBits);
  for (int i = 0; i < tamanhoMensagem; i++){
    if (mensagemBits.charAt(i) == '0'){
      digitalWrite(led, LOW);
    } else if (mensagemBits.charAt(i) == '1'){
      digitalWrite(led, HIGH);
    }
    delay(taxa);
  }
  digitalWrite(led, LOW);
}

// Envia tamanho da mensagem
void enviaTamanhoMensagem(int tamanho){
  String tamanhoBinario = padronizaBits(String(tamanho * 8, BIN), 9);
  digitalWrite(led, HIGH);
  delay(taxa);
  digitalWrite(led, LOW);
  Serial.print("Enviando tamanho: ");
  Serial.println(tamanhoBinario);
  for (int i = 0; i < 9; i++){
    if (tamanhoBinario.charAt(i) == '0'){
      digitalWrite(led, LOW);
    } else if (tamanhoBinario.charAt(i) == '1'){
      digitalWrite(led, HIGH);
    }
    delay(taxa);
  }
  digitalWrite(led, LOW);
}

void nrzI(String mensagem) {
  String mensagemBits = stringToBinary(mensagem);
  int tamanhoMensagem = mensagemBits.length();
  bool ultimoBit = LOW; // Estado inicial

  // Envia o bit inicial
  digitalWrite(led, ultimoBit);
  delay(taxa);

  Serial.print("Enviando mensagem (NRZ-I): ");
  Serial.println(mensagemBits);

  for (int i = 0; i < tamanhoMensagem; i++) {
    if (mensagemBits.charAt(i) == '1') {
      ultimoBit = !ultimoBit; // Inverte o bit
    }
    digitalWrite(led, ultimoBit);
    delay(taxa);
  }
  digitalWrite(led, LOW);
}

String stringToBinary(String str) {
  String bin = "";
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    String binChar = "";
    for (int j = 7; j >= 0; j--) {
      if (c & (1 << j)) {
        binChar += "1";
      } else {
        binChar += "0";
      }
    }
    bin += binChar; // Adiciona um espaÃ§o entre cada byte para legibilidade
  }
  return bin;
}


String calculaCRC(String mensagem) {
  const String polinomio = "1101"; // PolinÃ´mio gerador
  String mensagemComCRC = mensagem + "000"; // Adiciona zeros para o CRC

  for (int i = 0; i <= mensagemComCRC.length() - polinomio.length(); ) {
    for (int j = 0; j < polinomio.length(); j++) {
      mensagemComCRC.setCharAt(i + j, mensagemComCRC.charAt(i + j) == polinomio.charAt(j) ? '0' : '1');
    }
    while (i < mensagemComCRC.length() && mensagemComCRC.charAt(i) != '1') {
      i++;
    }
  }

  String crc = mensagemComCRC.substring(mensagemComCRC.length() - 3);
  return mensagem + crc;
}

