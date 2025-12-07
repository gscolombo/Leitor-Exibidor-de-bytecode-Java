/**
 * @file reader.c
 * @brief Implementação das funções de leitura de arquivos .class e utilitários de leitura.
 *
 * Implementa leitor de inteiros em big-endian (u1, u2, u4), abertura de arquivos
 * .class e a leitura completa da estrutura ClassFile, além de helpers para
 * leitura de member_info e atributos.
 *
 * Observações:
 * - As funções que alocam memória (por exemplo, ao ler arrays, constant pool,
 *   attributes) deixam a responsabilidade de liberar essa memória para o
 *   chamador, normalmente através de free_classfile() ou funções auxiliares.
 * - O código assume que o arquivo passado está em formato .class válido.
 */

#include "reader.h"

/**
 * @brief Lê 1 byte (u1) do arquivo.
 *
 * Faz uma leitura direta de 1 byte do stream `fptr`.
 *
 * @param fptr Ponteiro para o FILE de onde será lido o byte.
 * @return u1 Valor lido (0..255). Se a leitura falhar, o valor retornado é indeterminado
 *         (seguindo o comportamento de fread).
 */
u1 read_u1(FILE *fptr)
{
    u1 u;
    fread(&u, sizeof(u1), 1, fptr);

    return u;
}

/**
 * @brief Lê 2 bytes (u2) do arquivo em ordem big-endian.
 *
 * Lê 2 bytes do stream e, se a arquitetura for little-endian, faz o swap de byte
 * para retornar o valor corretamente em u2.
 *
 * @param fptr Ponteiro para o FILE de onde serão lidos os 2 bytes.
 * @return u2 Valor 16-bit lido em ordem big-endian.
 */
u2 read_u2(FILE *fptr)
{
    u2 u;
    fread(&u, sizeof(u2), 1, fptr);

    if (LE)
    {
        u = u2swap(u);
    }

    return u;
}

/**
 * @brief Lê 4 bytes (u4) do arquivo em ordem big-endian.
 *
 * Lê 4 bytes do stream e, se a arquitetura for little-endian, faz o swap de byte
 * para retornar o valor corretamente em u4.
 *
 * @param fptr Ponteiro para o FILE de onde serão lidos os 4 bytes.
 * @return u4 Valor 32-bit lido em ordem big-endian.
 */
u4 read_u4(FILE *fptr)
{
    u4 u;
    fread(&u, sizeof(u4), 1, fptr);

    if (LE)
    {
        u = u4swap(u);
    }

    return u;
}

/**
 * @brief Abre um arquivo .class em modo binário.
 *
 * Verifica se o caminho termina com a extensão ".class" e tenta abrir o arquivo
 * em leitura binária ("rb"). Em caso de caminho inválido ou extensão incorreta,
 * imprime uma mensagem e retorna NULL.
 *
 * @param path Caminho para o arquivo que se deseja abrir.
 * @return FILE* Ponteiro para o arquivo aberto em modo binário, ou NULL em erro.
 */
FILE *open_classfile(const char *path)
{
    size_t l;

    if ((l = strlen(path)) > 6)
    {
        const char *ext = &path[l - 6];

        if (strcmp(ext, ".class") != 0)
        {
            printf("Wrong file format. Give the path of a \".class\" file.\n");
            return NULL;
        }

        return fopen(path, "rb");
    }

    printf("Invalid path.\n");
    return NULL;
}

/**
 * @brief Lê um arquivo .class inteiro e popula uma estrutura ClassFile.
 *
 * A função procura o tamanho do arquivo, imprime-o, reposiciona o ponteiro e
 * realiza a leitura sequencial dos componentes do classfile:
 * - magic, versões
 * - constant pool (via parse_constant_pool)
 * - access flags, this_class, super_class
 * - interfaces (aloca e preenche se necessário)
 * - fields (aloca e chama read_member)
 * - methods (aloca e chama read_member)
 * - attributes_count (lê o valor; atributos de classe podem ser lidos por outra função)
 *
 * Observações importantes:
 * - A função fecha o FILE passado (fclose) antes de retornar.
 * - O chamador é responsável por liberar a memória alocada dentro do ClassFile
 *   (constant_pool, arrays, atributos) usando free_classfile().
 *
 * @param fptr Ponteiro para FILE já aberto (deve ser um .class).
 * @return ClassFile Estrutura preenchida com os dados lidos. Campos não inicializados
 *         podem ocorrer se houver erro de leitura; é recomendado usar free_classfile
 *         após o uso.
 */
