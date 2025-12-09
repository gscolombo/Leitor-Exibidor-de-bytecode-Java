/**
 * @file members/methods.c
 * @brief Rotinas para exibição (dump) de métodos presentes em um ClassFile.
 *
 * Este módulo implementa a impressão legível das informações dos métodos
 * declarados em um arquivo `.class`, incluindo:
 *  - flags de acesso (formato ACC_* e palavras-chave Java);
 *  - nome, descritor, parâmetros e tipo de retorno (descritores parseados);
 *  - atributos do método (por exemplo, Code e Exceptions), com detalhe do bytecode
 *    (delegando para `show_opcodes` / `show_methods`).
 *
 * A implementação cuida de dividir o descritor em parte de parâmetros e retorno,
 * traduzir descritores para forma legível via `parse_descriptor`, e imprimir
 * atributos conhecidos com validação de índices na constant pool.
 */

#include "methods.h"
#include <string.h>
#include "types/cp/constants.h"
#include "types/attributes/attribute_info.h"
#include "types/attributes/attributes.h"
#include "bytecode/printer.h"

/**
 * @brief Mapeamento entre flags de método e nomes simbólicos (ACC_*).
 *
 * Usado por parse_flags() para gerar uma string com as flags setadas no método.
 */
static const FlagMap flag_map[12] = {
    {0x0001, "ACC_PUBLIC"},
    {0x0002, "ACC_PRIVATE"},
    {0x0004, "ACC_PROTECTED"},
    {0x0008, "ACC_STATIC"},
    {0x0010, "ACC_FINAL"},
    {0x0020, "ACC_SYNCHRONIZED"},
    {0x0040, "ACC_BRIDGE"},
    {0x0080, "ACC_VARARGS"},
    {0x0100, "ACC_NATIVE"},
    {0x0400, "ACC_ABSTRACT"},
    {0x0800, "ACC_STRICT"},
    {0x1000, "ACC_SYNTHETIC"}};

/**
 * @brief Mapeamento entre flags de método e palavras-chave Java.
 *
 * Produz uma versão legível próxima da sintaxe Java (ex.: "public static").
 */
static const FlagMap flag_kw_map[9] = {
    {0x0001, "public"},
    {0x0002, "private"},
    {0x0004, "protected"},
    {0x0008, "static"},
    {0x0010, "final"},
    {0x0020, "synchronized"},
    {0x0100, "native"},
    {0x0400, "abstract"},
    {0x0800, "strictfp"}};

/**
 * @brief Recupera o nome de classe (UTF8) apontado por um entry CONSTANT_Class na CP.
 *
 * Faz validações de limites e tipos na constant pool:
 *  - class_index deve apontar para um CONSTANT_Class;
 *  - o CONSTANT_Class deve apontar para um CONSTANT_UTF8 com o nome.
 *
 * @param cf Ponteiro para o ClassFile contendo a constant pool.
 * @param class_index Índice (1-based) na constant pool que aponta para CONSTANT_Class.
 * @return Ponteiro para a string UTF8 com o nome da classe no formato interno
 *         (ex.: "java/lang/Exception"), ou NULL em caso de índice/tipo inválido.
 */
static const char* cp_class_name(const ClassFile *cf, u2 class_index) {
    if (class_index < 1 || class_index > cf->constant_pool_count) return NULL;
    const cp_info *cp = cf->constant_pool;
    const cp_info *cls = &cp[class_index - 1];
    if (cls->tag != CONSTANT_Class) return NULL;
    u2 name_index = cls->info.Class.name_index;
    if (name_index < 1 || name_index > cf->constant_pool_count) return NULL;
    const cp_info *utf = &cp[name_index - 1];
    if (utf->tag != CONSTANT_UTF8) return NULL;
    return utf->info.UTF8.str; // formato internal (java/lang/Exception)
}

/**
 * @brief Exibe todos os métodos definidos no ClassFile.
 *
 * Para cada método, imprime:
 *  - modifiers (palavras-chave Java);
 *  - tipo de retorno parseado, nome do método e parâmetros parseados;
 *  - descritor bruto e flags;
 *  - atributos do método: `Exceptions` (lista de classes lançadas) e `Code`
 *    (imprime max_stack/max_locals/code_length e chama `show_opcodes` para o bytecode);
 *  - atributos desconhecidos são listados com nome e comprimento.
 *
 * A função realiza validações de índices na constant pool para evitar acessos inválidos,
 * aloca strings temporárias (params/retorno/flags) e as libera antes de avançar para
 * o próximo método.
 *
 * @param cf Ponteiro para o ClassFile a ser exibido. Se cf->constant_pool for NULL, a função retorna imediatamente.
 */
