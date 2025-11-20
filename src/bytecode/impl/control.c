#include "bytecode/impl/control.h"

void _goto(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    f->pc += (int16_t)((code[f->pc + 1] << 8) | code[f->pc + 2]);
}

void tableswitch(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u4 start = f->pc;

    while ((++f->pc) % 4 != 0)
        continue;

    int32_t _default = get_switch_32B_values(f->pc, code);
    f->pc += 4;
    int32_t low = get_switch_32B_values(f->pc, code);
    f->pc += 4;
    int32_t high = get_switch_32B_values(f->pc, code);
    f->pc += 4;

    int32_t i = pop_operand(f).value.t._int;

    if (i < low || i > high)
        f->pc = start + _default;
    else
        f->pc = start + get_switch_32B_values(f->pc + 4 * i, code);
}

void lookupswitch(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u4 start = f->pc;

    while ((++f->pc) % 4 != 0)
        continue;

    int32_t _default = get_switch_32B_values(f->pc, code);
    f->pc += 4;
    int32_t npairs = get_switch_32B_values(f->pc, code);
    f->pc += 4;

    int32_t key = pop_operand(f).value.t._int;

    int32_t match;
    for (int32_t i = 0; i < npairs; i++)
    {
        match = get_switch_32B_values(f->pc, code);
        if (key == match)
        {
            f->pc = start + get_switch_32B_values(f->pc + 4, code); // add offset to pc;
            return;
        }
        f->pc += 8;
    }

    f->pc = start + _default;
}

void Treturn(Frame *f)
{
    dtype ret = pop_operand(f);

    if (f->method->bytecode.code[f->pc] == 0xb0 && ret.value.ref->type != REF_NULL)
    {
        dtype retcpy = initialize_var(REFERENCE, f->previous_frame);
        memcpy(retcpy.value.ref, ret.value.ref, sizeof(reference));

        if (!strcmp(f->method->rettype, "Ljava/lang/String;"))
        {
            allocref(f->previous_frame);

            char *str = ret.value.ref->value.array_ref.string;
            size_t length = strlen(str) + 1;
            retcpy.value.ref->value.array_ref.string = (char *)malloc(length * sizeof(char));
            memcpy(retcpy.value.ref->value.array_ref.string, str, length);

            f->previous_frame->method->refs[f->previous_frame->method->ref_count - 1] = retcpy.value.ref->value.array_ref.string;
        }
        else
        {
            allocref(f->previous_frame);
            allocref(f->previous_frame);

            retcpy.value.ref->value.object_ref = (Class *)malloc(sizeof(Class));
            if (!retcpy.value.ref->value.object_ref)
                exit(1);

            memcpy(retcpy.value.ref->value.object_ref, ret.value.ref->value.object_ref, sizeof(Class));

            u2 field_count = retcpy.value.ref->value.object_ref->field_count;
            retcpy.value.ref->value.object_ref->fields = (Field *)calloc(field_count, sizeof(Field));
            if (!retcpy.value.ref->value.object_ref->fields)
                exit(1);

            memcpy(retcpy.value.ref->value.object_ref->fields, ret.value.ref->value.object_ref->fields, sizeof(Field) * field_count);

            f->previous_frame->method->refs[f->previous_frame->method->ref_count - 1] = retcpy.value.ref->value.object_ref;
            f->previous_frame->method->refs[f->previous_frame->method->ref_count - 1] = retcpy.value.ref->value.object_ref->fields;
        }

        push_operand(f->previous_frame, retcpy);
    }
    else
        push_operand(f->previous_frame, ret);

    f->pc = f->method->bytecode.code_length;
}

void _return(Frame *f)
{
    f->pc = f->method->bytecode.code_length;
}