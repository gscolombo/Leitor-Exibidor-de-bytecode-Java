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
            if (f->method->refs)
            {
                f->method->ref_count++;
                multiarray->array_ref.array.t = (u1)c.type;
                multiarray->array_ref.array.arraylength = counts[dims];
                multiarray->array_ref.array.dims = dims + 1;
                multiarray->array_ref.array.values = (reference *)calloc(counts[dims], sizeof(reference));

                if (multiarray->array_ref.array.values)
                    f->method->refs[f->method->ref_count - 1] = multiarray->array_ref.array.values;
            }
            break;
        // TODO: Handle primitive types
        default:
            break;
        }
    }
    else
    {
        allocref(f);
        if (f->method->refs)
        {
            f->method->ref_count++;
            reference *ma = (reference *)calloc(counts[dims], sizeof(reference));
            if (ma)
            {
                f->method->refs[f->method->ref_count - 1] = ma;
                ArrayRef aref = {
                    .t = 0,
                    .arraylength = counts[dims],
                    .dims = dims + 1,
                    .values = ma};

                multiarray->array_ref.array = aref;
                for (u1 d = 0; d < counts[dims]; d++)
                {
                    create_multiarray(f, counts, dims - 1, &((reference *)multiarray->array_ref.array.values)[d]);
                }
            }
        }
    }
}

void multianewarray(Frame *f)
{
    u1 dims = f->method->bytecode.code[f->pc + 3];

    int32_t counts[dims];
    for (u1 i = 0; i < dims; i++)
        counts[dims - (i + 1)] = pop_operand(f).value.t._int;

    dtype multiarray = initialize_var(REFERENCE);
    create_multiarray(f, counts, dims - 1, &multiarray.value.ref);

    push_operand(f, multiarray);

    f->pc += 4;
}