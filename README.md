# RANSAC-counter
## Описание
Приложение служит для вычисления функции прямой для набора точек при помощи RANSAC-алгоритма и визуализации результата вычислений. Имеет две части: серверную и клиентскую.  

**Серверная** часть служит для:
  * внесения множества точек (вручную или из текстового файла, поддреживается сохранение текущего набора точек в файл, удаление из множества);
  * вычисления функции прямой RANSAC-алгоритмом (присутствуют последовательный и параллельный режимы вычисления);
  * визаулизации результата вычислений - отображение результирующей прямой, разделение точек на графике на входящие (синие) и выбросы (красные);
  * отправки сериализованного (3-ей версии Protocol Buffers) результата вычислений (функции) клиентской части через UDP-протокол.  

Общий вид графического приложения серверной части:  
![Снимок экрана от 2023-09-10 14-10-53](https://github.com/NikitaKadili/ransac-counter/assets/117101213/bf0ff27d-aa88-42be-8993-25fae3bd7789)  

**Клиентская** часть служит для:
  * приема результата вычислений (функции) по UDP-протоколу, десериализации полученных данных;
  * визуализации результирующей прямой;
  * вывода функции результирующей прямой в текстовое поле.  

Общий вид графического приложения клиентской части:  
![Снимок экрана от 2023-09-10 14-12-16](https://github.com/NikitaKadili/ransac-counter/assets/117101213/08800257-4466-4773-83c8-6a00e011a37d)  

## Компиляция
Компиляция осуществяелтся отдельно для клиента и сервера из среды разработки или из их корневых папок при помощи системы сборки CMake, с предустановленной средой разработки Qt версии 5.15 и выше, Protobuf (protoc или схожие компиляторы с поддержкой 3-ей версии Protocol Buffers).  
Команды для компиляции системой сборки релизной версии (для каждой части отдельно):
```
cmake -B./build-release -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
```

## Системные требования
  * C++17 (STL)
  * g++ с поддержкой 17-го стандарта (также, возможно применения иных компиляторов C++ с поддержкой необходимого стандарта)
  * Qt 5.15
  * Protobuf

## Планы по доработке
  - [X] Внедрить protobuf для сериализации/десериализации данных для отправки/получения результата вычислений
  - [ ] Скорректировать общий вид полей для ввода с масками
  - [ ] Оптимизировать показ большого количества точек на графике

## Стек
  * C++17
  * Qt 5.15 + QCustomPlot
  * Protobuf
