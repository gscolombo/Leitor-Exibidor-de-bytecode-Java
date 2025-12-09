/**
 * @file cp/writer.c
 * @brief Rotinas para exibição e resolução de entradas da Constant Pool (CP).
 *
 * Este módulo implementa utilitários para:
 *  - imprimir uma representação legível da constant pool (show_constants);
 *  - resolver recursivamente valores UTF-8 referenciados por entradas da CP
 *    (get_constant_UTF8_value).
 *
 * A saída é pensada para depuração e inspeção do conteúdo dos arquivos `.class`.
 * Algumas funções realizam alocações dinâmicas e retornam strings que devem ser
 * liberadas pelo chamador (quando documentado).
 */

#include "cp/writer.h"

/**
 * @brief Substitui quebras de linha em uma string por sequências literais "\n".
 *
 * Percorre a string `s` e cria uma nova string onde cada `'\n'` é substituído
 * por dois caracteres: `'\\'` e `'n'`. A nova string é alocada pela função e
 * deve ser liberada pelo chamador.
 *
 * @note Função `static` de utilidade interna ao módulo.
 *
 * @param s String de entrada (não é modificada).
 * @return Ponteiro para nova string alocada contendo as substituições ou NULL
 *         se a alocação falhar.
 */
static char *replace_newlines(char *s)
{
    int newlines = 0;
    
    for (char *c = s; c < s + strlen(s); c++)
        if (*c == '\n')
            newlines++;

    size_t l = strlen(s) + (newlines * 2) + 1;

    char *str = (char *)malloc(l * sizeof(char) + 1);
    size_t i, j;
    for (i = 0, j = 0; i < strlen(s); i++, j++) 
        if (s[i] == '\n')
        {
            str[j] = '\\';
            str[++j] = 'n';
        }
        else
            str[j] = s[i];

    str[j] = '\0';
    return str;
}

/**
 * @brief Imprime a constant pool completa em formato legível.
 *
 * Percorre `count` entradas começando em `_cp` (ponteiro para o primeiro elemento
 * da CP) e imprime informações formatadas dependendo do `tag` de cada entrada.
 * Para entradas de string/UTF8 aplica `replace_newlines` para evitar quebras de linha
 * na saída.
 *
 * A função usa `num_digits` para alinhar a saída e `parse_flags` quando aplicável.
 *
 * @param count Número de entradas na constant pool (valor retornado no ClassFile).
 * @param _cp Ponteiro para o array de cp_info (constant pool), com indexação 0-based.
 */
void show_constants(u2 count, cp_info *_cp)
{
    u2 i = 1;
    for (cp_info *cp = _cp; cp < _cp + count; cp++, i++)
    {
        u1 ws = num_digits(count) - num_digits(i);
        char *pad = malloc(ws + 1);
        memset(pad, ' ', ws);
        pad[ws] = '\0';

        switch (cp->tag)
        {
        case CONSTANT_Class:
        {
            u2 cls_name_index = cp->info.Class.name_index;
            printf("%s#%u = Class\t\t\t#%u\t\t// %s\n",
                   pad, i, cls_name_index, _cp[cls_name_index - 1].info.UTF8.str);
            break;
        }
        case CONSTANT_Fieldref:
        case CONSTANT_Methodref:
        case CONSTANT_InterfaceMethodref:
        {
            char *ref;
            if (cp->tag == CONSTANT_Fieldref)
                ref = "Fieldref";
            else if (cp->tag == CONSTANT_Methodref)
                ref = "Methodref";
            else
                ref = "InterfaceMethodref";

            u2 cls_index = cp->info.Ref.class_index;
            u2 name_and_type_index = cp->info.Ref.name_and_type_index;

            char *cls = _cp[_cp[cls_index - 1].info.Class.name_index - 1].info.UTF8.str;
            char *ref_name = _cp[_cp[name_and_type_index - 1].info.NameAndType.name_index - 1].info.UTF8.str;
            char *ref_type = _cp[_cp[name_and_type_index - 1].info.NameAndType.descriptor_index - 1].info.UTF8.str;

            printf("%s#%u = %s\t\t\t#%u.#%u\t\t// %s.%s:%s\n",
                   pad, i, ref, cls_index, name_and_type_index, cls, ref_name, ref_type);
            break;
        }
        case CONSTANT_String:
        {
            u2 str_index = cp->info.String.string_index;
            char *str = _cp[str_index - 1].info.UTF8.str;

            str = replace_newlines(str);

            printf("%s#%u = String\t\t\t#%u\t\t// %s\n", pad, i, str_index, str);
            free(str);
            break;
        }
        case CONSTANT_Integer:
            printf("%s#%u = Integer\t\t\t%i\n", pad, i, cp->info._4Bn.number.i);
            break;
        case CONSTANT_Float:
            printf("%s#%u = Float\t\t\t%.1ff\n", pad, i, cp->info._4Bn.number.f);
            break;
        case CONSTANT_Long:
            printf("%s#%u = Long\t\t\t%lil\n", pad, i, cp->info._8Bn.number.l);
            break;
        case CONSTANT_Double:
            printf("%s#%u = Double\t\t\t%.1lfd\n", pad, i, cp->info._8Bn.number.d);
            break;
        case CONSTANT_NameAndType:
        {
            u2 name_index = cp->info.NameAndType.name_index;
            u2 desc_index = cp->info.NameAndType.descriptor_index;
            char *name = _cp[name_index - 1].info.UTF8.str;
            char *desc = _cp[desc_index - 1].info.UTF8.str;
            name = !strcmp(name, "<init>") ? "\"<init>\"" : name;
            printf("%s#%u = NameAndType\t\t#%u.#%u\t\t// %s:%s\n", pad, i, name_index, desc_index, name, desc);
            break;
        }
        case CONSTANT_UTF8:
        {
            char *str = cp->info.UTF8.str;

            str = replace_newlines(str);

            printf("%s#%u = UTF-8\t\t\t%s\n", pad, i, str);
            free(str);
            break;
        }
        case CONSTANT_MethodHandle:
            printf("%s#%u = MethodHandle\n", pad, i);
            break;
        case CONSTANT_MethodType:
            printf("%s#%u = MethodType\n", pad, i);
            break;
        case CONSTANT_InvokeDynamic:
            printf("%s#%u = InvokeDynamic\n", pad, i);
            break;
        default:
            break;
        }

        free(pad);
    }
}

