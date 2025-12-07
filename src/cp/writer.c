/**
 * @file src/cp/writer.c
 * @brief Funções para exibição do Constant Pool (implementação).
 *
 * A função principal aqui é `show_constants` que percorre o array de `cp_info`
 * e imprime cada entrada em formato legível, semelhante à saída do `javap`.
 *
 * Observações:
 * - A função assume que as strings UTF-8 já foram decodificadas para `wchar_t*`
 *   (campo `UTF8.str`) durante o parsing do constant pool.
 * - Alguns índices no constant pool referenciam outras entradas; o código faz
 *   ajustes com base no índice atual `i` para acessar corretamente as posições.
 */

#include "cp/writer.h"

/**
 * @brief Exibe todas as entradas do constant pool.
 *
 * Para cada entrada, a função imprime uma linha no formato:
 *   #<index> = <TagName> <Detalhes>
 *
 * A função alinha os índices à direita calculando quantos espaços são
 * necessários conforme o número máximo de entradas (`count`).
 *
 * @param count Número total de entradas no constant pool (u2).
 * @param _cp Ponteiro para o array de cp_info (constant pool).
 */
void show_constants(u2 count, cp_info *_cp)
{
    /* i é o índice humano (começa em 1 conforme a especificação JVM) */
    u2 i = 1;

    /* Percorre o array de cp_info: cada cp aponta para a entrada atual */
    for (cp_info *cp = _cp; cp < _cp + count; cp++, i++)
    {
        /* 
         * Alinhamento visual: calcula quantos espaços são necessários para alinhar
         * os índices da lista à direita. Ex.: para 100 entradas, "#  1", "# 10", "#100".
         */
        u1 ws = num_digits(count) - num_digits(i);
        char *pad = malloc(ws + 1);
        if (pad == NULL)
        {
            /* Em caso de falha de alocação, garante não quebrar a execução */
            pad = (char *)malloc(1);
            if (pad == NULL)
                return;
            pad[0] = '\0';
        }
        memset(pad, ' ', ws);
        pad[ws] = '\0';

        switch (cp->tag)
        {
        case CONSTANT_Class:
        {
            /* A entrada Class armazena um índice para uma UTF8 que contém o nome. */
            u2 cls_name_index = cp->info.Class.name_index - i;
            /* Observação: impressão usa cp[cls_name_index].info.UTF8.str assumindo que
               o ajuste por '- i' corresponde à organização do array no projeto. */
            printf("%s#%u = Class\t\t\t#%u\t\t// %ls\n",
                   pad, i, cls_name_index, cp[cls_name_index].info.UTF8.str);
        }
        break;

        case CONSTANT_Fieldref:
        case CONSTANT_Methodref:
        case CONSTANT_InterfaceMethodref:
        {
            /* Determina o rótulo textual (Fieldref / Methodref / InterfaceMethodref) */
            char *ref;
            if (cp->tag == CONSTANT_Fieldref)
                ref = "Fieldref";
            else if (cp->tag == CONSTANT_Methodref)
                ref = "Methodref";
            else
                ref = "InterfaceMethodref";

            /* Índices das estruturas de referência */
            u2 cls_index = cp->info.Ref.class_index;
            u2 name_and_type_index = cp->info.Ref.name_and_type_index;

            /* Recupera as strings correspondentes (classe, nome e tipo) */
            wchar_t *cls = cp[cp[cls_index - i].info.Class.name_index - i].info.UTF8.str;
            wchar_t *ref_name = cp[cp[name_and_type_index - i].info.NameAndType.name_index - i].info.UTF8.str;
            wchar_t *ref_type = cp[cp[name_and_type_index - i].info.NameAndType.descriptor_index - i].info.UTF8.str;

            /* 
             * Para conformidade com a saída do javap, o método <init> é mostrado entre aspas.
             * Substitui somente para exibição, sem alterar o conteúdo original.
             */
            ref_name = !wcscmp(ref_name, (wchar_t *)L"<init>") ? L"\"<init>\"" : ref_name;

            printf("%s#%u = %s\t\t\t#%u.#%u\t\t// %ls.%ls:%ls\n",
                   pad, i, ref, cls_index, name_and_type_index, cls, ref_name, ref_type);
        }
        break;

        case CONSTANT_String:
        {
            /* String referencia uma entrada UTF8 pelo índice string_index */
            u2 str_index = cp->info.String.string_index;
            printf("%s#%u = String\t\t\t#%u\t\t// %ls\n", pad, i, str_index, cp[str_index - i].info.UTF8.str);
        }
        break;

        case CONSTANT_Integer:
            printf("%s#%u = Integer\t\t\t%i\n", pad, i, cp->info._4Bn.number.i);
            break;

        case CONSTANT_Float:
            printf("%s#%u = Float\t\t\t%f\n", pad, i, cp->info._4Bn.number.f);
            break;

        case CONSTANT_Long:
            printf("%s#%u = Long\t\t\t%li\n", pad, i, cp->info._8Bn.number.l);
            break;

        case CONSTANT_Double:
            printf("%s#%u = Double\t\t\t%f\n", pad, i, cp->info._8Bn.number.d);
            break;

        case CONSTANT_NameAndType:
        {
            /* Recupera índices e strings para NameAndType */
            u2 name_index = cp->info.NameAndType.name_index;
            u2 desc_index = cp->info.NameAndType.descriptor_index;

            wchar_t *name = cp[name_index - i].info.UTF8.str;
            wchar_t *desc = cp[desc_index - i].info.UTF8.str;

            /* Mesma regra de exibição para <init> */
            name = !wcscmp(name, (wchar_t *)L"<init>") ? L"\"<init>\"" : name;

            printf("%s#%u = NameAndType\t\t#%u.#%u\t\t// %ls:%ls\n", pad, i, name_index, desc_index, name, desc);
        }
        break;

        case CONSTANT_UTF8:
            /* Strings UTF-8 já decodificadas em wchar_t* */
            printf("%s#%u = UTF-8\t\t\t%ls\n", pad, i, cp->info.UTF8.str);
            break;

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
            /* Caso não reconhecido: não imprime nada específico */
            break;
        }

        /* Libera o buffer de padding criado para o alinhamento */
        free(pad);
    }
}
