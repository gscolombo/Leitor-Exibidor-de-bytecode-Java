#ifndef TYPES_ATTRIBUTES_ATTRIBUTE_INFO_H
#define TYPES_ATTRIBUTES_ATTRIBUTE_INFO_H

#include <wchar.h>

#include "uinteger.h"
#include "attributes.h"

/**
 * Estrutura geral de um atributo.
 */
typedef struct attribute
{
    u2 attribute_name_index;
    u4 attribute_length;
    union
    {
        _ConstantValue ConstantValue;
        struct
        {
            _Code fields;
            u2 attributes_count;
            struct attribute *attributes;
        } Code;
        _StackMapTable StackMapTable;
        _Exceptions Exceptions;
        _InnerClasses InnerClasses;
        _EnclosingMethod EnclosingMethod;
        _Signature Signature;
        _SourceFile SourceFile;
        _SourceDebugExtension SourceDebugExtension;
        _LineNumberTable LineNumberTable;
        _LocalVariableTable LocalVariableTable;
        _LocalVariableTypeTable LocalVariableTypeTable;
        _RuntimeAnnotation RuntimeVisibleAnnotations, RuntimeInvisibleAnnotations;
        _RuntimeParemeterAnnotations RuntimeVisibleParameterAnnotations, RuntimeInvisibleParameterAnnotations;
        _AnnotationDefault AnnotationDefault;
        _BootstrapMethod BootstrapMethods;
        _MethodParameters MethodParameters;
    } info;
} attribute;

#endif