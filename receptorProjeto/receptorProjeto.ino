// Define o pino do sensor LDR
const int ldr = A0;

// Taxa de transmissão (em milissegundos)
int taxa = 1000;

// Variável para armazenar o valor lido do sensor
int valorSensor = 0;

// Flag para indicar se a configuração inicial foi feita
boolean configurado = true;

// Definindo os valores para 0 e 1 nas condições ideais
int valorZero = 100; // Menor que 100 é considerado 0
int valorUm = 350; // Maior que 350 é considerado 1

void setup() {
  // Configura o pino do LDR como entrada
  pinMode(ldr, INPUT);

  // Inicializa a comunicação Serial a 9600 bits por segundo
  Serial.begin(9600);
  while (!Serial) {
    ; // Aguarda a conexão
  }
}

void loop() {
  // Verifica se há um sinal de início de transmissão
  if (lerValor() == 1){
    if (configurado){
      Serial.println("Iniciando codificação");
      delay(1900); // Delay para sincronização
      taxa = configurarTaxa();
      Serial.println("Valor da taxa: " + String(taxa));
    } else {
      int tamanho = getTamanhoPalavra();
      Serial.print("Tamanho Recebido: ");
      Serial.println(tamanho);
      
      // Verifica se o próximo bit é 1 para escolher NRZ-L ou NRZ-I
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

// Função para ler o valor do sensor LDR
int lerValor(){
  if (analogRead(ldr) > valorUm) {
    return 1;
  } else {
    return 0;
  }
}

// Função para configurar a taxa de transmissão
int configurarTaxa(){
  int numeroDeBits = 0;
  String dado = "";
  
  // Lê 10 bits de configuração
  while (numeroDeBits <= 10){
    Serial.println("Valor LDR lido: " + String(analogRead(ldr)));
    dado += String(lerValor());
    delay(500); // Delay para leitura dos bits
    numeroDeBits++;
  }
  Serial.println("Acabou a codificação.");
  configurado = false;
  return binaryStringToInt(dado);
}

// Função para converter uma string binária para inteiro
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

// Função para obter o tamanho da palavra
int getTamanhoPalavra(){
  delay(taxa * 1.5); // Delay para sincronização
  String tamanho = "";
  int numeroDeBits = 0;
  Serial.println("Recebendo tamanho da palavra...");
  
  // Lê 9 bits que representam o tamanho da palavra
  while (numeroDeBits < 9){
    Serial.println("Valor LDR lido: " + String(analogRead(ldr)));
    tamanho += String(lerValor());
    numeroDeBits++;
    delay(taxa);
  } 
  Serial.println("Tamanho lido: " + tamanho);
  return binaryStringToInt(tamanho);
}

// Função para receber mensagem usando NRZ-L
String nrzL(int tamanho){
  delay(taxa); // Delay para sincronização
  String mensagem = "";
  int numeroDeBits = 0;
  Serial.println("Recebendo mensagem...");
  
  Serial.print("Tamanho mensagem: ");
  Serial.println(tamanho);

  // Lê os bits da mensagem
  while (numeroDeBits < tamanho){
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

// Função para receber mensagem usando NRZ-I
String nrzI(int tamanho) {
  delay(taxa); // Delay para sincronização
  String mensagem = "";
  int numeroDeBits = 0;
  bool ultimoBit = LOW; // Estado inicial

  Serial.println("Recebendo mensagem (NRZ-I)...");
  Serial.print("Tamanho mensagem: ");
  Serial.println(tamanho);

  // Leitura do bit inicial
  ultimoBit = lerValor();

  // Lê os bits da mensagem
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

// Função para converter uma string binária para string ASCII
String binaryToString(String bin) {
  String str = "";
  for (int i = 0; i < bin.length(); i += 8) {
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

// Função para verificar o CRC da mensagem recebida
bool verificaCRC(String mensagem) {
  const String polinomio = "1101"; // Polinômio gerador

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