ClassFile read_classfile(FILE *fptr)
{
    ClassFile cf;

    if (fseek(fptr, 0, SEEK_END) == 0)
    {
        const size_t fsize = ftell(fptr);

        printf("Size: %lu bytes.\n", fsize);
        fseek(fptr, 0, SEEK_SET);

        cf.magic = read_u4(fptr);
        cf.minor_version = read_u2(fptr);
        cf.major_version = read_u2(fptr);
        cf.constant_pool_count = read_u2(fptr);
        cf.constant_pool = parse_constant_pool(fptr, cf.constant_pool_count);
        cf.access_flags = read_u2(fptr);
        cf.this_class = read_u2(fptr);
        cf.super_class = read_u2(fptr);

        /* Interfaces */
        cf.interfaces_count = read_u2(fptr);
        cf.interfaces = NULL;

        if (cf.interfaces_count > 0)
        {
            cf.interfaces = (u2 *)calloc(cf.interfaces_count, sizeof(u2));
            for (size_t i = 0; i < cf.interfaces_count; i++)
            {
                cf.interfaces[i] = read_u2(fptr);
            }
        }

        /* Fields */
        cf.fields_count = read_u2(fptr);
        cf.fields = (member_info *)calloc(cf.fields_count, sizeof(member_info));
        if (cf.fields != NULL)
            read_member(cf.constant_pool, cf.fields_count, cf.fields, fptr);

        /* Methods */
        cf.methods_count = read_u2(fptr);
        cf.methods = (member_info *)calloc(cf.methods_count, sizeof(member_info));
        if (cf.methods != NULL)
            read_member(cf.constant_pool, cf.methods_count, cf.methods, fptr);

        cf.attributes_count = read_u2(fptr);
    }

    printf("%lu bytes readed.\n\n", ftell(fptr));
    fclose(fptr);
    return cf;
}

/**
 * @brief Lê uma lista de member_info (fields ou methods) do arquivo e preenche o array.
 *
 * Para cada member, a função lê:
 * - access_flags (u2)
 * - name_index (u2)
 * - descriptor_index (u2)
 * - attributes_count (u2) e, se > 0, aloca e lê os atributos via read_attributes()
 *
 * @param cp Ponteiro para o constant pool (cp_info *) associado ao classfile.
 * @param count Número de elementos (member_info) a serem lidos.
 * @param info Ponteiro para o primeiro elemento de um array de member_info já alocado.
 * @param fptr Ponteiro para o FILE de onde os dados serão lidos.
 */
void read_member(const cp_info *cp, u2 count, member_info *info, FILE *fptr)
{
    u2 attr_count;

    for (size_t i = 0; i < count; i++)
    {
        info[i].access_flags = read_u2(fptr);
        info[i].name_index = read_u2(fptr);
        info[i].descriptor_index = read_u2(fptr);
        info[i].attributes_count = attr_count = read_u2(fptr);

        if (attr_count > 0)
        {
            info[i].attributes = (attribute *)calloc(attr_count, sizeof(attribute));
            if (info[i].attributes != NULL)
                read_attributes(cp, attr_count, fptr, info[i].attributes);
        }
    }
}