/**
 * @brief Resolve recursivamente e retorna uma string com o valor textual de uma entrada da CP.
 *
 * Esta função recebe um índice 1-based `i` e o array `cp` (0-based) e resolve o valor
 * UTF-8 correspondente àquela entrada. Para entradas compostas (Fieldref/Methodref/NameAndType)
 * a função constrói e retorna uma string alocada dinamicamente que concatena as partes
 * (por exemplo "java/lang/ClassName.methodName:descriptor"). O chamador é responsável por
 * liberar a string retornada com free().
 *
 * @param i Índice 1-based da entrada na constant pool.
 * @param cp Ponteiro para o array de cp_info (constant pool).
 * @return Ponteiro para string alocada contendo a representação textual do valor ou NULL
 *         se não for possível resolver. A string, se não-NULL, deve ser liberada pelo chamador.
 */
char *get_constant_UTF8_value(u2 i, const cp_info *cp)
{
    char *s = NULL;
    i--;

    switch (cp[i].tag)
    {
    case CONSTANT_Class:
        return get_constant_UTF8_value(cp[i].info.Class.name_index, cp);
    case CONSTANT_Fieldref:
    case CONSTANT_Methodref:
    case CONSTANT_InterfaceMethodref:
    {
        u2 cls_index = cp[i].info.Ref.class_index;
        u2 name_and_type_index = cp[i].info.Ref.name_and_type_index;

        char *cls_name = get_constant_UTF8_value(cls_index, cp);
        char *name_and_type = get_constant_UTF8_value(name_and_type_index, cp);

        if (cls_name && name_and_type)
        {
            s = (char *)malloc((strlen(cls_name) + strlen(name_and_type) + 2) * sizeof(char));
            if (s)
                sprintf(s, "%s.%s", cls_name, name_and_type);
            free(cls_name);
            free(name_and_type);
        }
        break;
    }
    case CONSTANT_NameAndType:
    {
        u2 name_index = cp[i].info.NameAndType.name_index;
        u2 desc_index = cp[i].info.NameAndType.descriptor_index;

        char *ref_name = get_constant_UTF8_value(name_index, cp);
        char *ref_type = get_constant_UTF8_value(desc_index, cp);

        if (ref_name != NULL && ref_type != NULL)
        {
            s = (char *)malloc((strlen(ref_name) + strlen(ref_type) + 2) * sizeof(char));
            if (s)
                sprintf(s, "%s:%s", ref_name, ref_type);
            free(ref_name);
            free(ref_type);
        }
        break;
    }
    case CONSTANT_String:
    {
        char *str = get_constant_UTF8_value(cp[i].info.String.string_index, cp);
        s = (char *)malloc((strlen(str) + 1) * sizeof(char));
        strcpy(s, str);
        free(str);
        break;
    }
    case CONSTANT_Integer:
    case CONSTANT_Float:
    case CONSTANT_Long:
    case CONSTANT_Double:
        s = (char *)malloc(15 * sizeof(char));
        if (s)
        {
            switch (cp[i].tag)
            {
            case CONSTANT_Integer:
                sprintf(s, "%i", cp[i].info._4Bn.number.i);
                break;
            case CONSTANT_Float:
                sprintf(s, "%gf", cp[i].info._4Bn.number.f);
                break;
            case CONSTANT_Long:
                sprintf(s, "%lil", cp[i].info._8Bn.number.l);
                break;
            case CONSTANT_Double:
                sprintf(s, "%gd", cp[i].info._8Bn.number.d);
                break;
            default:
                break;
            }
            size_t l = strlen(s);
            s = (char *)realloc(s, l + 1);
        }
        break;
    case CONSTANT_UTF8:
        s = (char *)malloc((strlen(cp[i].info.UTF8.str) + 1) * sizeof(char));
        strcpy(s, cp[i].info.UTF8.str);
        break;
    default:
        break;
    }

    return s;
}
