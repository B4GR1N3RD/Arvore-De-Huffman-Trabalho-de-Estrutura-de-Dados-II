#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define BUFFER_SIZE 256

// | Cabeçalho das funções que serão utilizadas durante a execução do programa |

typedef struct no_arvore{
    struct no_arvore *esquerda;
    struct no_arvore *direita;
    int peso;
    int indice;
}No_arvore;

int verificar_extensao(char *,char *); //Função para verificar se o arquivo é .txt para compactação ou .ggc para descompactação.

No_arvore pegar_menor(No_arvore*,int,int*); //Função responsável por buscar o nó de menor peso dentro da lista de frequencia.

void percorrer_arvore(No_arvore **,char *[],int,char *,int*); //Função responsável por percorrer a árvore em pré-ordem.

void escrita_cabecalho(int,size_t,char *[],FILE *); //Função responsável por escrever o cabeçalho do arquivo .ggc ao ser compactado.

void escrita_corpo(size_t,char *[],unsigned char *,FILE *);  //Função responsável por escrever o corpo do arquivo .ggc ao ser compactado, oque na teoria crua da execução seria o 'acumulador de BITS'.

// | ------------------------------------------------------------------------- |

int main(int argc,char *argv[]){ //Função main sendo chamada com os argumentos de verificação adequados e conforme a norma.

    if(argc != 3){ // Verificação se está sendo digitado corretamente a quantidade de argumentos na chamada da execução do terminal, se foi digitado um valor diferente do necessário acontece o tratamento de erro abaixo, e assim é mostrada essa mensagem de orientação.

        printf("\n");
        printf("Utilize a quantidade de argumentos correta para a execução devida do programa, sendo 3 argumentos e não %i como foi utilizado.\n",argc);
        printf("Exemplo: \"./manipulador -c texto.txt\"\n");
        printf("\n");    
        exit(1);
    }
    else{ // Passando pela validação da quantidade de argumentos agora vamos tratar as tags.

        char *tag = argv[1];
        if(strcmp(tag,"-c") == 0){ // Verificamos se a tag que está sendo chamada é a de compactação de arquivo, sendo ela o "-c".

            if(!verificar_extensao(argv[2],".txt")){ // Verificamos se o arquivo a ser compactado é da extensão ".txt" que é a única permitida no nosso programa.

                printf("\n");
                printf("Utilize uma extensão de arquivo adequada para utilização do programa de compactação, sendo ela \".txt\";\n");
                printf("Exemplo: \"./manipulador -c texto.txt\"\n");
                printf("\n");
                exit(1);
            }
            else{
                FILE *leitor_arquivo_txt = fopen(argv[2],"rb"); //Abertura do arquivo .txt realizada após todas as verificações anteriores.

                if(!leitor_arquivo_txt){ //Tratamento de erro básico caso falhe na abertura do arquivo.
                    printf("Erro ao abrir o arquivo: %s.\n",argv[2]);
                    exit(1);
                }
                
                fseek(leitor_arquivo_txt,0,SEEK_END); //Função que mandou o ponteiro de leitura para o final do arquivo .txt.
                long tamanho_arquivo = ftell(leitor_arquivo_txt); //Armazenou o tamanho entre o começo do arquivo e o lugar onde o ponteiro se encontrava.
                fseek(leitor_arquivo_txt,0,SEEK_SET); // Mandei o ponteiro de volta para o início do arquivo para realizar a leitura a partir daqui.

                unsigned char *leitor = malloc(tamanho_arquivo); //Criação do ponteiro de unsigned char (String) com capacidade para ler todo o arquivo de uma só vez.

                if(!leitor){ //Tratamento de erro apenas para boas práticas pq vai ser bem dificil de não existir memoria para alocar esse ponteiro de leitura.
                    printf("Erro ao alocar memória.\n");
                    fclose(leitor_arquivo_txt);
                    exit(1);
                }

                size_t extensao_arquivo = fread(leitor,1,tamanho_arquivo,leitor_arquivo_txt); //Criação de um size_t (tipo próprio para armazenamento de tamanhos de objetos) para armazenar a quantidade de bytes lidos pelo fread e armazenando no leitor;
                
                int contador_caracteres[BUFFER_SIZE]={0}; //Lista de frequência que cada caracter aparece no meu texto lido.

                for(size_t i = 0; i < extensao_arquivo;i++){ //For criado para preencher a lista de frequência com base no arquivo de texto, utilizando externsao_arquivo como limitador do for (quantidade de bytes dentro de leitor -> seria semelhante a um leitor.length() em c++ peloq eu entendi);
                    contador_caracteres[leitor[i]]++;
                }

                fclose(leitor_arquivo_txt); // fechamento do arquivo de texto que não será mais utilizado pois já peguei sua frequência de caracteres.

                int tamanho_lista=0;
                No_arvore lista_frequencia[BUFFER_SIZE] = {0}; //Criação de um array para armazenar apenas os caracteres diferentes de 0 na minha lista de frequencia.

                for(int i = 0;i < BUFFER_SIZE;i++){ //For criado para inserir no array de lista_frequencia apenas os caracteres que existem no meu texto, retirando a necessidade de ficar verificando todos os 256 no melhor caso e também já preparando os nós folhas da Árvore de Huffman.
                    if(contador_caracteres[i] != 0){
                        lista_frequencia[tamanho_lista].indice = i;
                        lista_frequencia[tamanho_lista].peso = contador_caracteres[i];

                        tamanho_lista++; //Contador da quantidade de caracteres que possuem no meu arquivo, no caso aqueles que são diferentes de 0 no meu contador_caracteres;
                    }
                }

                int nos_arvore = 0; //Inicialização do meu contador de nós para quebra do meu while.
                
                No_arvore *raiz = NULL; // Criação da Raíz da minha árvore!

                char *traducao[BUFFER_SIZE] = {NULL}; //Vetor de ponteiros para armazenar o código de tradução para cada caracter.
                char buffer[BUFFER_SIZE]; //String para armazenar o código que está sendo gerado na descida da árvore.
                int profundidades[BUFFER_SIZE] = {0}; //Vetor de int para armazenar a profundidade que é igual ao tamanho do meu código de tradução.

                if(tamanho_lista == 1){
                    traducao[lista_frequencia[0].indice] = strdup("0");
                    profundidades[lista_frequencia[0].indice] = 1;
                }
                else{
                    while(nos_arvore < (tamanho_lista-1)){
                        int indice_esq = 0,indice_dir = 0;

                        No_arvore *esquerdo = (No_arvore *)malloc(sizeof(No_arvore)); //Alocação de memória para meu  nó esquerdo do novo nó.
                        *esquerdo = pegar_menor(lista_frequencia,tamanho_lista,&indice_esq);// Preenchimento doque vai ser o nó esquerdo do meu novo nó.
                        
                        lista_frequencia[indice_esq].peso = -1; //Realizo uma remoção lógica do meu nó do array já que ele é o menor e já está sendo operado na árvore, mas não realizo uma remoção real.

                        No_arvore *direito = (No_arvore *)malloc(sizeof(No_arvore)); //Alocação de memória para meu  nó direito do novo nó.
                        *direito = pegar_menor(lista_frequencia,tamanho_lista,&indice_dir);// Preenchimento doque vai ser o nó direito do meu novo nó.

                        if(((*esquerdo).peso < 0)||((*direito).peso < 0)){ //apenas condicional de parada caso aconteça algum erro, mas normalmente não será utilizada.
                            free(esquerdo);
                            free(direito);
                            break;
                        }

                        No_arvore *novo;
                        novo = (No_arvore *)malloc(sizeof(No_arvore)); //Alocação de memória para meu novo nó que será a união do esquerdo e do direito.

                        (*novo).esquerda = esquerdo;
                        (*novo).direita = direito;
                        (*novo).peso = (*novo).direita->peso + (*novo).esquerda->peso;
                        (*novo).indice = -1;

                        // Acima ocorre o preenchimento do meu nó de fusão entre os meus 2 nós de menores peso.

                        lista_frequencia[indice_dir] = *novo; //Substituição lógica do meu nó direito pelo novo nó que fundiu eles, para economizar um espaço no meu array. (substituição lógica funciona pq por estar encadeando os nós eu não perco a ligação com os nós filhos daquele nó que está sendo substituido, já que todos os seus filhos estão alocados atraves do encadeamento de nós na árvore).

                        raiz = novo; // Atualização da raíz da minha árvore.

                        nos_arvore++; // Incrementador da quantidade de nós fundidos.
                    }
                

                    percorrer_arvore(&raiz,traducao,0,buffer,profundidades); // Chamada da função para percorrer a árvore e já  criar a tabela de códigos.

                }

                char arquivo_saida[BUFFER_SIZE];
                strcpy(arquivo_saida,argv[2]);
                char *ponto = strrchr(arquivo_saida,'.');
                if(ponto != NULL) {
                    strcpy(ponto, ".ggc");  // Substitui .txt por .ggc
                }

                FILE *saida = fopen(arquivo_saida, "wb"); // Criação do novo arquivo compactado .ggc.

                if(!saida){ //Tratamento de erro básico caso falhe na abertura do arquivo.
                    printf("Erro ao abrir o arquivo: %s.\n",arquivo_saida);
                    exit(1);
                }

                escrita_cabecalho(tamanho_lista,extensao_arquivo,traducao,saida); //Chamada da função de escrita do cabeçalho de meu arquivo traduzido.

                escrita_corpo(extensao_arquivo,traducao,leitor,saida); //Chamada da função de escrita do corpo do texto no novo arquivo utilizando a lista de códigos e o texto original que está armazenado na string leitor.


                fclose(saida); // Fechamento do meu arquivo de saída já escrito.
                free(leitor); // Liberação de memória da minha string que continha meu texto original.
            }
            
        }
        else if(strcmp(tag,"-d") == 0){  // Verificamos se a tag que está sendo chamada é a de descompactação de arquivo, sendo ela o "-d".

            if(!verificar_extensao(argv[2],".ggc")){ // Verificamos se o arquivo a ser descompactado é da extensão ".ggc" que é a única permitida no nosso programa, sendo um antigo arquivo ".txt" que já passou por uma compactação do nosso programa.
                printf("\n");
                printf("Utilize uma extensão de arquivo adequada para utilização do programa de compactação, sendo ela \".ggc\";\n");
                printf("Exemplo: \"./manipulador -d texto.ggc\"\n");
                printf("\n");
                exit(1);
            }
            else{
                printf("Tamo descompactando!\n"); // <--------------------- JESUS SUA PARTE É AQUI
            }
        }
        else{  // Verificamos que a tag que está sendo chamada não corresponde a nenhuma aceita no nosso programa de manipulação de arquivos, sendo assim é mostrada uma mensagem de orientação abaixo.

            printf("\n");
            printf("Utilize a tag adequada para utilização do programa, sendo elas:\n");
            printf("\"-c\"-> Para compactação de arquivos cuja extenção é .txt - (Exemplo: \"./manipulador -c texto.txt\");\n");
            printf("\"-d\"-> Para descompactar um arquivo que já foi compactado nesse programa- (Exemplo: \"./manipulador -d compactado.ggc\");\n");
            printf("\n");
            exit(1);
        }
    }

    return 0;
}

