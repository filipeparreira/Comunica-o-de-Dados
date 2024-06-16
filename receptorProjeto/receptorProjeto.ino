const int ldr = A0;
int taxa = 1000;
int valorSensor = 0;
boolean configurado = true;

// Definindo o valor de 1 e 0 para condições ideais
int valorZero = 100; // Menor que 100
int valorUm = 350; // Maior que 350
void setup() {
  pinMode(ldr, INPUT);
  Serial.begin(9600);
  while (!Serial) {
    ; // Aguarda a conexão
  }
}

void loop() {
  if (lerValor() == 1){
    if (configurado){
      Serial.println("Iniciando codificação");
      delay(1900);
      taxa = configurarTaxa();
      Serial.println("Valor da taxa: " + String(taxa));
    } else {
      int tamanho = getTamanhoPalavra();
      Serial.print("Tamanho Recebido: ");
      Serial.println(tamanho);
      
      if (lerValor() == 1){
        delay(taxa);
        String mensagem = nrzL(tamanho);
        Serial.print("Mensagem recebida: ");
        Serial.println(mensagem);
      } else {
        delay(taxa);
        String mensagem = nrzI(tamanho);
        Serial.print("Mensagem recebida NRZ-I: ");
        Serial.println(mensagem);
      }
    }
  }  
}

int lerValor(){
  if (analogRead(ldr) > valorUm) {
    return 1;
  } else {
    return 0;
  }
}

int configurarTaxa(){
  int numeroDeBits = 0;
  String dado = "";
  while (numeroDeBits <= 10){
    Serial.println("Valor LDR lido: " + String(analogRead(ldr)));
    dado += String(lerValor());
    delay(500);
    numeroDeBits++;
  }
  Serial.println("Acabou a codificação.");
  configurado = false;
  return binaryStringToInt(dado);
}

int binaryStringToInt(String binaryString) {
  int result = 0;
  int length = binaryString.length();
  
  for (int i = 0; i < length; i++) {
    if (binaryString.charAt(i) == '1') {
      result += (1 << (length - 1 - i));
    }
  }
  return result;
}

int getTamanhoPalavra(){
  delay(taxa*1.5);
  String tamanho = "";
  int numeroDeBits = 0;
  Serial.println("Recebendo tamanho da palavra...");
  
  while(numeroDeBits < 9){
    Serial.println("Valor LDR lido: " + String(analogRead(ldr)));
    tamanho += String(lerValor());
    numeroDeBits++;
    delay(taxa);
  } 
  Serial.println("Tamanho lido: " + tamanho);
  return binaryStringToInt(tamanho);
}

String nrzL(int tamanho){
  delay(taxa);
  String mensagem = "";
  int numeroDeBits = 0;
  Serial.println("Recebendo mensagem...");
  
  Serial.print("Tamanho mensagem: ");
  Serial.println(tamanho);

  
  while(numeroDeBits < tamanho){
    Serial.println("Valor LDR lido: " + String(analogRead(ldr)));
    mensagem += String(lerValor());
    numeroDeBits++;
    delay(taxa);
  }
  Serial.print("Mensagem em binario: ");
  Serial.println(mensagem);
  
  mensagem = binaryToString(mensagem);
  if (verificaCRC(mensagem)) {
    Serial.println("Mensagem recebida com sucesso (CRC OK): " + mensagem);
  } else {
    Serial.println("Erro na mensagem recebida (CRC Falhou)");
  }

  return mensagem;
}

String nrzI(int tamanho) {
  delay(taxa);
  String mensagem = "";
  int numeroDeBits = 0;
  bool ultimoBit = LOW; // Estado inicial

  Serial.println("Recebendo mensagem (NRZ-I)...");
  Serial.print("Tamanho mensagem: ");
  Serial.println(tamanho);

  // Leitura do bit inicial
  ultimoBit = lerValor();

  while (numeroDeBits < tamanho) {
    int valorAtual = lerValor();
    if (valorAtual != ultimoBit) {
      mensagem += "1";
      ultimoBit = valorAtual;
    } else {
      mensagem += "0";
    }
    numeroDeBits++;
    delay(taxa);
  }
  Serial.print("Mensagem em binario: ");
  Serial.println(mensagem);

  mensagem = binaryToString(mensagem);
  if (verificaCRC(mensagem)) {
    Serial.println("Mensagem recebida com sucesso (CRC OK): " + mensagem);
  } else {
    Serial.println("Erro na mensagem recebida (CRC Falhou)");
  }

  return mensagem;
}

String binaryToString(String bin) {
  String str = "";
  for (int i = 0; i < bin.length(); i += 8) { // Incrementa de 9 em 9 para pular o espaÃ§o
    String binChar = bin.substring(i, i + 8);
    Serial.print("Palavra binario: ");
    Serial.println(binChar);
    char c = 0;
    for (int j = 0; j < 8; j++) {
      c = (c << 1) | (binChar.charAt(j) - '0');
    }
    str += c;
  }
  return str;
}

bool verificaCRC(String mensagem) {
  const String polinomio = "1101"; // PolinÃ´mio gerador

  for (int i = 0; i <= mensagem.length() - polinomio.length(); ) {
    for (int j = 0; j < polinomio.length(); j++) {
      mensagem.setCharAt(i + j, mensagem.charAt(i + j) == polinomio.charAt(j) ? '0' : '1');
    }
    while (i < mensagem.length() && mensagem.charAt(i) != '1') {
      i++;
    }
  }

  String crcRecebido = mensagem.substring(mensagem.length() - 3);
  return crcRecebido == "000";
}
