Тестовый проект по мониторингу компьютерного клуба.


# Инструкции по сборке:

Собирать в Lunix через gcc/clang

## DEBUG with UNIT tests
```
mkdir build-debug && cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON ..
cmake --build .
ctest 
```

## Release 
```
mkdir build-release && cd build-release
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF ..
cmake --build . --config Release
./main <path_to_test_file.txt>
```
### Тесты из data/ 

empty.txt - пустой файл

formatName.txt - не корректное имя клиента

formatNTables.txt - не корректное число столов

testOrig.txt - Исходный пример из ТЗ

test2.txt - Пример нормальной работы

tooEarly.txt - Проверка работы не смотря на возникающие ошибки 

Unit тесты представлены в tests/