/**
 * @file reader.c
 * @brief Implementações das funções para leitura de arquivos `.class` (JVM 8).
 *
 * Este arquivo contém as rotinas que encapsulam leituras de tipos primitivos
 * (`u1`, `u2`, `u4`), abertura de arquivos `.class` e a construção da estrutura
 * `ClassFile` a partir do conteúdo binário do arquivo. Também implementa a leitura
 * de membros (fields/methods) e atributos conforme a especificação da JVM 8.
 *
 * As funções fazem uso de utilitários de conversão de endianess (u2swap, u4swap)
 * definidos em outro módulo quando o sistema é little-endian.
 *
 * Observações de segurança/erro:
 * - Em caso de erro de formato (magic number inválido) a função `read_classfile`
 *   imprime mensagem em stderr e encerra o processo com EXIT_FAILURE.
 * - Funções que alocam memória verificam o retorno de calloc/malloc antes de usar.
 *
 * @see reader.h
 */

#include "reader.h"
#include <errno.h>

/**
 * @brief Lê 1 byte (`u1`) do arquivo `.class`.
 *
 * Wrapper simples sobre fread para padronizar leitura de 1 byte.
 *
 * @param fptr Ponteiro válido para um arquivo `.class` aberto em modo binário.
 * @return Valor lido como `u1`. Em caso de falha no fread o comportamento depende
 *         do estado do FILE* (valor retornado pode ser indeterminado).
 */
u1 read_u1(FILE *fptr)
{
    u1 u;
    fread(&u, sizeof(u1), 1, fptr);

    return u;
}

/**
 * @brief Lê 2 bytes (`u2`) do arquivo `.class` e ajusta endianess quando necessário.
 *
 * Se a plataforma for little-endian, os bytes são trocados para corresponder ao
 * formato big-endian do arquivo `.class`.
 *
 * @param fptr Ponteiro válido para um arquivo `.class`.
 * @return Valor lido como `u2` já com a ordenação correta para interpretação.
 * @see u2swap
 */
u2 read_u2(FILE *fptr)
{
    u2 u;
    fread(&u, sizeof(u2), 1, fptr);

    if (LittleEndian)
    {
        u = u2swap(u);
    }

    return u;
}

/**
 * @brief Lê 4 bytes (`u4`) do arquivo `.class` e ajusta endianess quando necessário.
 *
 * Semelhante a read_u2, mas para inteiros de 4 bytes.
 *
 * @param fptr Ponteiro válido para um arquivo `.class`.
 * @return Valor lido como `u4` já com a ordenação correta para interpretação.
 * @see u4swap
 */
u4 read_u4(FILE *fptr)
{
    u4 u;
    fread(&u, sizeof(u4), 1, fptr);

    if (LittleEndian)
    {
        u = u4swap(u);
    }

    return u;
}

/**
 * @brief Abre um arquivo `.class` para leitura binária após validação mínima do caminho.
 *
 * Verifica se a string de caminho tem ao menos 7 caracteres e termina com ".class".
 * Em caso de extensão incorreta a função imprime mensagem e encerra o processo.
 *
 * @param path Caminho para o arquivo `.class`.
 * @return Ponteiro `FILE*` aberto em modo "rb" caso válido; NULL se o caminho for inválido.
 */
FILE *open_classfile(const char *path)
{
    size_t l;

    if ((l = strlen(path)) > 6)
    {
        const char *ext = &path[l - 6];

        if (strcmp(ext, ".class") != 0)
        {
            fprintf(stderr, "Erro: formato de arquivo inválido. Forneça o caminho de um arquivo \".class\".\n");
            return NULL;
        }

        FILE *fptr = fopen(path, "rb");
        if (fptr == NULL)
        {
            fprintf(stderr, "Erro: não foi possível abrir o arquivo '%s'. ", path);
            if (errno == ENOENT)
                fprintf(stderr, "Arquivo não encontrado.\n");
            else if (errno == EACCES)
                fprintf(stderr, "Permissão negada.\n");
            else
                fprintf(stderr, "Erro: %s\n", strerror(errno));
        }
        return fptr;
    }

    fprintf(stderr, "Erro: caminho inválido.\n");
    return NULL;
}

/**
 * @brief Lê um arquivo `.class` inteiro e monta a estrutura `ClassFile`.
 *
 * Realiza as seguintes etapas:
 * - Calcula o tamanho do arquivo (opcionalmente imprime).
 * - Lê magic number e valida (0xCAFEBABE).
 * - Lê versões (minor/major).
 * - Lê constant pool via parse_constant_pool.
 * - Lê flags, this_class, super_class.
 * - Lê interfaces, fields, methods e atributos (usando read_member e read_attributes).
 * - Fecha o arquivo e retorna a estrutura `ClassFile`.
 *
 * @param fptr Ponteiro para o arquivo `.class` já aberto.
 * @param show_bytes Se verdadeiro, imprime o tamanho do arquivo e quantos bytes foram lidos.
 * @return Estrutura `ClassFile` preenchida com as informações do arquivo.
 *
 * @note Em caso de magic number inválido a função imprime erro em stderr, fecha o arquivo
 *       e termina o programa com EXIT_FAILURE.
 */
