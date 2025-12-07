/**
 * @file writer.c
 * @brief Implementação das rotinas de exibição (impressão) de estruturas ClassFile.
 *
 * Este módulo contém a lógica responsável por exibir em stdout informações formatadas
 * sobre um arquivo .class já carregado em memória. Ele mostra:
 *  - nome da classe e da superclasse
 *  - flags de acesso em formato textual (tanto nomes JVM quanto palavras-chave)
 *  - magic number, versão
 *  - constant pool
 *  - fields e methods
 *
 * O objetivo é oferecer uma representação amigável do conteúdo do ClassFile,
 * similar às ferramentas tradicionais como `javap`.
 */

#include "writer.h"

/**
 * @brief Tabela de flags JVM para classes (nomes oficiais ACC_).
 *
 * Esta tabela é usada por parse_flags() para montar uma string contendo
 * as flags encontradas no campo `access_flags` do ClassFile.
 */
static const FlagMap class_flag_map[8] = {
    {0x0001, "ACC_PUBLIC"},
    {0x0010, "ACC_FINAL"},
    {0x0020, "ACC_SUPER"},
    {0x0200, "ACC_INTERFACE"},
    {0x0400, "ACC_ABSTRACT"},
    {0x1000, "ACC_SYNTHETIC"},
    {0x2000, "ACC_ANNOTATION"},
    {0x4000, "ACC_ENUM"}
};

/**
 * @brief Tabela de flags JVM mapeadas para palavras-chave Java.
 *
 * Usada para exibir a linha inicial:
 *     "<flags> class <NomeDaClasse>"
 *
 * Exemplo:
 *   - ACC_PUBLIC → "public"
 *   - ACC_FINAL → "final"
 *   - ACC_INTERFACE → "interface"
 */
static const FlagMap class_flag_kw_map[6] = {
    {0x0001, "public"},
    {0x0010, "final"},
    {0x0200, "interface"},
    {0x0400, "abstract"},
    {0x2000, "@interface"},
    {0x4000, "enum"}
};

/**
 * @brief Exibe em stdout todas as informações relevantes de um ClassFile.
 *
 * A função exibe:
 * - Palavra-chave correspondente às flags (ex.: "public final class X")
 * - Magic number
 * - Versão da classe
 * - Índices e nomes de this_class e super_class
 * - Contagens de interfaces, fields, methods e attributes
 * - Todo o constant pool (via show_constants)
 * - Fields e methods formatados (via show_fields e show_methods)
 *
 * O conteúdo exibido usa dados do constant pool para resolver nomes de classes,
 * superclasses e UTF-8 strings armazenadas pela JVM.
 *
 * A função **não altera** a estrutura nem libera memória do ClassFile. A única
 * memória liberada é a alocada internamente por parse_flags().
 *
 * @param cf Ponteiro para o ClassFile a ser exibido. Deve ser válido e carregado
 *           previamente por read_classfile().
 */
void show_classfile(ClassFile *cf)
{
    cp_info *cp = cf->constant_pool;

    /* Recupera índices de nome da classe e superclasse */
    u2 cls_name_index = cp[cf->this_class - 1].info.Class.name_index;
    u2 super_cls_name_index = cp[cf->super_class - 1].info.Class.name_index;

    /* Nomes das classes no constant pool (UTF-8 wide string) */
    wchar_t *classname = cp[cls_name_index - 1].info.UTF8.str;
    wchar_t *super_classname = cp[super_cls_name_index - 1].info.UTF8.str;

    /* Conversão das flags para texto */
    char *class_access_flags = parse_flags(cf->access_flags, 8, ", ", class_flag_map);
    char *class_kws_flags = parse_flags(cf->access_flags, 6, " ", class_flag_kw_map);

    /* Cabeçalho da classe */
    printf("%s class %ls\n", class_kws_flags, classname);

    printf("  Magic: %#X\n", cf->magic);
    printf("  Version: %u.%u\n", cf->major_version, cf->minor_version);
    printf("  Flags: (%#.4x) %s\n", cf->access_flags, class_access_flags);

    printf("  this_class: #%u\t\t\t// %ls\n", cf->this_class, classname);
    printf("  super_class: #%u\t\t\t// %ls\n", cf->super_class, super_classname);

    printf("  interfaces: %u, fields: %u, methods: %u, attributes: %u\n",
           cf->interfaces_count, cf->fields_count, cf->methods_count, cf->attributes_count);

    /* Constant Pool */
    printf("Constant Pool (count = %u):\n", cf->constant_pool_count);
    if (cf->constant_pool_count > 0)
    {
        show_constants(cf->constant_pool_count, cp);
        printf("{\n");
    }

    /* Fields */
    if (cf->fields_count > 0)
    {
        show_fields(cf);
        printf("\n");
    }

    /* Methods */
    if (cf->methods_count > 0)
    {
        show_methods(cf);
        printf("}\n");
    }

    /* Libera as strings alocadas por parse_flags */
    free(class_access_flags);
    free(class_kws_flags);
}
