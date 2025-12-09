/**
 * @file method_area_utils.c
 * @brief Implementação das rotinas de manipulação da Method Area e carregamento de classes.
 *
 * Este módulo contém funções responsáveis por localizar classes, métodos e campos na Method Area,
 * construir a estrutura interna `Class` a partir de um ClassFile, preparar a Runtime Constant Pool,
 * inicializar campos, métodos e bytecode, além de efetuar limpeza completa da memória alocada.
 *
 * Funcionalidades principais:
 *  - Busca de classes, métodos e campos por nome e descritor.
 *  - Construção da representação em runtime de uma classe (incluindo constantes, métodos e campos).
 *  - Cálculo do número de argumentos de métodos a partir do descritor JVM.
 *  - Descarregamento e liberação de memória associada às classes na Method Area.
 */

#include "method_area_utils.h"

/**
 * @brief Procura uma classe pelo nome dentro da Method Area.
 *
 * Percorre o vetor de classes carregadas e retorna o ponteiro para
 * a classe cujo nome coincide com o nome solicitado.
 *
 * @param class_name Nome da classe a ser procurada.
 * @param method_area Estrutura contendo todas as classes carregadas.
 * @return Ponteiro para a classe encontrada, ou NULL caso não exista.
 */
Class *lookup_class(const char *class_name, const MethodArea *method_area)
{
    for (u2 i = 0; i < method_area->num_classes; i++)
    {
        if (!strcmp(class_name, method_area->classes[i].name))
            return &method_area->classes[i];
    }

    return NULL;
}

/**
 * @brief Procura um método pelo nome e descritor dentro de uma classe.
 *
 * @param method_name Nome do método, ex.: "main".
 * @param method_descriptor Descritor JVM, ex.: "([Ljava/lang/String;)V".
 * @param class Classe onde procurar o método.
 * @return Ponteiro para o método encontrado, ou NULL caso não exista.
 */
Method *lookup_method(const char *method_name, const char *method_descriptor, const Class *class)
{
    for (u2 i = 0; i < class->method_count; i++)
    {
        if (!strcmp(method_name, class->methods[i].name) &&
            !strcmp(method_descriptor, class->methods[i].descriptor))
            return &class->methods[i];
    }

    return NULL;
}

/**
 * @brief Procura um campo pelo nome e descritor dentro de uma classe.
 *
 * @param field_name Nome do campo.
 * @param field_descriptor Tipo do campo em formato JVM, ex.: "I", "Ljava/lang/String;".
 * @param class Classe onde procurar o campo.
 * @return Ponteiro para o campo encontrado, ou NULL caso não exista.
 */
Field *lookup_field(const char *field_name, const char *field_descriptor, const Class *class)
{
    for (u2 i = 0; i < class->field_count; i++)
    {
        if (!strcmp(field_name, class->fields[i].name) &&
            !strcmp(field_descriptor, class->fields[i].type))
            return &class->fields[i];
    }

    return NULL;
}

/**
 * @brief Inicializa a Runtime Constant Pool (RCP) da classe.
 *
 * Converte a constant_pool do ClassFile em uma representação interna otimizada para execução.
 * Extrai valores primitivos e armazena referências para strings, nomes e tipos.
 *
 * @param cls Classe sendo construída.
 * @param cf Estrutura ClassFile lida do arquivo .class.
 */
static void set_runtimecp(Class *cls, ClassFile *cf)
{
    cls->constants_count = cf->constant_pool_count;
    cls->runtime_cp = (RuntimeConstant *)calloc(cf->constant_pool_count, sizeof(RuntimeConstant));

    for (u2 i = 0; i < cf->constant_pool_count - 1; i++)
    {
        cls->runtime_cp[i].type = cf->constant_pool[i].tag;

        switch (cf->constant_pool[i].tag)
        {
        case CONSTANT_NameAndType:
        case CONSTANT_UTF8:
            break;

        case CONSTANT_Integer:
            cls->runtime_cp[i].value.i = cf->constant_pool[i].info._4Bn.number.i;
            break;

        case CONSTANT_Float:
            cls->runtime_cp[i].value.f = cf->constant_pool[i].info._4Bn.number.f;
            break;

        case CONSTANT_Long:
            cls->runtime_cp[i].value.l = cf->constant_pool[i].info._8Bn.number.l;
            i++; // Long ocupa duas entradas
            break;

        case CONSTANT_Double:
            cls->runtime_cp[i].value.d = cf->constant_pool[i].info._8Bn.number.d;
            i++; // Double ocupa duas entradas
            break;

        default:
            cls->runtime_cp[i].value.strref = get_constant_UTF8_value(i + 1, cf->constant_pool);
            break;
        }
    }
}

/**
 * @brief Copia os campos declarados no ClassFile para a estrutura Class.
 *
 * Extrai nome, tipo, flags de acesso e atributos.
 *
 * @param cls Classe sendo construída.
 * @param cf Estrutura ClassFile correspondente.
 */
static void set_class_fields(Class *cls, ClassFile *cf)
{
    cls->field_count = cf->fields_count;
    cls->fields = (Field *)calloc(cf->fields_count, sizeof(Field));

    for (u2 i = 0; i < cf->fields_count; i++)
    {
        cls->fields[i].name = get_constant_UTF8_value(cf->fields[i].name_index, cf->constant_pool);
        cls->fields[i].type = get_constant_UTF8_value(cf->fields[i].descriptor_index, cf->constant_pool);
        cls->fields[i].access_flags = cf->fields[i].access_flags;
        cls->fields[i].attrs = cf->fields[i].attributes;
    }
}

