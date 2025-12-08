#include "bytecode/impl/extended.h"

static void create_multiarray(Frame *f, int32_t counts[], u1 dims, reference *multiarray)
{
    if (!dims)
    {
        u1 b1 = f->method->bytecode.code[f->pc + 1];
        u1 b2 = f->method->bytecode.code[f->pc + 2];
        u2 index = (b1 << 8) | b2;

        RuntimeConstant c = f->class->runtime_cp[index - 1];

        switch (c.type)
        {
        case CONSTANT_Class:
        case CONSTANT_String:
            allocref(f);
            multiarray->value.array_ref.array.t = (u1)c.type;
            multiarray->value.array_ref.array.class_name = c.value.strref;
            multiarray->value.array_ref.array.arraylength = counts[dims];
            multiarray->value.array_ref.array.dims = dims + 1;
            multiarray->value.array_ref.array.values = (reference *)calloc(counts[dims], sizeof(reference));

            if (!multiarray->value.array_ref.array.values)
                exit(1);

            appendref(f, multiarray->value.array_ref.array.values);
            break;
        // TODO: Handle primitive types
        default:
            break;
        }
    }
    else
    {
        allocref(f);
        reference *ma = (reference *)calloc(counts[dims], sizeof(reference));
        if (ma)
        {
            f->method_area->refs[f->method_area->ref_count - 1] = ma;
            ArrayRef aref = {
                .t = 0,
                .class_name = NULL,
                .arraylength = counts[dims],
                .dims = dims + 1,
                .values = ma};

            multiarray->value.array_ref.array = aref;
            for (u1 d = 0; d < counts[dims]; d++)
                create_multiarray(f, counts, dims - 1, &((reference *)multiarray->value.array_ref.array.values)[d]);
        }
    }
}

void multianewarray(Frame *f)
{
    u1 dims = f->method->bytecode.code[f->pc + 3];

    int32_t counts[dims];
    for (u1 i = 0; i < dims; i++)
        counts[dims - (i + 1)] = pop_operand(f).value.t._int;

    dtype multiarray = initialize_var(REFERENCE, f);
    multiarray.value.ref->type = REF_ARRAY;
    create_multiarray(f, counts, dims - 1, multiarray.value.ref);

    push_operand(f, multiarray);

    f->pc += 4;
}

void if_null(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 offset = (code[f->pc + 1] << 8) | code[f->pc + 2];

    bool isnull = pop_operand(f).value.ref->type == REF_NULL;
    bool branch = (code[f->pc] == 198) ? isnull : !isnull; // Choose between ifnull and ifnonnull

    if (branch)
        f->pc += offset;
    else
        f->pc += 3;
}