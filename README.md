# Leitor/Exibidor de *bytecode* Java

Ferramenta de linha de comando para leitura e exibição de *bytecode* Java, similar ao `javap`.

### Instruções de uso
Um arquivo `Makefile` está disponível na raiz do projeto.\
Primeiramente, **certifique-se de possuir a ferramenta `make` instalada** no seu sistema. Em seguida, na pasta raiz do projeto, execute o comando `make`. Será criado o arquivo executável `cjavap` na mesma pasta e uma pasta `build` com os arquivos `.o` associados.\
Para utilizar o programa, execute-o em um terminal passando como argumento o **caminho relativo** para um arquivo `.class`.
```bash
# Exemplo
./cjavap ../caminho/para/ponto/class
```
O arquivo `Makefile` também possui outras opções disponíveis:
- `clean`: Deleta o arquivo executável `cjavap` e a pasta `build`.
- `rebuild`: Deleta os arquivos criados e recompila o código-fonte.
- `debug`: Recompila o código-fonte com informações para *debugging* e linkado com o [*AddressSanitizer*](https://github.com/google/sanitizers/wiki/AddressSanitizer), para análise dinâmica do código. Em seguida, executa o executável criado e, na ausência de erros, deleta o arquivo.
  
**Observação:** A compilação é realizada com `gcc`. Certifique-se de possuir a ferramenta no seu sistema.