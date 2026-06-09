#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// | Cabeçalho das funções que serão utilizadas durante a execução do programa |

int verificar_extensao(char *,char [4]);

// | ------------------------------------------------------------------------- |

int main(int argc,char *argv[]){ //Função main sendo chamada com os argumentos de verificação adequados e conforme a norma.

    if(argc != 3){ // Verificação se está sendo digitado corretamente a quantidade de argumentos na chamada da execução do terminal, se foi digitado um valor diferente do necessário acontece o tratamento de erro abaixo, e assim é mostrada essa mensagem de orientação.

        printf("\n");
        printf("Utilize a quantidade de argumentos correta para a execução devida do programa, sendo 3 argumentos e não %i como foi utilizado.\n",argc);
        printf("Exemplo: \"./manipulador -c texto.txt\"\n");
        printf("\n");    
        exit;
    }
    else{ // Passando pela validação da quantidade de argumentos agora vamos tratar as tags.

        char *tag = argv[1];
        if(strcmp(tag,"-c") == 0){ // Verificamos se a tag que está sendo chamada é a de compactação de arquivo, sendo ela o "-c".

            if(!verificar_extensao(argv[2],".txt")){ // Verificamos se o arquivo a ser compactado é da extensão ".txt" que é a única permitida no nosso programa.

                printf("\n");
                printf("Utilize uma extensão de arquivo adequada para utilização do programa de compactação, sendo ela \".txt\";\n");
                printf("Exemplo: \"./manipulador -c texto.txt\"\n");
                printf("\n");
                exit;
            }
            else{
                printf("Tamo compactando!\n");
            }
        }
        else if(strcmp(tag,"-d") == 0){  // Verificamos se a tag que está sendo chamada é a de descompactação de arquivo, sendo ela o "-d".

            if(!verificar_extensao(argv[2],".ggc")){ // Verificamos se o arquivo a ser descompactado é da extensão ".ggc" que é a única permitida no nosso programa, sendo um antigo arquivo ".txt" que já passou por uma compactação do nosso programa.
                printf("\n");
                printf("Utilize uma extensão de arquivo adequada para utilização do programa de compactação, sendo ela \".ggc\";\n");
                printf("Exemplo: \"./manipulador -c texto.ggc\"\n");
                printf("\n");
                exit;
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
            exit;
        }
    }

    return 0;
}

int verificar_extensao(char *arquivo_verificado, char comparador[4]){
    char *verificador = strrchr(arquivo_verificado,'.');
    if(verificador == NULL || (strcmp(verificador,comparador) != 0)){
        return 0;
    }
    
    return 1;
}