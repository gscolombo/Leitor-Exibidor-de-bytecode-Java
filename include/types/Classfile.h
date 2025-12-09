/**
 * @file Classfile.h
 * @brief Estrutura que representa um arquivo `.class` conforme a especificação da JVM 8.
 *
 * Esta estrutura contém todos os dados extraídos diretamente do arquivo `.class`,
 * incluindo constant pool, interfaces, fields, methods e atributos da classe.
 *
 * Ela é consumida pelos módulos de:
 * - carregamento de classes (bootstrap_loader),
 * - construção da MethodArea,
 * - interpretador,
 * - exibidor (`show_classfile`).
 */

#ifndef CLASSFILE_H_
#define CLASSFILE_H_

#include "uinteger.h"
#include "member.h"
#include "constants.h"

/**
 * @struct ClassFile
 * @brief Representa a estrutura completa de um arquivo `.class` da JVM 8.
 *
 * Depois de lido pelo parser, esta estrutura mantém fielmente todas
 * as informações do arquivo binário, permitindo inspeção e conversão
 * para estruturas de tempo de execução.
 */
typedef struct ClassFile
{
    /**
     * @brief Número mágico 0xCAFEBABE que identifica arquivos `.class`.
     *
     * Se este valor for diferente, o arquivo é inválido.
     */
    u4 magic;

    /**
     * @brief Versão menor do compilador (minor version).
     */
    u2 minor_version;

    /**
     * @brief Versão maior do compilador (major version).
     *
     * Exemplos:
     * - 52 → Java 8  
     * - 55 → Java 11
     * - 61 → Java 17
     */
    u2 major_version;

    /**
     * @brief Quantidade de entradas na constant pool.
     *
     * Importante: índices válidos vão de **1 até constant_pool_count - 1**.
     * O índice 0 é reservado e nunca usado.
     */
    u2 constant_pool_count;

    /**
     * @brief Vetor de entradas da constant pool (`cp_info`).
     */
    cp_info *constant_pool;

    /**
     * @brief Flags de acesso da classe (public, final, abstract etc.).
     *
     * Valores definidos em `constants.h`, como:
     * - ACC_PUBLIC
     * - ACC_FINAL
     * - ACC_SUPER
     * - ACC_INTERFACE
     * - ACC_ABSTRACT
     */
    u2 access_flags;

    /**
     * @brief Índice no constant pool que aponta para o nome desta classe.
     *
     * O valor deve referenciar um CONSTANT_Class, que por sua vez referencia um CONSTANT_Utf8.
     */
    u2 this_class;

    /**
     * @brief Índice no constant pool para a superclasse.
     *
     * É 0 *somente* quando se trata de `java/lang/Object`.
     */
    u2 super_class;

    /**
     * @brief Quantidade de interfaces implementadas por esta classe.
     */
    u2 interfaces_count;

    /**
     * @brief Vetor de índices no constant pool para cada interface implementada.
     */
    u2 *interfaces;

    /**
     * @brief Quantidade de campos declarados na classe.
     */
    u2 fields_count;

    /**
     * @brief Vetor de estruturas `member_info` representando cada campo.
     */
    member_info *fields;

    /**
     * @brief Quantidade de métodos declarados na classe.
     */
    u2 methods_count;

    /**
     * @brief Vetor de estruturas `member_info` representando cada método.
     *
     * Caso o método tenha atributo `Code`, seu bytecode aparecerá dentro dessa estrutura.
     */
    member_info *methods;

    /**
     * @brief Quantidade de atributos da classe.
     *
     * Exemplos comuns: SourceFile, InnerClasses, RuntimeVisibleAnnotations.
     */
    u2 attributes_count;

    /**
     * @brief Vetor de atributos da classe.
     */
    attribute *attributes;

} ClassFile;

#endif /* CLASSFILE_H_ */
