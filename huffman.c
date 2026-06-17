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

                if(!leitor_arquivo_txt){
                    printf("Erro ao abrir o arquivo: %s.\n",argv[2]);
                    exit(1);
                }
                
                fseek(leitor_arquivo_txt,0,SEEK_END);
                long tamanho_arquivo = ftell(leitor_arquivo_txt); 
                fseek(leitor_arquivo_txt,0,SEEK_SET);

                unsigned char *leitor = malloc(tamanho_arquivo); 

                if(!leitor){
                    printf("Erro ao alocar memória.\n");
                    fclose(leitor_arquivo_txt);
                    exit(1);
                }

                size_t extensao_arquivo = fread(leitor,1,tamanho_arquivo,leitor_arquivo_txt);
                
                int contador_caracteres[BUFFER_SIZE]={0};

                for(size_t i = 0; i < extensao_arquivo;i++){
                    contador_caracteres[leitor[i]]++;
                }

                fclose(leitor_arquivo_txt);

                int tamanho_lista=0;
                No_arvore lista_frequencia[BUFFER_SIZE+1] = {0};

                for(int i = 0;i < BUFFER_SIZE;i++){
                    if(contador_caracteres[i] != 0){
                        lista_frequencia[tamanho_lista].indice = i;
                        lista_frequencia[tamanho_lista].peso = contador_caracteres[i];

                        tamanho_lista++;
                    }
                }

                int nos_arvore = 0;

                No_arvore *raiz = NULL;
                while(nos_arvore < (tamanho_lista-1)){
                    int indice_esq = 0,indice_dir = 0;
                    No_arvore *esquerdo = (No_arvore *)malloc(sizeof(No_arvore));
                    *esquerdo = pegar_menor(lista_frequencia,tamanho_lista,&indice_esq);
                    lista_frequencia[indice_esq].peso = -1;
                    No_arvore *direito = (No_arvore *)malloc(sizeof(No_arvore));
                    *direito = pegar_menor(lista_frequencia,tamanho_lista,&indice_dir);

                    if(((*esquerdo).peso < 0)||((*direito).peso < 0)){
                        free(esquerdo);
                        free(direito);
                        break;
                    }

                    No_arvore *novo;
                    novo = (No_arvore *)malloc(sizeof(No_arvore));

                    (*novo).esquerda = esquerdo;
                    (*novo).direita = direito;
                    (*novo).peso = (*novo).direita->peso + (*novo).esquerda->peso;
                    (*novo).indice = -1;

                    lista_frequencia[indice_dir] = *novo;

                    raiz = novo;

                    nos_arvore++;
                }
                char *traducao[BUFFER_SIZE] = {NULL};
                char buffer[BUFFER_SIZE];
                int profundidades[BUFFER_SIZE] = {0};

                percorrer_arvore(&raiz,traducao,0,buffer,profundidades);

                char arquivo_saida[BUFFER_SIZE];
                strcpy(arquivo_saida,argv[2]);
                char *ponto = strrchr(arquivo_saida,'.');
                if(ponto != NULL) {
                    strcpy(ponto, ".ggc");  // Substitui .txt por .ggc
                }

                FILE *saida = fopen(arquivo_saida, "wb");

                if(!saida){
                    printf("Erro ao abrir o arquivo: %s.\n",arquivo_saida);
                    exit(1);
                }

                escrita_cabecalho(tamanho_lista,extensao_arquivo,traducao,saida);

                escrita_corpo(extensao_arquivo,traducao,leitor,saida);


                fclose(saida);
                free(leitor);
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


            FILE *leitor_ggc = fopen(argv[2], "rb");
            if (!leitor_ggc) {
                printf("Erro ao abrir o arquivo: %s.\n", argv[2]);
                exit(1);
            }

            // Lê tamanho original e quantidade de símbolos
            size_t tamanho_original = 0;
            unsigned char qtd_simbolos = 0;
            fread(&tamanho_original, sizeof(size_t), 1, leitor_ggc);
            fread(&qtd_simbolos, sizeof(unsigned char), 1, leitor_ggc);

            // Reconstrói a tabela de tradução (código binário -> caractere)
            char *traducao[BUFFER_SIZE] = {NULL};
            for (int i = 0; i < (int)qtd_simbolos; i++) {
                unsigned char indice, tamanho_codigo;
                fread(&indice, sizeof(unsigned char), 1, leitor_ggc);
                fread(&tamanho_codigo, sizeof(unsigned char), 1, leitor_ggc);

                char *codigo = malloc(tamanho_codigo + 1);
                fread(codigo, sizeof(char), tamanho_codigo, leitor_ggc);
                codigo[tamanho_codigo] = '\0';
                traducao[indice] = codigo;
            }

            // Lê o corpo compactado inteiro
            long pos_corpo = ftell(leitor_ggc);
            fseek(leitor_ggc, 0, SEEK_END);
            long tamanho_corpo = ftell(leitor_ggc) - pos_corpo;
            fseek(leitor_ggc, pos_corpo, SEEK_SET);

            unsigned char *corpo = malloc(tamanho_corpo);
            if (!corpo) {
                printf("Erro ao alocar memória.\n");
                fclose(leitor_ggc);
                exit(1);
            }
            fread(corpo, 1, tamanho_corpo, leitor_ggc);
            fclose(leitor_ggc);

            // Reconstrói a árvore de Huffman a partir da tabela
            No_arvore *raiz = (No_arvore *)calloc(1, sizeof(No_arvore));
            raiz->indice = -1;

            for (int i = 0; i < BUFFER_SIZE; i++) {
                if (traducao[i] == NULL) continue;

                No_arvore *atual = raiz;
                char *codigo = traducao[i];

                for (int j = 0; codigo[j] != '\0'; j++) {
                    if (codigo[j] == '0') {
                        if (atual->esquerda == NULL) {
                            atual->esquerda = (No_arvore *)calloc(1, sizeof(No_arvore));
                            atual->esquerda->indice = -1;
                        }
                        atual = atual->esquerda;
                    } else {
                        if (atual->direita == NULL) {
                            atual->direita = (No_arvore *)calloc(1, sizeof(No_arvore));
                            atual->direita->indice = -1;
                        }
                        atual = atual->direita;
                    }
                }
                atual->indice = i; // Marca a folha com o caractere
            }

            // Monta o nome do arquivo de saída (.txt)
            char arquivo_saida[BUFFER_SIZE];
            strcpy(arquivo_saida, argv[2]);
            char *ponto = strrchr(arquivo_saida, '.');
            if (ponto != NULL) {
                strcpy(ponto, ".txt");
            }

            FILE *saida = fopen(arquivo_saida, "wb");
            if (!saida) {
                printf("Erro ao criar o arquivo: %s.\n", arquivo_saida);
                exit(1);
            }

            // Percorre os bits do corpo e navega na árvore para recuperar os caracteres
            No_arvore *atual = raiz;
            size_t chars_escritos = 0;

            for (long b = 0; b < tamanho_corpo && chars_escritos < tamanho_original; b++) {
                unsigned char byte = corpo[b];
                for (int bit = 7; bit >= 0 && chars_escritos < tamanho_original; bit--) {
                    if ((byte >> bit) & 1)
                        atual = atual->direita;
                    else
                        atual = atual->esquerda;

                    if (atual->esquerda == NULL && atual->direita == NULL) {
                        unsigned char c = (unsigned char)atual->indice;
                        fwrite(&c, sizeof(unsigned char), 1, saida);
                        chars_escritos++;
                        atual = raiz; // Volta para a raiz
                    }
                }
            }

            fclose(saida);
            free(corpo);

            // Libera a tabela
            for (int i = 0; i < BUFFER_SIZE; i++) {
                if (traducao[i]) free(traducao[i]);
            }

            printf("Arquivo descompactado com sucesso: %s\n", arquivo_saida);
        }
     //--------------------- TERMINEI DE DESCOMPACTAR AQUI
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

int verificar_extensao(char *arquivo_verificado, char *comparador){
    char *verificador = strrchr(arquivo_verificado,'.');
    if(verificador == NULL || (strcmp(verificador,comparador) != 0)){
        return 0;
    }
    
    return 1;
}

No_arvore pegar_menor(No_arvore *frequencia,int tamanho,int *indice){
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

void percorrer_arvore(No_arvore **raiz,char *traducao[],int profundidade,char *buffer,int *profundidades){
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

void escrita_cabecalho(int tamanho_lista,size_t extensao_arquivo,char *traducao[],FILE *saida){
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

void escrita_corpo(size_t extensao_arquivo,char *traducao[],unsigned char *leitor,FILE *saida){
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