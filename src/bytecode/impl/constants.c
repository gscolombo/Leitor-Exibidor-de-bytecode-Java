/**
 * @file bytecode/impl/constants.c
 * @brief Implementações das instruções JVM que empilham constantes na pilha de operandos.
 *
 * Contém handlers para instruções que carregam constantes imediatas ou da constant pool:
 *  - aconst_null
 *  - iconst_m1..iconst_5 (agrupadas em iconst_i)
 *  - fconst_0..fconst_2 (agrupadas em fconst_f)
 *  - dconst_0..dconst_1 (agrupadas em dconst_d)
 *  - bipush
 *  - sipush
 *  - ldc / ldc_w / ldc2_w (implementadas por ldc_)
 *
 * Cada handler:
 *  - inicializa um `dtype` apropriado (via initialize_var),
 *  - preenche seu valor,
 *  - empilha o valor na pilha de operandos do frame,
 *  - avança o program counter (`f->pc`) conforme o tamanho da instrução.
 *
 * Observações:
 *  - `ldc_` interpreta o opcode atual para distinguir entre ldc (u1 index) e ldc_w / ldc2_w (u2 index).
 *  - Valores da runtime constant pool são obtidos em `f->class->runtime_cp[index - 1]`.
 */

#include "bytecode/impl/constants.h"

/**
 * @brief Implementa aconst_null.
 *
 * Empilha uma referência nula (REF_NULL) na pilha de operandos.
 *
 * Opcode: aconst_null (0x01)
 *
 * @param f Frame atual de execução.
 */
void aconst_null(Frame *f)
{
    dtype null = initialize_var(REFERENCE, f);
    null.value.ref->type = REF_NULL;

    push_operand(f, null);

    f->pc++;
}

/**
 * @brief Implementa iconst_<n> compacto (agrupamento).
 *
 * Trata as instruções iconst_m1, iconst_0 .. iconst_5. O opcode atual determina
 * o valor empilhado: (opcode - 3) produz os valores -1..5 conforme mapeamento do projeto.
 *
 * Ex.: iconst_0 (opcode 0x03) -> 0, iconst_1 -> 1, etc.
 *
 * @param f Frame atual.
 */
void iconst_i(Frame *f)
{
    dtype i = initialize_var(INT, f);
    i.value.t._int = (int32_t)f->method->bytecode.code[f->pc] - 3;
    push_operand(f, i);
    f->pc++;
}

/**
 * @brief Implementa fconst_<n> compacto (agrupamento).
 *
 * Trata fconst_0..fconst_2. Converte a diferença entre o opcode atual e o deslocamento
 * usado no projeto para obter o valor float (0.0f, 1.0f, 2.0f).
 *
 * @param f Frame atual.
 */
void fconst_f(Frame *f)
{
    dtype _f = initialize_var(FLOAT, f);
    _f.value.t._float = (float)(f->method->bytecode.code[f->pc] - 11);
    push_operand(f, _f);
    f->pc++;
}

/**
 * @brief Implementa dconst_<n> compacto (agrupamento).
 *
 * Trata dconst_0 e dconst_1. Calcula o valor double a partir do opcode.
 *
 * @param f Frame atual.
 */
void dconst_d(Frame *f)
{
    dtype d = initialize_var(DOUBLE, f);
    d.value.t._double = (double)(f->method->bytecode.code[f->pc] - 14);
    push_operand(f, d);
    f->pc++;
}

/**
 * @brief Implementa bipush.
 *
 * Empilha um byte assinado imediato como integer (promovido para dtype INT).
 *
 * Formato: bipush <byte>
 * Tamanho: 2 bytes
 *
 * @param f Frame atual.
 */
void bipush(Frame *f)
{
    dtype bi = initialize_var(BYTE, f);
    bi.value.t._int = (int8_t)f->method->bytecode.code[f->pc + 1];
    push_operand(f, bi);
    f->pc += 2;
}

/**
 * @brief Implementa sipush.
 *
 * Empilha um short (16-bit big-endian) imediato como integer (promovido para dtype INT).
 *
 * Formato: sipush <byte1> <byte2>
 * Tamanho: 3 bytes
 *
 * @param f Frame atual.
 */
void sipush(Frame *f)
{
    dtype si = initialize_var(SHORT, f);
    u1 b1 = f->method->bytecode.code[f->pc + 1];
    u1 b2 = f->method->bytecode.code[f->pc + 2];
    si.value.t._int = (int32_t)((b1 << 8) | b2);

    push_operand(f, si);
    f->pc += 3;
}

/**
 * @brief Implementa ldc / ldc_w / ldc2_w (unified handler).
 *
 * - Determina se o índice vem em 1 byte (ldc) ou 2 bytes (ldc_w, ldc2_w) com base
 *   no opcode atual: `mode = f->method->bytecode.code[f->pc] - 18`.
 *   (modo > 0 => índice de 2 bytes).
 * - Recupera o RuntimeConstant em `f->class->runtime_cp[index - 1]`.
 * - Constrói e empilha o `dtype` correspondente (INT, FLOAT, LONG, DOUBLE, REFERENCE/string).
 *
 * Observações:
 *  - ldc (u1 index) usa 2 bytes de instrução; ldc_w/ldc2_w usam 3 bytes.
 *  - Para CONSTANT_String, empilha uma referência do tipo REF_STRING apontando para
 *    a string resolvida em runtime_cp.
 *  - Outros tipos (ex.: class refs) estão marcados como TODO quando necessário.
 *
 * @param f Frame atual.
 */
void ldc_(Frame *f)
{
    u1 mode = f->method->bytecode.code[f->pc] - 18;

    dtype var;

    u2 index;
    u1 b1 = f->method->bytecode.code[f->pc + 1];
    u1 b2 = f->method->bytecode.code[f->pc + 2];

    index = mode > 0 ? ((b1 << 8) | b2) : b1;

    RuntimeConstant c = f->class->runtime_cp[index - 1];

    switch (c.type)
    {
    case CONSTANT_Integer:
        var = initialize_var(INT, f);
        var.value.t._int = c.value.i;
        break;
    case CONSTANT_Float:
        var = initialize_var(FLOAT, f);
        var.value.t._float = c.value.f;
        break;
    case CONSTANT_Long:
        var = initialize_var(LONG, f);
        var.value.t._long = c.value.l;
        break;
    case CONSTANT_Double:
        var = initialize_var(DOUBLE, f);
        var.value.t._double = c.value.d;
        break;
    case CONSTANT_String:
        var = initialize_var(REFERENCE, f);
        var.value.ref->type = REF_STRING;
        var.value.ref->value.array_ref.string = c.value.strref;
        break;
    // TODO: Handle class references
    default:
        break;
    }

    push_operand(f, var);
    f->pc += mode > 0 ? 3 : 2;
}