int verificar_extensao(char *arquivo_verificado, char *comparador){ //Função básica apenas para verificação se o arquivo passado no argumento é igual ao que se deseja trabalhar.
    char *verificador = strrchr(arquivo_verificado,'.');
    if(verificador == NULL || (strcmp(verificador,comparador) != 0)){
        return 0;
    }
    
    return 1;
}

No_arvore pegar_menor(No_arvore *frequencia,int tamanho,int *indice){ // Função que percorre o array que contém os caracteres com seus pesos e retorna o endereço do menor elemento presente naquele vetor (Não ordenei ele para ficar mais fácil então ele não está otimizado, logo pode ser melhorado ainda).
    int menor = INT_MAX;
    int indice_menor = 0;
    for(int i = 0;i<tamanho;i++){
        if((frequencia[i].peso < menor )&&(frequencia[i].peso > 0)){ 
            menor = frequencia[i].peso;
            indice_menor = i;
        }
    }

    *indice = indice_menor;

    return frequencia[indice_menor];
}

void percorrer_arvore(No_arvore **raiz,char *traducao[],int profundidade,char *buffer,int *profundidades){ //Função que percorre a árvore de huffman que criei e já gera o seu código correspondente, preenchendo a minha lista de tradução.
    if(*raiz == NULL){
        return;
    }
    
    if((*raiz)->indice != -1){
        buffer[profundidade] = '\0';
        traducao[(*raiz)->indice] = strdup(buffer);
        profundidades[(*raiz)->indice] = profundidade;
        return;
    }

    buffer[profundidade] = '0';
    percorrer_arvore(&(*raiz)->esquerda,traducao,profundidade+1,buffer,profundidades);
    buffer[profundidade] = '1';
    percorrer_arvore(&(*raiz)->direita,traducao,profundidade+1,buffer,profundidades);

}