/**
 * @brief Conta quantos argumentos existem no descritor de método.
 *
 * Exemplo:
 *  - "(IDLjava/lang/String;)V" → 3 argumentos
 *
 * Tipos suportados:
 *  - primitivos (I, D, F, B, C, S, Z)
 *  - objetos (L...;)
 *  - arrays ([...)
 *
 * @param params Parte do descritor entre parênteses.
 * @return Número de argumentos.
 */
static u2 count_args(char *params)
{
    u2 nargs = 0;
    char *p = params + 1;

    while (*p != ')')
    {
        switch (*p)
        {
        case '[':
            break;
        case 'L':
            nargs++;
            while (*p != ';')
                p++;
            break;
        default:
            nargs++;
            break;
        }
        p++;
    }

    return nargs;
}

/**
 * @brief Copia e inicializa os métodos da classe a partir do ClassFile.
 *
 * O procedimento inclui:
 *  - Extração do nome e descritor
 *  - Separação da lista de parâmetros e tipo de retorno
 *  - Cálculo do número de argumentos
 *  - Cópia do bytecode, caso o método não seja `abstract`
 *
 * @param cls Classe sendo construída.
 * @param cf Estrutura ClassFile correspondente.
 */
static void set_class_methods(Class *cls, ClassFile *cf)
{
    cls->method_count = cf->methods_count;
    cls->methods = (Method *)calloc(cf->methods_count, sizeof(Method));

    for (u2 i = 0; i < cf->methods_count; i++)
    {
        cls->methods[i].name = get_constant_UTF8_value(cf->methods[i].name_index, cf->constant_pool);
        cls->methods[i].descriptor = get_constant_UTF8_value(cf->methods[i].descriptor_index, cf->constant_pool);

        size_t lparams = 1;
        char *desc = cls->methods[i].descriptor;
        while (*desc++ != ')')
            lparams++;

        size_t lret = strlen(cls->methods[i].descriptor) - lparams;

        cls->methods[i].params = (char *)calloc((lparams + 1), sizeof(char));
        cls->methods[i].rettype = (char *)calloc((lret + 1), sizeof(char));

        if (cls->methods[i].params && cls->methods[i].rettype)
        {
            strncpy(cls->methods[i].params, cls->methods[i].descriptor, lparams);
            strncpy(cls->methods[i].rettype, cls->methods[i].descriptor + lparams, lret);
        }

        cls->methods[i].access_flags = cf->methods[i].access_flags;
        cls->methods[i].bytecode.nargs = count_args(cls->methods[i].params);

        if (!(cf->methods[i].access_flags & 0x0400))
        {
            cls->methods[i].bytecode.max_locals = cf->methods[i].attributes->info.Code.max_locals;
            cls->methods[i].bytecode.max_stack = cf->methods[i].attributes->info.Code.max_stack;
            cls->methods[i].bytecode.code_length = cf->methods[i].attributes->info.Code.code_length;

            cls->methods[i].bytecode.code = (u1 *)malloc(cf->methods[i].attributes->info.Code.code_length);
            memcpy(cls->methods[i].bytecode.code, cf->methods[i].attributes->info.Code.code,
                   cf->methods[i].attributes->info.Code.code_length);
        }
    }
}

/**
 * @brief Cria uma classe em runtime e carrega suas estruturas internas a partir de um arquivo .class.
 *
 * O processo envolve:
 *  - abrir e interpretar o ClassFile
 *  - criar a estrutura Class
 *  - copiar a constant pool, campos e métodos
 *  - preparar bytecode e metadados
 *
 * @param path Caminho do arquivo .class.
 * @return Ponteiro para a classe carregada.
 */
Class *create_and_load_class(const char *path)
{
    FILE *fptr = open_classfile(path);
    ClassFile cf = read_classfile(fptr, false);

    Class *cls = (Class *)malloc(sizeof(Class));

    cls->name = get_constant_UTF8_value(cf.this_class, cf.constant_pool);
    cls->super = get_constant_UTF8_value(cf.super_class, cf.constant_pool);
    cls->super_index = cf.super_class;

    set_runtimecp(cls, &cf);
    set_class_fields(cls, &cf);
    set_class_methods(cls, &cf);

    free_classfile(&cf);

    return cls;
}

/**
 * @brief Libera completamente a memória associada à Method Area.
 *
 * Desaloca:
 *  - tabelas de referência
 *  - classes e seus nomes
 *  - runtime constant pool
 *  - campos e métodos (incluindo bytecode)
 *
 * @param method_area Estrutura contendo todas as classes e referências.
 */
void cleanup(MethodArea method_area)
{
    for (size_t i = 0; i < method_area.ref_count; i++)
        free(method_area.refs[i]);
    free(method_area.refs);

    for (size_t i = 0; i < method_area.num_classes; i++)
    {
        Class cls = method_area.classes[i];
        free(cls.name);
        free(cls.super);

        for (size_t j = 0; j < cls.constants_count; j++)
        {
            if ((cls.runtime_cp[j].type >= 7 && cls.runtime_cp[j].type <= 11) ||
                cls.runtime_cp[j].type >= 15)
                free(cls.runtime_cp[j].value.strref);
        }
        free(cls.runtime_cp);

        for (u2 j = 0; j < cls.field_count; j++)
        {
            free(cls.fields[j].name);
            free(cls.fields[j].type);
        }
        free(cls.fields);

        for (u2 j = 0; j < cls.method_count; j++)
        {
            free(cls.methods[j].name);
            free(cls.methods[j].descriptor);
            free(cls.methods[j].params);
            free(cls.methods[j].rettype);
            free(cls.methods[j].bytecode.code);
        }
        free(method_area.classes[i].methods);
    }
    free(method_area.classes);
}
