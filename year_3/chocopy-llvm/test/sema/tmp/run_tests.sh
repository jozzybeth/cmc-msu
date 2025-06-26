#!/bin/bash

# Папка с тестами
TEST_DIR="/home/gorina007/chocopy-workdir/chocopy-llvm/test/sema/tmp"

# Проверяем, существует ли папка
if [ ! -d "$TEST_DIR" ]; then
    echo "Ошибка: папка $TEST_DIR не существует!"
    exit 1
fi

# Счетчики тестов
total_tests=0
passed_tests=0
failed_tests=0


# Проходим по всем .py файлам и запускаем chocopy-llvm
for test_file in "$TEST_DIR"/*.py; do
    ((total_tests++))
    filename=$(basename "$test_file")
    
    echo "Тест $total_tests: $filename"
    
    # Запускаем тест и захватываем вывод
    if output=$(chocopy-llvm --run-sema "$test_file" 2>&1); then
        echo "  Успех"
        ((passed_tests++))
    else
        echo "  Ошибка"
        echo "  Вывод:"
        echo "$output" | sed 's/^/    /'
        ((failed_tests++))
    fi
    
    echo "----------------------------------"
done

echo "Результаты:"
echo "Всего тестов: $total_tests"
echo "Успешных:     $passed_tests"
echo "Проваленных:  $failed_tests"

if [ "$failed_tests" -eq 0 ]; then
    echo "Все тесты прошли успешно!"
    exit 0
else
    echo "Найдены ошибки в тестах!"
    exit 1
fi