void escrita_cabecalho(int tamanho_lista,size_t extensao_arquivo,char *traducao[],FILE *saida){ //Função que escreve no cabeçalho do meu arquivo .ggc a minha lista de tradução para termos a referencia ao descompactar.
    unsigned char tamanho_lista_uc = (unsigned char)tamanho_lista; 

    fwrite(&extensao_arquivo, sizeof(extensao_arquivo), 1, saida);
    fwrite(&tamanho_lista_uc, sizeof(unsigned char), 1, saida);

    for(int i = 0; i < BUFFER_SIZE; i++){
        if(traducao[i] != NULL){
            unsigned char indice = (unsigned char)i;
            unsigned char tamanho_codigo = (unsigned char)strlen(traducao[i]);

            fwrite(&indice, sizeof(unsigned char), 1, saida);
            fwrite(&tamanho_codigo, sizeof(unsigned char), 1, saida);
            fwrite(traducao[i], sizeof(char), tamanho_codigo, saida);
        }
    }
}

void escrita_corpo(size_t extensao_arquivo,char *traducao[],unsigned char *leitor,FILE *saida){ // Função que traduz o meu texto original com base na minha lista de tradução e já escreve ele traduzido no arquivo .ggc logo abaixo do cabeçalho.
    unsigned char acumulador = 0;
    int bits_preenchidos = 0;

    for(size_t i = 0; i < extensao_arquivo; i++){
        char *codigo = traducao[leitor[i]];

        for(int j = 0; codigo[j] != '\0'; j++){
            if(codigo[j] == '1'){
                acumulador = acumulador | (1 << (7 - bits_preenchidos));
            }
            bits_preenchidos++;

            if(bits_preenchidos == 8){
                fwrite(&acumulador, sizeof(unsigned char), 1, saida);
                acumulador = 0;
                bits_preenchidos = 0;
            }
        }
    }   

    // Tratamento do último byte incompleto (se houver bits sobrando)
    if(bits_preenchidos > 0){
        fwrite(&acumulador, sizeof(unsigned char), 1, saida);
    }
}