void show_methods(const ClassFile *cf)
{
    if (cf->constant_pool == NULL)
        return;

    cp_info *cp = cf->constant_pool;
    u2 count = cf->methods_count;
    char nl;

    if (count > 0 && cf->methods != NULL)
        for (size_t i = 0; i < count; i++)
        {
            nl = i == (size_t)(count - 1) ? ' ' : '\n';
            member_info method = cf->methods[i];

            u2 access_flags = method.access_flags;
            char *method_name = cp[method.name_index - 1].info.UTF8.str;
            char *method_desc = cp[method.descriptor_index - 1].info.UTF8.str;

            /* Trata método <init> se necessário (código comentado originalmente). */

            /* Calcula comprimento da parte de parâmetros e retorno no descritor */
            char *params_end = strchr(method_desc, L')') + 1;
            size_t params_desc_len = params_end - method_desc;
            size_t ret_desc_len = strlen(params_end);

            /* Aloca e copia as partes do descritor */
            char *params_desc = (char *)calloc(params_desc_len + 1, sizeof(char));
            if (params_desc)
            {
                strncpy(params_desc, method_desc, params_desc_len);
                params_desc[params_desc_len] = L'\0';
            }

            char *ret_desc = (char *)calloc(ret_desc_len + 1, sizeof(char));
            if (ret_desc)
            {
                strncpy(ret_desc, params_end, ret_desc_len);
                ret_desc[ret_desc_len] = L'\0';
            }

            /* Faz o parsing textual dos descritores */
            char *params_str = NULL, *ret_str = NULL;
            int is_init = !strcmp(method_name, "<init>");
            if (!is_init)
                ret_str = parse_descriptor(ret_desc, NULL);
            params_str = parse_descriptor(params_desc, ",");

            /* Converte flags para strings legíveis */
            char *flags = parse_flags(access_flags, 12, ", ", flag_map);
            char *kws = parse_flags(access_flags, 9, " ", flag_kw_map);

            printf(" %s %s%s%s;\n    descriptor: %s\n    flags: (0x%04x) %s\n",
                   kws, ret_str ? ret_str : "", method_name, params_str, method_desc, access_flags, flags);

            /* Impressão dos atributos do método (com validação do nome do atributo) */
            if (method.attributes_count > 0 && method.attributes != NULL) {
                attribute *attrs = method.attributes;
                cp_info *cp = cf->constant_pool;
                u2 cp_count = cf->constant_pool_count;

                for (u2 a = 0; a < method.attributes_count; ++a) {
                    const attribute *ai = &attrs[a];

                    /* Valida índice e tipo do nome do atributo na constant pool */
                    if (ai->attribute_name_index < 1 || ai->attribute_name_index > cp_count) {
                        printf("  [<invalid-attribute-name-index #%u>: length=%u]\n",
                            ai->attribute_name_index, ai->attribute_length);
                        continue;
                    }
                    if (cp[ai->attribute_name_index - 1].tag != CONSTANT_UTF8) {
                        printf("  [<non-utf8-attribute-name #%u>: length=%u]\n",
                            ai->attribute_name_index, ai->attribute_length);
                        continue;
                    }
                    const char *attr_name = cp[ai->attribute_name_index - 1].info.UTF8.str;
                    if (!attr_name) {
                        printf("  [<unknown-attribute>: length=%u]\n", ai->attribute_length);
                        continue;
                    }

                    /* Trata atributos conhecidos */
                    if (strcmp(attr_name, "Exceptions") == 0) {
                        u2 n = ai->info.Exceptions.number_of_exceptions;
                        u2 *tab = ai->info.Exceptions.exception_index_table;
                        if (n == 0 || tab == NULL) {
                            printf("  [Exceptions: none]\n");
                        } else {
                            printf("  [Exceptions:\n");
                            for (u2 k = 0; k < n; ++k) {
                                u2 exc_class_idx = tab[k];
                                const char *exc_name = cp_class_name(cf, exc_class_idx);
                                if (exc_name)
                                    printf("    %s\n", exc_name);
                                else
                                    printf("    <invalid class index #%u>\n", exc_class_idx);
                            }
                            printf("  ]\n");
                        }
                    }
                    else if (strcmp(attr_name, "Code") == 0) {
                        printf("    Code:\n");
                        printf("      max_stack: %u, max_locals: %u, code_length: %u\n",
                            ai->info.Code.max_stack,
                            ai->info.Code.max_locals,
                            ai->info.Code.code_length);

                        /* Mostra o bytecode detalhado delegando a show_opcodes */
                        show_opcodes(ai->info.Code.code, ai->info.Code.code_length, cp, cf->constant_pool_count);
                    }
                    else {
                        /* Para atributos não tratados, apenas liste nome e comprimento */
                        printf("  [%s: length=%u]\n", attr_name, ai->attribute_length);
                    }
                }
            }

            printf("%c", nl);

            /* Libera buffers alocados temporariamente */
            free(flags), free(kws);
            free(params_desc), free(ret_desc);
            free(params_str), free(ret_str);
        }
}
