#ifndef TYPES_ATTRIBUTES_ATTRIBUTE_ENUM_H
#define TYPES_ATTRIBUTES_ATTRIBUTE_ENUM_H

// Enumeração de nomes de atributos para uso em funções utilitárias
typedef enum
{
    ConstantValue,
    Code,
    StackMapTable,
    Exceptions,
    InnerClasses,
    EnclosingMethod,
    Synthetic,
    Signature,
    SourceFile,
    SourceDebugExtension,
    LineNumberTable,
    LocalVariableTable,
    LocalVariableTypeTable,
    Deprecated,
    RuntimeVisibleAnnotations,
    RuntimeInvisibleAnnotations,
    RuntimeVisibleParameterAnnotations,
    RuntimeInvisibleParameterAnnotations,
    AnnotationDefault,
    BootstrapMethods,
    MethodParameters
} attribute_name;

#endif