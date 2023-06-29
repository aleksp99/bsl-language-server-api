# AddIn.Parser

## AddinTemplate - шаблон внешней компоненты 1С:
https://github.com/Infactum/addin-template  
https://github.com/lintest/AddinTemplate  

## BSL:
https://github.com/1c-syntax/bsl-parser


## ANTLR4:  
java -jar antlr-4.9.12-complete.jar -Dlanguage=Cpp BSLLexer.g4

## 1С:  
```
//УстановитьВнешнююКомпоненту("ОбщийМакет.BSL_LS_API"); // только для клиента
ПодключитьВнешнююКомпоненту("ОбщийМакет.BSL_LS_API", "BSLLS");
BSL_LS = Новый("AddIn.BSLLS.BSLLanguageServerAPI");
BSL_LS.Адрес = "ws://localhost:8025/lsp";
СтрокаJSON = ВК.Проверять("1=1");
```	