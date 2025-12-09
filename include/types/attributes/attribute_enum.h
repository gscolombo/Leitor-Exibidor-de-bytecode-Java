/**
 * @file attribute_enum.h
 * @brief Enumeração de todos os tipos de atributos definidos pela JVM 8.
 *
 * Este enum é utilizado por funções utilitárias (como `convert_attr_name`)
 * para mapear nomes textuais de atributos — lidos da constant pool —
 * para valores internos mais fáceis de manipular no código.
 *
 * Cada valor corresponde exatamente a um atributo listado na especificação
 * do formato de arquivo `.class`.
 */

#ifndef TYPES_ATTRIBUTES_ATTRIBUTE_ENUM_H
#define TYPES_ATTRIBUTES_ATTRIBUTE_ENUM_H

/**
 * @enum attribute_name
 * @brief Enumeração de nomes de atributos suportados.
 *
 * A JVM define diversos tipos de atributos que podem aparecer em:
 * - classes
 * - métodos
 * - campos
 * - atributos internos (ex.: dentro de Code)
 *
 * Este enum lista todos os tipos de atributos relevantes.  
 * A identificação do atributo é feita via comparação com o nome UTF-8
 * correspondente no constant pool.
 *
 * Exemplos de nomes no constant pool → valores deste enum:
 * - `"Code"`                   → Code  
 * - `"SourceFile"`             → SourceFile  
 * - `"LineNumberTable"`        → LineNumberTable  
 * - `"Exceptions"`             → Exceptions  
 * - `"RuntimeVisibleAnnotations"`      → RuntimeVisibleAnnotations  
 */
typedef enum
{
    ConstantValue,                     /**< Valor constante para campos final (ConstantValue). */
    Code,                              /**< Atributo contendo bytecode e informações auxiliares. */
    StackMapTable,                     /**< Tabela usada pelo verificador de tipos. */
    Exceptions,                        /**< Lista de exceções declaradas por um método. */
    InnerClasses,                      /**< Informações sobre classes internas. */
    EnclosingMethod,                   /**< Indica o método que contém uma classe local/anônima. */
    Synthetic,                         /**< Marca entidades geradas automaticamente pelo compilador. */
    Signature,                         /**< Contém assinaturas estendidas (inclui genéricos). */
    SourceFile,                        /**< Nome do arquivo-fonte original. */
    SourceDebugExtension,              /**< Dados estendidos para debugging. */
    LineNumberTable,                   /**< Mapeia bytecode para números de linha do fonte. */
    LocalVariableTable,                /**< Informações de variáveis locais para debugging. */
    LocalVariableTypeTable,            /**< Tipos genéricos das variáveis locais. */
    Deprecated,                        /**< Marca classes/métodos/campos como obsoletos. */
    RuntimeVisibleAnnotations,         /**< Anotações visíveis em runtime (usadas por reflection). */
    RuntimeInvisibleAnnotations,       /**< Anotações invisíveis em runtime. */
    RuntimeVisibleParameterAnnotations,/**< Anotações sobre parâmetros visíveis em runtime. */
    RuntimeInvisibleParameterAnnotations,/**< Anotações sobre parâmetros invisíveis em runtime. */
    AnnotationDefault,                 /**< Valor default de elementos de anotações. */
    BootstrapMethods,                  /**< Métodos bootstrap para invokedynamic. */
    MethodParameters                   /**< Metadados sobre parâmetros formais do método. */

} attribute_name;

#endif /* TYPES_ATTRIBUTES_ATTRIBUTE_ENUM_H */
