/**
 * @file main.c
 * @brief Ponto de entrada do leitor-exibidor de bytecode Java.
 *
 * Este programa lê um arquivo `.class` da JVM, interpreta sua estrutura interna
 * (constant pool, fields, methods, atributos etc.) e exibe tudo em formato
 * legível ao usuário, de forma semelhante à ferramenta `javap`.
 *
 * O fluxo do programa é:
 *  1. Validar argumentos de linha de comando.
 *  2. Abrir o arquivo `.class` usando `open_classfile()`.
 *  3. Ler o conteúdo usando `read_classfile()`.
 *  4. Exibir a estrutura usando `show_classfile()`.
 *  5. Liberar todos os recursos com `free_classfile()`.
 */

#include "utils.h"
#include "reader.h"
#include "writer.h"

/**
 * @brief Função principal do programa leitor-exibidor de bytecode.
 *
 * Uso esperado:
 * ```
 * ./exibidor caminho/para/Arquivo.class
 * ```
 *
 * Comportamento:
 * - Se nenhum argumento for fornecido, exibe mensagem de erro.
 * - Caso o arquivo exista e seja válido, realiza:
 *      1. Abertura do arquivo `.class`.
 *      2. Leitura completa da estrutura ClassFile.
 *      3. Impressão detalhada da classe.
 *      4. Liberação completa de memória.
 *
 * @param argc Número de argumentos passados pela linha de comando.
 * @param argv Vetor de strings contendo os argumentos.
 * @return 0 em caso de sucesso, 1 se argumentos inválidos, ou outro valor em caso de erro.
 */
int main(const int argc, char *argv[]) {
    if (argc < 2) {
        printf("Invalid number of arguments. Pass the path for the .class file.\n");
        return 1;
    }

    FILE *fptr = open_classfile(argv[1]);
    
    if (fptr != NULL) {
        ClassFile cf = read_classfile(fptr);
        show_classfile(&cf);
        free_classfile(&cf);
    }

    return 0;
}
