
# Инструкции по сборке:

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

empty - пустой файл
formatName - не корректное имя клиента
formatNTables - не корректное число столов
testOrig - Исходный пример из ТЗ
test2 - Пример нормальной работы
tooEarly - Проверка работы не смотря на возникающие ошибки 

Unit представлены в tests/