ClassFile read_classfile(FILE *fptr, bool show_bytes)
{
    ClassFile cf;

    if (fseek(fptr, 0, SEEK_END) == 0)
    {
        const size_t fsize = ftell(fptr);

        if (show_bytes)
            printf("Size: %u bytes.\n", fsize);
        fseek(fptr, 0, SEEK_SET);

        cf.magic = read_u4(fptr);

        if (cf.magic != 0xCAFEBABE)
        {
            fprintf(stderr, "Erro: arquivo .class inválido (magic number = 0x%08X, esperado 0xCAFEBABE).\n", cf.magic);
            fclose(fptr);
            exit(EXIT_FAILURE);
        }

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

        /* Class attributes */
        cf.attributes_count = read_u2(fptr);
        cf.attributes = (attribute *)calloc(cf.attributes_count, sizeof(attribute));
        if (cf.attributes != NULL)
            read_attributes(cf.constant_pool, cf.attributes_count, fptr, cf.attributes);
    }

    if (show_bytes)
        printf("%lu bytes readed.\n\n", ftell(fptr));
    fclose(fptr);
    return cf;
}

/**
 * @brief Lê um conjunto de membros (`field_info` ou `method_info`) do arquivo `.class`.
 *
 * A JVM utiliza a mesma estrutura básica para fields e methods no arquivo `.class`,
 * por isso esta função lê ambos populando um vetor de `member_info`.
 *
 * Para cada membro lê:
 * - access_flags (u2)
 * - name_index (u2)
 * - descriptor_index (u2)
 * - attributes_count (u2) e, se houver, aloca e lê os atributos com read_attributes.
 *
 * @param cp Ponteiro para o constant pool (usado na resolução de nomes/descriptors).
 * @param count Quantidade de membros a serem lidos.
 * @param info Ponteiro para um vetor previamente alocado de `member_info` com espaço para `count` elementos.
 * @param fptr Ponteiro para o arquivo `.class`.
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
 * @brief Lê um vetor de atributos (`attribute`) do arquivo `.class`.
 *
 * Para cada atributo lido:
 * - lê attribute_name_index e attribute_length;
 * - obtém o nome do atributo a partir do constant pool;
 * - determina o tipo do atributo via convert_attr_name;
 * - de acordo com o tipo, lê a estrutura específica (ex: Code, ConstantValue, Exceptions, InnerClasses);
 * - se o tipo não for tratado explicitamente, avança o cursor do arquivo por attribute_length bytes.
 *
 * Observações:
 * - A função tenta alocar memória para estruturas variáveis (code, exception_table, nested attributes) e,
 *   se falhar, avança no arquivo para manter o alinhamento.
 *
 * @param cp Ponteiro para o constant pool.
 * @param n Número de atributos a serem lidos.
 * @param fptr Ponteiro para o arquivo `.class`.
 * @param attr Vetor previamente alocado de `attribute` com espaço para `n` elementos.
 */
void read_attributes(const cp_info *cp, u2 n, FILE *fptr, attribute *attr)
{
    u2 c;

    if (attr != NULL)
        for (size_t i = 0; i < n; i++)
        {
            attr[i].attribute_name_index = read_u2(fptr);
            attr[i].attribute_length = read_u4(fptr);

            /* Recupera o nome do atributo a partir do constant pool (índices em ClassFile são 1-based). */
            const char *attr_name = cp[attr[i].attribute_name_index - 1].info.UTF8.str;

            /* Determina o tipo do atributo (enum attribute_name) a partir do nome. */
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
                        attr[i].info.Code.exception_table = (struct exception_table *)calloc(c, sizeof(struct exception_table));
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
                case SourceFile:
                    attr[i].info.SourceFile.sourcefile_index = read_u2(fptr);
                    break;
                case Exceptions:
                {
                    /* Estrutura:
                     * u2 number_of_exceptions;
                     * u2 exception_index_table[number_of_exceptions];
                     */
                    u2 n = read_u2(fptr);
                    attr[i].info.Exceptions.number_of_exceptions = n;

                    if (n > 0)
                    {
                        attr[i].info.Exceptions.exception_index_table = (u2 *)calloc(n, sizeof(u2));
                        if (attr[i].info.Exceptions.exception_index_table != NULL)
                        {
                            for (u2 k = 0; k < n; ++k)
                            {
                                attr[i].info.Exceptions.exception_index_table[k] = read_u2(fptr);
                            }
                        }
                        else
                        {
                            /* Falha de alocação: avance o arquivo para não desalinhar */
                            for (u2 k = 0; k < n; ++k)
                                (void)read_u2(fptr);
                        }
                    }
                    else
                    {
                        attr[i].info.Exceptions.exception_index_table = NULL;
                    }
                    break;
                }
                case InnerClasses:
                {
                    u2 n = read_u2(fptr);

                    attr[i].info.InnerClasses.number_of_classes = n;
                    if (n > 0)
                    {
                        attr[i].info.InnerClasses.classes = (struct classes *)calloc(n, sizeof(struct classes));
                        if (attr[i].info.InnerClasses.classes != NULL)
                            for (size_t j = 0; j < n; j++)
                            {
                                attr[i].info.InnerClasses.classes[j].inner_class_info_index = read_u2(fptr);
                                attr[i].info.InnerClasses.classes[j].outer_class_info_index = read_u2(fptr);
                                attr[i].info.InnerClasses.classes[j].inner_name_index = read_u2(fptr);
                                attr[i].info.InnerClasses.classes[j].inner_class_access_flags = read_u2(fptr);
                            }
                        else
                        {
                            /* Avança o cursor do arquivo caso não seja possível alocar */
                            fseek(fptr, n * sizeof(struct classes), SEEK_CUR);
                        }
                    }
                    break;
                }
                default:
                    /* Tipo de atributo não tratado explicitamente: pula os bytes do atributo. */
                    fseek(fptr, attr[i].attribute_length, SEEK_CUR);
                    break;
                }
        }
}
