/**
 * @file src/members/fields.c
 * @brief Implementação das rotinas de exibição (printing) dos campos (fields) de um ClassFile.
 *
 * Este módulo imprime as informações de cada campo definido na classe,
 * incluindo:
 *  - flags de acesso (ex.: public, static, final)
 *  - nome do campo (resolvido via constant pool)
 *  - descritor JVM (ex.: I, Ljava/lang/String;)
 *  - tipo convertido para forma legível (ex.: int, java.lang.String)
 *  - atributos associados (não exibidos aqui, apenas a contagem e flags)
 *
 * A saída é semelhante à produzida pelo `javap`, formatada para facilitar leitura.
 */

#include "fields.h"

/**
 * @brief Mapa de flags da JVM para campos, com nomes oficiais ACC_*.
 *
 * Usado por parse_flags() para gerar a linha:
 *   flags: (0xXXXX) ACC_PUBLIC, ACC_STATIC, ...
 */
static const FlagMap flag_map[9] = {
    {0x0001, "ACC_PUBLIC"},
    {0x0002, "ACC_PRIVATE"},
    {0x0004, "ACC_PROTECTED"},
    {0x0008, "ACC_STATIC"},
    {0x0010, "ACC_FINAL"},
    {0x0040, "ACC_VOLATILE"},
    {0x0080, "ACC_TRANSIENT"},
    {0x1000, "ACC_SYNTHETIC"},
    {0x4000, "ACC_ENUM"}
};

/**
 * @brief Mapa de flags convertidas para palavras-chave Java.
 *
 * Usado para gerar o prefixo da linha do field, como:
 *   "public static final int contador;"
 */
static const FlagMap flag_kw_map[7] = {
    {0x0001, "public"},
    {0x0002, "private"},
    {0x0004, "protected"},
    {0x0008, "static"},
    {0x0010, "final"},
    {0x0040, "volatile"},
    {0x0080, "transient"}
};

/**
 * @brief Exibe todos os campos (fields) de um ClassFile.
 *
 * Para cada field, imprime algo como:
 *
 * ```
 * public static int contador;
 *     descriptor: I
 *     flags: (0x0009) ACC_PUBLIC, ACC_STATIC
 * ```
 *
 * Passos realizados:
 *  1. Verifica se o constant pool existe.
 *  2. Percorre `cf->fields_count`.
 *  3. Resolve o nome do campo via constant pool.
 *  4. Resolve o descritor JVM e converte para forma legível com parse_descriptor().
 *  5. Converte flags de acesso em:
 *      - nomes completos (ACC_PUBLIC)
 *      - palavras-chave (public, static, ...)
 *  6. Imprime a saída formatada.
 *
 * Observações:
 * - `parse_descriptor()` e `parse_flags()` alocam memória, que é liberada aqui.
 * - Esta função apenas exibe; não altera `cf` nem libera recursos internos dele.
 *
 * @param cf Ponteiro constante para a estrutura ClassFile contendo os fields.
 */
void show_fields(const ClassFile *cf)
{
    if (cf->constant_pool == NULL)
        return;

    cp_info *cp = cf->constant_pool;
    u2 count = cf->fields_count;

    char nl;

    if (count > 0 && cf->fields != NULL)
        for (size_t i = 0; i < count; i++)
        {
            /* Determina se há quebra de linha entre fields */
            nl = i == (size_t)(count - 1) ? ' ' : '\n';

            member_info field = cf->fields[i];

            /* Índices para nome e descritor no constant pool */
            u2 access_flags = field.access_flags;
            wchar_t *field_name = cp[field.name_index - 1].info.UTF8.str;
            wchar_t *field_desc = cp[field.descriptor_index - 1].info.UTF8.str;

            /* Converte descritor JVM ("I", "Ljava/lang/String;") para forma legível ("int", "java.lang.String") */
            wchar_t *field_desc_str = parse_descriptor(field_desc, NULL);

            /* Converte flags para string completa (ACC_PUBLIC, ...) */
            char *flags = parse_flags(access_flags, 9, ", ", flag_map);

            /* Converte flags para palavras-chave Java (public, static, ...) */
            char *kws = parse_flags(access_flags, 7, " ", flag_kw_map);

            printf(" %s %ls%ls;\n    descriptor: %ls\n    flags: (0x%04x) %s\n%c",
                   kws, field_desc_str, field_name, field_desc,
                   access_flags, flags, nl);

            /* Libera memória alocada por funções auxiliares */
            free(flags);
            free(field_desc_str);
            free(kws);
        }
}
