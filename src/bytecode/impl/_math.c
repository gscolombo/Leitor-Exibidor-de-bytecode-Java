/**
 * @file bytecode/impl/_math.c
 * @brief Implementações das instruções aritméticas da JVM (parte inteira e ponto-flutuante).
 *
 * Este módulo fornece handlers para instruções aritméticas usadas pelo interpretador,
 * incluindo operações sobre inteiros, long e double, e a instrução iinc.
 *
 * Cada handler:
 *  - popa os operandos necessários da pilha de operandos do frame;
 *  - realiza a operação aritmética correspondente;
 *  - empilha o resultado (quando aplicável);
 *  - avança o program counter (`f->pc`) de acordo com o tamanho da instrução.
 *
 * Observações:
 *  - Usa-se `initialize_var(...)` para criar um `dtype` de retorno apropriado.
 *  - As operações em ponto-flutuante usam o tipo `double` quando indicado.
 */

#include "bytecode/impl/_math.h"
#include <math.h> /* para fmod */

/**
 * @brief Implementa iadd.
 *
 * Popa dois inteiros (v2, v1), calcula v1 + v2 e empilha o resultado como INT.
 *
 * Formato (tamanho): 1 byte (opcode).
 *
 * @param f Frame atual.
 */
void iadd(Frame *f)
{
    int32_t v1, v2;
    v2 = pop_operand(f).value.t._int;
    v1 = pop_operand(f).value.t._int;

    dtype result = initialize_var(INT, f);
    result.value.t._int = v1 + v2;
    push_operand(f, result);

    f->pc++;
}

/**
 * @brief Implementa isub.
 *
 * Popa dois inteiros (v2, v1), calcula v1 - v2 e empilha o resultado como INT.
 *
 * @param f Frame atual.
 */
void isub(Frame *f)
{
    int32_t v1, v2;
    v2 = pop_operand(f).value.t._int;
    v1 = pop_operand(f).value.t._int;

    dtype result = initialize_var(INT, f);
    result.value.t._int = v1 - v2;
    push_operand(f, result);

    f->pc++;
}

/**
 * @brief Implementa imul.
 *
 * Popa dois inteiros (v2, v1), calcula v1 * v2 e empilha o resultado como INT.
 *
 * @param f Frame atual.
 */
void imul(Frame *f)
{
    int32_t v1, v2;
    v2 = pop_operand(f).value.t._int;
    v1 = pop_operand(f).value.t._int;

    dtype result = initialize_var(INT, f);
    result.value.t._int = v1 * v2;
    push_operand(f, result);

    f->pc++;
}

/**
 * @brief Implementa iinc.
 *
 * Incrementa a variável local no índice imediato por uma constante imediata (signed byte).
 * Não empilha nem desempilha valores.
 *
 * Formato: iinc indexbyte constbyte
 *
 * @param f Frame atual.
 */
void iinc(Frame *f)
{
    u1 idx = f->method->bytecode.code[f->pc + 1];
    int32_t _const = (int32_t)(int8_t)f->method->bytecode.code[f->pc + 2];

    f->local_variables[idx].value.t._int += _const;
    f->pc += 3;
}

/**
 * @brief Implementa lsub.
 *
 * Popa dois long (v2, v1), calcula v1 - v2 e empilha o resultado como LONG.
 *
 * @param f Frame atual.
 */
void lsub(Frame *f)
{
    int64_t v2 = pop_operand(f).value.t._long;
    int64_t v1 = pop_operand(f).value.t._long;

    dtype result = initialize_var(LONG, f);
    result.value.t._long = v1 - v2;
    push_operand(f, result);

    f->pc++;
}

/**
 * @brief Implementa dadd.
 *
 * Popa dois double (v2, v1), calcula v1 + v2 e empilha o resultado como DOUBLE.
 *
 * @param f Frame atual.
 */
void dadd(Frame *f)
{
    double v2 = pop_operand(f).value.t._double;
    double v1 = pop_operand(f).value.t._double;

    dtype result = initialize_var(DOUBLE, f);
    result.value.t._double = v1 + v2;
    push_operand(f, result);

    f->pc++;
}

/**
 * @brief Implementa dsub.
 *
 * Popa dois double (v2, v1), calcula v1 - v2 e empilha o resultado como DOUBLE.
 *
 * @param f Frame atual.
 */
void dsub(Frame *f)
{
    double v2 = pop_operand(f).value.t._double;
    double v1 = pop_operand(f).value.t._double;

    dtype result = initialize_var(DOUBLE, f);
    result.value.t._double = v1 - v2;
    push_operand(f, result);

    f->pc++;
}

/**
 * @brief Implementa dmul.
 *
 * Popa dois double (v2, v1), calcula v1 * v2 e empilha o resultado como DOUBLE.
 *
 * @param f Frame atual.
 */
void dmul(Frame *f)
{
    double v2 = pop_operand(f).value.t._double;
    double v1 = pop_operand(f).value.t._double;

    dtype result = initialize_var(DOUBLE, f);
    result.value.t._double = v1 * v2;
    push_operand(f, result);

    f->pc++;
}

/**
 * @brief Implementa ddiv.
 *
 * Popa dois double (v2, v1), calcula v1 / v2 e empilha o resultado como DOUBLE.
 *
 * Observação: a divisão por zero segue o comportamento do ponto-flutuante C (Inf/NaN).
 *
 * @param f Frame atual.
 */
void ddiv(Frame *f)
{
    double v2 = pop_operand(f).value.t._double;
    double v1 = pop_operand(f).value.t._double;

    dtype result = initialize_var(DOUBLE, f);
    result.value.t._double = v1 / v2;
    push_operand(f, result);

    f->pc++;
}

/**
 * @brief Implementa dneg.
 *
 * Popa um double e empilha seu negativo.
 *
 * @param f Frame atual.
 */
void dneg(Frame *f)
{
    double value = pop_operand(f).value.t._double;

    dtype result = initialize_var(DOUBLE, f);
    result.value.t._double = -value;
    push_operand(f, result);

    f->pc++;
}

/**
 * @brief Implementa drem.
 *
 * Popa dois double (v2, v1), calcula o resto (fmod) v1 % v2 e empilha o resultado como DOUBLE.
 *
 * Observação: usa `fmod` da math.h para comportamento consistente com ponto-flutuante.
 *
 * @param f Frame atual.
 */
void drem(Frame *f)
{
    double v2 = pop_operand(f).value.t._double;
    double v1 = pop_operand(f).value.t._double;

    dtype result = initialize_var(DOUBLE, f);
    result.value.t._double = fmod(v1, v2);
    push_operand(f, result);

    f->pc++;
}
