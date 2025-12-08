/**
 * @file bytecode/impl/conversions.c
 * @brief Implementações das instruções de conversão de tipos (type conversions) da JVM.
 *
 * Este arquivo contém os handlers para instruções que convertem valores entre tipos
 * numéricos (por exemplo, i2l, i2f, i2d, i2b, i2c, i2s e conversões de double para outros).
 *
 * As funções aqui implementadas são:
 *  - i2T : trata várias conversões a partir de int (opcodes 0x85..0x87 e 0x91..0x93)
 *  - d2T : trata conversões a partir de double (opcodes 0x90, 0x8e, 0x8f)
 *
 * Convenções:
 *  - Cada handler popa o operando necessário da pilha de operandos, constrói um novo
 *    `dtype` apropriado ao tipo de destino usando `initialize_var(...)`, armazena o
 *    valor convertido e empilha o resultado.
 *  - Avançam `f->pc` em 1 byte (opcode) ao final.
 *
 * Observações de comportamento:
 *  - Conversões de ponto flutuante para inteiros tratam NaN e overflow conforme comportamentos
 *    simplificados adotados no projeto (por exemplo, NaN → 0; saturação para limites do tipo).
 *  - O código assume que o opcode atual em `f->method->bytecode.code[f->pc]` determina
 *    qual conversão específica deve ser aplicada: a mesma função trata múltiplos opcodes
 *    relacionados (padrão aplicado em vários handlers do projeto).
 */

#include "bytecode/impl/conversions.h"

/**
 * @brief Conversões originadas de um inteiro (`int`) — handler genérico i2X.
 *
 * Esta função trata várias instruções cuja origem é um int:
 *  - 0x85 (i2l) : int → long
 *  - 0x86 (i2f) : int → float
 *  - 0x87 (i2d) : int → double
 *  - 0x91 (i2b) : int → byte (com conversão truncada para signed byte)
 *  - 0x92 (i2c) : int → char (truncamento para unsigned 16-bit)
 *  - 0x93 (i2s) : int → short (truncamento para signed 16-bit)
 *
 * Procedimento:
 *  - Pop do operando (int32_t) da pilha de operandos.
 *  - Dependendo do opcode atual, inicializa um dtype do tipo destino e atribui
 *    o valor convertido.
 *  - Empilha o resultado convertido e avança o programa (pc++). 
 *
 * @param f Ponteiro para o Frame atual.
 */
void i2T(Frame *f)
{
    int32_t value = pop_operand(f).value.t._int;
    dtype result;

    switch (f->method->bytecode.code[f->pc])
    {
    case 0x85: /* i2l */
        result = initialize_var(LONG, f);
        result.value.t._long = (long)value;
        break;
    case 0x86: /* i2f */
        result = initialize_var(FLOAT, f);
        result.value.t._float = (float)value;
        break;
    case 0x87: /* i2d */
        result = initialize_var(DOUBLE, f);
        result.value.t._double = (double)value;
        break;
    case 0x91: /* i2b */
        result = initialize_var(INT, f);
        result.value.t._int = (int32_t)(int8_t)value;
        break;
    case 0x92: /* i2c */
        result = initialize_var(INT, f);
        result.value.t._int = (int32_t)(u2)value;
        break;
    case 0x93: /* i2s */
        result = initialize_var(INT, f);
        result.value.t._int = (int32_t)(int16_t)value;
        break;
    default:
        /* Opcode não tratado aqui; comportamento atual: não modifica result. */
        break;
    }

    push_operand(f, result);
    f->pc++;
}

/**
 * @brief Conversões originadas de um double — handler genérico d2X.
 *
 * Esta função trata conversões cujo operando de origem é um double:
 *  - 0x90 (d2f) : double → float
 *  - 0x8e (d2i) : double → int
 *  - 0x8f (d2l) : double → long
 *
 * Observações:
 *  - Para d2i e d2l a função aplica regras simplificadas para NaN e overflow:
 *      * Se value == NaN → resultado = 0.
 *      * Se value < limite inferior do tipo destino → saturação para INT_MIN / LONG_MIN.
 *      * Se value > limite superior do tipo destino → saturação para INT_MAX / LONG_MAX.
 *
 * Procedimento:
 *  - Pop do operando (double) da pilha de operandos.
 *  - Realiza a conversão segura conforme o opcode.
 *  - Empilha o resultado convertido e avança `pc`.
 *
 * @param f Ponteiro para o Frame atual.
 */
void d2T(Frame *f)
{
    double value = pop_operand(f).value.t._double;
    dtype result;

    switch (f->method->bytecode.code[f->pc])
    {
    case 0x90: /* d2f */
        result = initialize_var(FLOAT, f);
        result.value.t._float = (float)value;
        break;
    case 0x8e: /* d2i */
        result = initialize_var(INT, f);
        if (value == NAN)
            result.value.t._int = 0;
        else if (value < INT32_MIN)
            result.value.t._int = INT32_MIN;
        else if (value > INT32_MAX)
            result.value.t._int = INT32_MAX;
        else
            result.value.t._int = (int32_t)value;
        break;
    case 0x8f: /* d2l */
        result = initialize_var(LONG, f);
        if (value == NAN)
            result.value.t._long = 0;
        else if (value < INT64_MIN)
            result.value.t._long = INT64_MIN;
        else if (value > INT64_MAX)
            result.value.t._long = INT64_MAX;
        else
            result.value.t._long = (int64_t)value;
        break;
    default:
        /* Opcode não tratado aqui; comportamento atual: não modifica result. */
        break;
    }

    push_operand(f, result);
    f->pc++;
}