/**
 * @brief Lê um array de atributos a partir do arquivo e preenche a estrutura `attribute`.
 *
 * Para cada atributo, a função:
 * - lê attribute_name_index (u2) e attribute_length (u4)
 * - obtém o nome do atributo a partir do constant pool e converte para o tipo de atributo
 *   (via convert_attr_name)
 * - de acordo com o tipo (ex: ConstantValue, Code, LineNumberTable) faz leituras específicas
 *   e aloca buffers (code, exception_table, sub-attributes) conforme necessário.
 *
 * Observações:
 * - Esta função acessa campos específicos do union `attribute.info` com base no tipo.
 * - Caso o atributo possua sub-estruturas alocadas, estas ficam sob responsabilidade do
 *   chamador liberar posteriormente (por exemplo, através de free_attributes).
 *
 * @param cp Ponteiro para o constant pool (cp_info *) para resolução de nomes.
 * @param n Número de atributos a serem lidos.
 * @param fptr Ponteiro para o FILE de onde os atributos serão lidos.
 * @param attr Ponteiro para o array de `attribute` previamente alocado que será preenchido.
 */
void read_attributes(const cp_info *cp, u2 n, FILE *fptr, attribute *attr)
{
    u2 c;

    if (attr != NULL)
        for (size_t i = 0; i < n; i++)
        {
            attr[i].attribute_name_index = read_u2(fptr);
            attr[i].attribute_length = read_u4(fptr);

            const wchar_t *attr_name = cp[attr[i].attribute_name_index - 1].info.UTF8.str;

            const attribute_name *attr_type = convert_attr_name(attr_name);

            if (attr_type != NULL)
                switch (*attr_type)
                {
                case ConstantValue:
                    attr[i].info.ConstantValue.constantvalue_index = read_u2(fptr);
                    break;
                case Code:
                    attr[i].info.Code.max_stack = read_u2(fptr);
                    attr[i].info.Code.max_locals = read_u2(fptr);
                    c = attr[i].info.Code.code_length = read_u4(fptr);
                    attr[i].info.Code.code = NULL;

                    if (c > 0)
                    {
                        attr[i].info.Code.code = (u1 *)malloc(c);
                        if (attr[i].info.Code.code != NULL)
                            fread(attr[i].info.Code.code, sizeof(u1), c, fptr);
                    }

                    c = attr[i].info.Code.exception_table_length = read_u2(fptr);
                    attr[i].info.Code.exception_table = NULL;

                    if (c > 0)
                    {
                        attr[i].info.Code.exception_table = (exception_table *)calloc(c, sizeof(exception_table));
                        if (attr[i].info.Code.exception_table != NULL)
                            for (size_t j = 0; j < c; j++)
                            {
                                attr[i].info.Code.exception_table[j].start_pc = read_u2(fptr);
                                attr[i].info.Code.exception_table[j].end_pc = read_u2(fptr);
                                attr[i].info.Code.exception_table[j].handler_pc = read_u2(fptr);
                                attr[i].info.Code.exception_table[j].catch_type = read_u2(fptr);
                            }
                    }

                    c = attr[i].info.Code.attributes_count = read_u2(fptr);
                    attr[i].info.Code.attributes = NULL;

                    if (c > 0)
                    {
                        attr[i].info.Code.attributes = (attribute *)calloc(c, sizeof(attribute));
                        if (attr[i].info.Code.attributes != NULL)
                            read_attributes(cp, c, fptr, attr[i].info.Code.attributes);
                    }
                    break;
                case LineNumberTable:
                    c = attr[i].info.LineNumberTable.line_number_table_length = read_u2(fptr);
                    attr[i].info.LineNumberTable.line_number_table = NULL;

                    if (c > 0)
                    {
                        attr[i].info.LineNumberTable.line_number_table = (line_number_table *)calloc(c, sizeof(line_number_table));
                        if (attr[i].info.LineNumberTable.line_number_table != NULL)
                            for (size_t j = 0; j < c; j++)
                            {
                                attr[i].info.LineNumberTable.line_number_table[j].start_pc = read_u2(fptr);
                                attr[i].info.LineNumberTable.line_number_table[j].line_number = read_u2(fptr);
                            }
                    }
                    break;
                default:
                    break;
                }
        }
}
