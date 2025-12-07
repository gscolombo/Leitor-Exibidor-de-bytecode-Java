/**
 * @file src/members/methods.c
 * @brief Implementação das rotinas de exibição dos métodos definidos em um ClassFile.
 *
 * Este módulo imprime informações sobre cada método (member_info) de um ClassFile,
 * incluindo:
 *  - modificadores (public, private, static, synchronized, etc.)
 *  - nome do método (incluindo substituição de `<init>` pelo nome da classe)
 *  - tipos de parâmetros e tipo de retorno convertidos para formato legível
 *  - descritor JVM original
 *  - flags ACC_* correspondentes
 *
 * A saída é semelhante à produzida pelo `javap`.
 */

#include "methods.h"

/**
 * @brief Mapa de flags JVM (ACC_*) para métodos.
 *
 * Usado por parse_flags() para gerar uma linha como:
 *    flags: (0xXXXX) ACC_PUBLIC, ACC_STATIC, ACC_NATIVE
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
    {0x1000, "ACC_SYNTHETIC"}
};

/**
 * @brief Mapa de flags JVM convertidas para palavras-chave Java.
 *
 * Usado na primeira linha da exibição para gerar algo como:
 *    "public static void exemplo(int, float);"
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
    {0x0800, "strictfp"}
};

/**
 * @brief Exibe todos os métodos definidos em um ClassFile.
 *
 * Para cada método, esta função:
 *  - Recupera nome e descritor no constant pool
 *  - Trata o método `<init>` (construtor), substituindo seu nome pelo nome da classe
 *  - Separa a lista de parâmetros e o tipo de retorno
 *  - Converte descritores JVM para texto legível via parse_descriptor()
 *  - Converte flags ACC_* em texto via parse_flags()
 *
 * Exemplo de saída:
 *
 * ```
 * public void exemplo(int, float);
 *     descriptor: (IF)V
 *     flags: (0x0001) ACC_PUBLIC
 * ```
 *
 * A função não altera o conteúdo original do `ClassFile` e libera toda memória
 * auxiliar alocada dinamicamente durante o processo.
 *
 * @param cf Ponteiro para a estrutura ClassFile contendo os métodos.
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
            /* Último método não imprime newline extra */
            nl = i == (size_t)(count - 1) ? ' ' : '\n';

            member_info method = cf->methods[i];
            u2 access_flags = method.access_flags;

            wchar_t *method_name = cp[method.name_index - 1].info.UTF8.str;
            wchar_t *method_desc = cp[method.descriptor_index - 1].info.UTF8.str;

            /* -------------------------------------------------------------------
             *  Tratamento especial para <init>
             * -------------------------------------------------------------------
             * Em Java, o construtor é mostrado como o nome da classe
             * em vez de "<init>". Aqui fazemos essa substituição.
             */
            int is_init = !wcscmp(method_name, L"<init>");
            if (is_init)
            {
                /* Recupera o nome da classe (this_class → CONSTANT_Class → Utf8) */
                wchar_t *classname = cp[cp[cf->this_class - 1].info.Class.name_index - 1].info.UTF8.str;

                /* Converte primeiro '/' encontrado em '.' */
                for (wchar_t *wcptr = classname; wcptr < classname + wcslen(classname) - 1; wcptr++)
                    if (*wcptr == L'/')
                    {
                        *wcptr = L'.';
                        break;
                    }

                method_name = classname;
            }

            /* -------------------------------------------------------------------
             *  Separação do descritor em parâmetros e tipo de retorno
             * -------------------------------------------------------------------
             */
            wchar_t *params_end = wcschr(method_desc, L')') + 1;
            size_t params_desc_len = params_end - method_desc;
            size_t ret_desc_len = wcslen(params_end);

            /* Copia substring dos parâmetros */
            wchar_t *params_desc = (wchar_t *)calloc(params_desc_len + 1, sizeof(wchar_t));
            if (params_desc)
            {
                wcsncpy(params_desc, method_desc, params_desc_len);
                params_desc[params_desc_len] = L'\0';
            }

            /* Copia substring do tipo de retorno */
            wchar_t *ret_desc = (wchar_t *)calloc(ret_desc_len + 1, sizeof(wchar_t));
            if (ret_desc)
            {
                wcsncpy(ret_desc, params_end, ret_desc_len);
                ret_desc[ret_desc_len] = L'\0';
            }

            /* Converte descritores */
            wchar_t *params_str = parse_descriptor(params_desc, L",");
            wchar_t *ret_str = NULL;
            if (!is_init)  /* Construtores não têm tipo de retorno */
                ret_str = parse_descriptor(ret_desc, NULL);

            /* Converte flags */
            char *flags = parse_flags(access_flags, 12, ", ", flag_map);
            char *kws   = parse_flags(access_flags, 9, " ", flag_kw_map);

            /* -------------------------------------------------------------------
             *  Impressão final
             * -------------------------------------------------------------------
             */
            printf(" %s %ls%ls%ls;\n    descriptor: %ls\n    flags: (0x%04x) %s\n%c",
                   kws,
                   ret_str ? ret_str : L"",  /* tipo de retorno */
                   method_name,               /* nome do método */
                   params_str,                /* lista de parâmetros */
                   method_desc,               /* descritor bruto original */
                   access_flags,
                   flags,
                   nl);

            /* Liberação de memória alocada dinamicamente */
            free(flags);
            free(kws);
            free(params_desc);
            free(ret_desc);
            free(params_str);
            free(ret_str);
        }
}
