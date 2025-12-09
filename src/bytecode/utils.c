/**
 * @file bytecode/utils.c
 * @brief Utilitários e helpers usados pelos handlers de bytecode.
 *
 * Este módulo implementa funções auxiliares usadas por várias instruções JVM
 * (por exemplo, instruções `tableswitch`/`lookupswitch`, operações de impressão,
 * e manipulação simples de "referências" e buffers de string usados pelo bootstrap
 * / runtime simplificado do projeto).
 *
 * As funções aqui fornecem:
 *  - leitura de valores de 32 bits alinhados no array de bytecode;
 *  - rotina genérica de impressão (`_print`) para simular chamadas a System.out.println/...;
 *  - helpers para gerenciamento rápido de "string buffers" (inicialização, append, toString);
 *  - funções para alocar e registrar referências no MethodArea (appendref/allocref).
 *
 * Observações:
 *  - A implementação usa a estrutura `Frame`, `dtype` e `MethodArea` definidas em headers do projeto.
 *  - Algumas partes são simplificações / placeholders (comentadas com TODO) — adequadas
 *    ao propósito didático/demonstrativo do interpretador.
 */

#include "bytecode/utils.h"

/**
 * @brief Lê e retorna um valor de 32 bits armazenado como 4 bytes consecutivos em `code`.
 *
 * Interpreta os bytes em ordem big-endian a partir do índice `i` em `code` e
 * monta um int32_t. Usado por instruções de switch que precisam ler offsets de 4 bytes.
 *
 * @param i Índice inicial (0-based) no array `code`.
 * @param code Ponteiro para o array de bytecode (u1*).
 * @return Valor int32_t resultante da concatenação dos 4 bytes a partir de `code[i]`.
 *
 * @note A assinatura usa `inline extern` conforme o arquivo original; dependendo
 *       das flags do compilador, a ligação/visibilidade pode variar.
 */
inline extern int32_t get_switch_32B_values(u4 i, const u1 *code)
{
    return (code[i] << 24) | (code[i + 1] << 16) | (code[i + 2] << 8) | code[i + 3];
}

/**
 * @brief Função auxiliar de impressão para simular chamadas a métodos de print.
 *
 * Popa o(s) operandos necessários da pilha do frame e imprime conforme o tipo de retorno
 * especificado por `rettype`. O parâmetro `descriptor` pode ser usado para escolhas específicas
 * de assinatura (por exemplo, detectar `"(Ljava/lang/String;)V"`).
 *
 * Tipos suportados atualmente:
 *  - 'L' (referência) — apenas String é tratada explicitamente quando o descriptor for "(Ljava/lang/String;)V".
 *  - 'I' — int
 *  - 'J' — long
 *  - 'F' — float
 *  - 'D' — double
 *  - 'C' — char
 *
 * Para outros tipos há um fallback que apenas imprime o caractere `e`.
 *
 * @param f Ponteiro para o frame atual.
 * @param descriptor Descriptor do método (ex.: "(Ljava/lang/String;)V") — usado para casos especiais.
 * @param rettype Caractere indicando o tipo do valor a ser impresso (ex.: 'I', 'L', ...).
 * @param e Caractere de terminação a ser impresso após o valor (por exemplo, '\n').
 */
void _print(Frame *f, const char *descriptor, char rettype, char e)
{
    switch (rettype)
    {
    case 'L':
        if (!strcmp(descriptor, "(Ljava/lang/String;)V"))
        {
            char *str = pop_operand(f).value.ref->value.array_ref.string;
            printf("%s%c", str, e);
        }
        // TODO: Define logic for class references
        break;
    case 'I':
    {
        int32_t i = pop_operand(f).value.t._int;
        printf("%i%c", i, e);
        break;
    }
    case 'J':
    {
        int64_t l = pop_operand(f).value.t._long;
        printf("%lli%c", l, e);
        break;
    }
    case 'F':
    {
        float _f = pop_operand(f).value.t._float;
        printf("%.1f%c", _f, e);
        break;
    }
    case 'D':
    {
        double _d = pop_operand(f).value.t._double;
        if (_d == (int64_t)_d)
            printf("%.1f%c", _d, e);
        else
            printf("%.16g%c", _d, e);
        break;
    }
    case 'C':
    {
        u2 c = pop_operand(f).value.t._char;
        printf("%c%c", c, e);
        break;
    }
    // TODO: Define logic for other cases
    default:
        printf("%c", e);
        break;
    }
}

