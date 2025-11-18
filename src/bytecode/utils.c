#include "bytecode/utils.h"

inline extern int32_t get_tableswitch_32B_values(u4 i, const u1 *code)
{
    return (code[i] << 24) | (code[i + 1] << 16) | (code[i + 2] << 8) | code[i + 3];
}

void _print(Frame *f, const char *descriptor, char rettype, char e)
{
    switch (rettype)
    {
    case 'L':
        if (!strcmp(descriptor, "(Ljava/lang/String;)V"))
        {
            char *str = pop_operand(f).value.ref.array_ref.string;
            printf("%s%c", str, e);
        }
        // TODO: Define logic for class references
        break;
    case 'I':
        int32_t i = pop_operand(f).value.t._int;
        printf("%i%c", i, e);
        break;
    case 'J':
        int64_t l = pop_operand(f).value.t._long;
        printf("%lli%c", l, e);
        break;
    case 'F':
        float _f = pop_operand(f).value.t._float;
        printf("%.1f%c", _f, e);
        break;
    case 'D':
        double _d = pop_operand(f).value.t._double;
        if (_d == (int64_t)_d)
            printf("%.1f%c", _d, e);
        else
            printf("%.16g%c", _d, e);
        break;
    case 'C':
        u2 c = pop_operand(f).value.t._char;
        printf("%c%c", c, e);
        break;
    // TODO: Define logic for other cases
    default:
        printf("%c", e);
        break;
    }
}

void init_stringbuffer(Frame *f, dtype *objectref)
{
    // Pre-allocate a string with the default capacity of 16 characters
    char *strbuf = (char *)calloc(16, sizeof(char));

    allocref(f);
    if (f->method->refs && strbuf)
    {
        f->method->ref_count++;
        f->method->refs[f->method->ref_count - 1] = strbuf;
        objectref->value.ref.array_ref.string = strbuf;
    }
}

void strbuf_append(Frame *f)
{
    // TODO: Handle other argument types besides string
    char *arg = pop_operand(f).value.ref.array_ref.string;
    dtype strbuf = pop_operand(f);
    strcat(strbuf.value.ref.array_ref.string, arg); // Concatenate strings
    push_operand(f, strbuf);                        // Return reference to objectref (like areturn)
}

void strbuf_tostring(Frame *f)
{
    dtype strbuf = pop_operand(f);
    char *str = strbuf.value.ref.array_ref.string;

    u4 i = 0;
    for (i; i < f->method->ref_count; i++)
        if (f->method->refs[i] == str)
            break;

    strbuf.value.ref.array_ref.string = (char *)realloc(str, strlen(str) + 1); // Adjust string size
    f->method->refs[i] = strbuf.value.ref.array_ref.string;
    push_operand(f, strbuf); // Return reference to objectref (like areturn)
}

void allocref(Frame *f)
{
    if (!f->method->ref_count)
        f->method->refs = malloc(sizeof(void **));
    else
        f->method->refs = (void **)realloc(f->method->refs, ((f->method->ref_count + 1) * sizeof(void **)));
}