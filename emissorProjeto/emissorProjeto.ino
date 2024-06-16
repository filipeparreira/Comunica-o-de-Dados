// Define o pino do LED
const int led = 12;

// Taxa de transmissão (em milissegundos)
int taxa = 500;

// Variável para indicar se a mensagem foi enviada
boolean enviou = false;

// Tipo de codificação: 0 - NRZ-L; 1 - NRZ-I
int tipoDeCod = 0; 

void setup() {
  // Configura o pino do LED como saída
  pinMode(led, OUTPUT);
  
  // Inicializa a comunicação Serial a 9600 bits por segundo
  Serial.begin(9600);
  while (!Serial) {
    ; // Aguarda a conexão
  }
  
  // Solicita o valor da taxa de transmissão
  taxa = pedirValorInteiro("Digite o valor da taxa de transmissão.");
  Serial.println("Valor da Taxa: " + String(taxa));
  
  // Sinaliza a inicialização do LED
  digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  delay(2000);
  
  // Envia a taxa de transmissão
  enviaTaxa(taxa);
  
  // Limpa o buffer Serial
  Serial.read();
  
  // Solicita ao usuário para digitar uma frase
  Serial.println("Digite uma frase.");
}

void loop() {
  // Verifica se há dados disponíveis na porta Serial
  if (Serial.available() > 0) {
    // Lê a string até o caractere de nova linha
    String mensagem = Serial.readStringUntil('\n'); 
    mensagem = calculaCRC(mensagem);
    int tamanho = mensagem.length(); // Obtém o comprimento da string
    
    int escolha = -1;
    Serial.print("Você digitou: ");
    Serial.println(mensagem);
    
    Serial.print("Tamanho da frase: ");
    Serial.println(tamanho);
    
    // Solicita a escolha do meio de transmissão
    while (escolha == -1){
      Serial.println("Selecione o meio de transmissão: ");
      escolha = Serial.readStringUntil('\n').toInt();
      Serial.print("Escolha: ");
      Serial.println(escolha);
    }
    
    if (escolha == 0){
      // Envia o tamanho da mensagem
      enviaTamanhoMensagem(tamanho);
      
      // Prepara para a transmissão NRZ-L
      digitalWrite(led, HIGH);
      delay(taxa);
      digitalWrite(led, LOW);
      
      // Envia a mensagem usando NRZ-L
      nrzL(mensagem);
    } else if (escolha == 1){
      // Envia o tamanho da mensagem
      enviaTamanhoMensagem(tamanho);
      
      // Prepara para a transmissão NRZ-I
      digitalWrite(led, LOW);
      delay(taxa);

      // Envia a mensagem usando NRZ-I
      nrzI(mensagem);
    }

    // Solicita a próxima frase
    Serial.println("Digite outra frase e pressione Enter:");
  }
}

// Solicita um valor inteiro do usuário
int pedirValorInteiro(String mensagem){
  int numeroInt = 0;
  Serial.println(mensagem);
  Serial.setTimeout(5000); // Define o timeout para 5 segundos
  
  boolean lido = false;
  
  // Aguarda até que um valor seja lido
  while (!lido) {
    if (Serial.available() > 0){
      numeroInt = Serial.parseInt();
      lido = true;
    }
  }

  return numeroInt;
}

// Solicita uma mensagem do usuário
String pedirMensagem(String mensagem){
  String texto = "";
  Serial.println(mensagem);
  delay(5000);
  boolean lido = false;
  
  // Aguarda até que uma mensagem seja lida
  while (!lido) {
    if (Serial.available() > 0){
      texto = Serial.read();
      lido = true;
    }
  }

  return texto;
}

// Envia a taxa de transmissão em sinal luminoso
void enviaTaxa(int taxa){
  String taxaBin = String(taxa, BIN);
  
  // Padroniza a taxa para 10 bits
  taxaBin = padronizaBits(taxaBin, 10);
  Serial.println("Taxa: " + taxaBin);
  
  // Envia a taxa bit a bit
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

// Padroniza a string de bits para um tamanho específico
String padronizaBits(String bits, int padrao){
  int diferenca = padrao - bits.length(); 
  String zeros = "";
  
  // Adiciona zeros à esquerda até alcançar o tamanho desejado
  for (int i = 0; i < diferenca; i++){
    zeros += "0";
  }
  return zeros + bits;
}

// Envia uma mensagem usando codificação NRZ-L
void nrzL(String mensagem){
  String mensagemBits = stringToBinary(mensagem);
  int tamanhoMensagem = mensagemBits.length();
  
  // Prepara para a transmissão
  digitalWrite(led, HIGH);
  delay(taxa);
  digitalWrite(led, LOW);
  Serial.print("Enviando mensagem: ");
  Serial.println(mensagemBits);
  
  // Envia a mensagem bit a bit
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

// Envia o tamanho da mensagem em bits
void enviaTamanhoMensagem(int tamanho){
  String tamanhoBinario = padronizaBits(String(tamanho * 8, BIN), 9);
  
  // Prepara para a transmissão
  digitalWrite(led, HIGH);
  delay(taxa);
  digitalWrite(led, LOW);
  Serial.print("Enviando tamanho: ");
  Serial.println(tamanhoBinario);
  Serial.print("Tamanho em decimal: ");
  Serial.println(tamanho * 8);
  
  // Envia o tamanho bit a bit
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

// Envia uma mensagem usando codificação NRZ-I
void nrzI(String mensagem) {
  String mensagemBits = stringToBinary(mensagem);
  int tamanhoMensagem = mensagemBits.length();
  bool ultimoBit = LOW; // Estado inicial

  // Envia o bit inicial
  digitalWrite(led, ultimoBit);
  delay(taxa);

  Serial.print("Enviando mensagem (NRZ-I): ");
  Serial.println(mensagemBits);

  // Envia a mensagem bit a bit, invertendo o bit para '1'
  for (int i = 0; i < tamanhoMensagem; i++) {
    if (mensagemBits.charAt(i) == '1') {
      ultimoBit = !ultimoBit; // Inverte o bit
    }
    digitalWrite(led, ultimoBit);
    delay(taxa);
  }
  digitalWrite(led, LOW);
}

// Converte uma string para uma representação binária
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
    bin += binChar; 
  }
  return bin;
}

// Calcula o CRC para a mensagem
String calculaCRC(String mensagem) {
  const String polinomio = "1101"; // Polinômio gerador
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
