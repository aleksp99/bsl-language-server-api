# AddIn.BSL_LS_API

## AddinTemplate - шаблон внешней компоненты 1С:
https://github.com/Infactum/addin-template  
https://github.com/lintest/AddinTemplate  

## BSL-Language-Server:
https://github.com/1c-syntax/bsl-language-server

## 1С:  
```
//УстановитьВнешнююКомпоненту("ОбщийМакет.BSL_LS_API"); // только для клиента
ПодключитьВнешнююКомпоненту("ОбщийМакет.BSL_LS_API", "BSLLS");
BSL_LS = Новый("AddIn.BSLLS.BSLLanguageServerAPI");
BSL_LS.Адрес = "ws://localhost:8025/lsp";
СтрокаJSON = BSL_LS.Проверять("1=1");
```	