#include "attributes.h"

static const struct
{
    const attribute_name attr;
    const wchar_t *str;
} conversion[] = {
    {ConstantValue, L"ConstantValue"},
    {Code, L"Code"},
    {StackMapTable, L"StackMapTable"},
    {Exceptions, L"Exceptions"},
    {InnerClasses, L"InnerClasses"},
    {EnclosingMethod, L"EnclosingMethod"},
    {Synthetic, L"Synthetic"},
    {Signature, L"Signature"},
    {SourceFile, L"SourceFile"},
    {SourceDebugExtension, L"SourceDebugExtension"},
    {LineNumberTable, L"LineNumberTable"},
    {LocalVariableTable, L"LocalVariableTable"},
    {LocalVariableTypeTable, L"LocalVariableTypeTable"},
    {Deprecated, L"Deprecated"},
    {RuntimeVisibleAnnotations, L"RuntimeVisibleAnnotations"},
    {RuntimeInvisibleAnnotations, L"RuntimeInvisibleAnnotations"},
    {RuntimeVisibleParameterAnnotations, L"RuntimeVisibleParameterAnnotations"},
    {RuntimeInvisibleParameterAnnotations, L"RuntimeInvisibleParameterAnnotations"},
    {AnnotationDefault, L"AnnotationDefault"},
    {BootstrapMethods, L"BootstrapMethods"},
    {MethodParameters, L"MethodParameters"}};

const attribute_name *convert_attr_name(const wchar_t *name)
{
    if (name != NULL)
        for (size_t i = 0; i < sizeof(conversion) / sizeof(conversion[0]); i++)
        {
            if (!wcscmp(name, conversion[i].str))
                return &conversion[i].attr;
        }

    return NULL;
}