/**
 * @brief Inicializa um StringBuffer simples associado a uma referência de objeto.
 *
 * Aloca um buffer de caracteres com capacidade inicial (16) e registra o ponteiro
 * na tabela de referências da MethodArea (última entrada criada por allocref()).
 * Ajusta `objectref->value.ref->value.array_ref.string` para apontar para o buffer.
 *
 * @param f Ponteiro para o frame atual (usa f->method_area para registrar a referência).
 * @param objectref Ponteiro para o dtype que contém a referência ao objeto a ser inicializado.
 *
 * @note Em caso de falha de alocação a função chama exit(1).
 */
void init_stringbuffer(Frame *f, dtype *objectref)
{
    allocref(f);

    // Pre-allocate a string with the default capacity of 16 characters
    char *strbuf = (char *)calloc(16, sizeof(char));

    if (!strbuf)
        exit(1);

    f->method_area->refs[f->method_area->ref_count - 1] = strbuf;
    objectref->value.ref->value.array_ref.string = strbuf;
}

/**
 * @brief Implementa StringBuffer.append(Object) simplificado — concatena strings.
 *
 * Popa o argumento (esperado ser uma referência para string), popa o `strbuf` (referência
 * para o buffer), concatena a string argumento no buffer e empilha de volta a referência
 * ao buffer (comportamento equivalente a `areturn` do método em termos de retorno de referência).
 *
 * @param f Ponteiro para o frame atual.
 *
 * @warning Atualmente assume que ambos operandos são strings; TODO: suportar outros tipos.
 */
void strbuf_append(Frame *f)
{
    // TODO: Handle other argument types besides string
    char *arg = pop_operand(f).value.ref->value.array_ref.string;
    dtype strbuf = pop_operand(f);
    strcat(strbuf.value.ref->value.array_ref.string, arg); // Concatenate strings
    push_operand(f, strbuf);                               // Return reference to objectref (like areturn)
}

/**
 * @brief Converte o StringBuffer em String (ajusta alocação e retorna referência).
 *
 * Ajusta o tamanho do buffer com realloc para a ocupação atual (`strlen(str) + 1`),
 * atualiza a entrada correspondente em method_area->refs e empilha a referência resultante.
 *
 * @param f Ponteiro para o frame atual.
 */
void strbuf_tostring(Frame *f)
{
    dtype strbuf = pop_operand(f);
    char *str = strbuf.value.ref->value.array_ref.string;

    u4 i;
    for (i = 0; i < f->method_area->ref_count; i++)
        if (f->method_area->refs[i] == str)
            break;

    strbuf.value.ref->value.array_ref.string = (char *)realloc(str, strlen(str) + 1); // Adjust string size
    f->method_area->refs[i] = strbuf.value.ref->value.array_ref.string;
    push_operand(f, strbuf); // Return reference to objectref (like areturn)
}

/**
 * @brief Adiciona a última referência alocada (`allocref`) na tabela de referências.
 *
 * A função escreve `ref` na última posição alocada (index ref_count - 1) do vetor
 * `method_area->refs`. É utilizada internamente após `allocref()` para completar
 * o registro da nova referência.
 *
 * @param f Ponteiro para o frame atual (usa f->method_area).
 * @param ref Ponteiro para o objeto/estrutura a ser registrada.
 */
inline void appendref(Frame *f, void *ref)
{
    f->method_area->refs[f->method_area->ref_count - 1] = ref;
}

/**
 * @brief Aloca (ou realoca) espaço para uma nova referência na MethodArea.
 *
 * Garante que `method_area->refs` tenha espaço para uma entrada adicional e incrementa
 * `method_area->ref_count`. Em caso de falha de alocação a função imprime uma mensagem
 * e encerra o processo.
 *
 * @param f Ponteiro para o frame atual cujo MethodArea será usado.
 *
 * @note A função preserva a convenção do projeto em que a última posição alocada é aquela
 *       usada imediatamente por `appendref()` ou pelo chamador.
 */
void allocref(Frame *f)
{
    if (!f->method_area->ref_count)
        f->method_area->refs = malloc(sizeof(void *));
    else
        f->method_area->refs = (void **)realloc(f->method_area->refs, ((f->method_area->ref_count + 1) * sizeof(void *)));

    if (!f->method_area->refs)
    {
        printf("Error during allocation for reference variable.");
        exit(1);
    }

    f->method_area->ref_count++;
}
