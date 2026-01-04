csv_median_calculator
====================

Консольное приложение для чтения CSV с биржевыми данными (level.csv / trade.csv)
и расчёта инкрементальной медианы по полю price.
Результат записывается в CSV median_result.csv в формате:
receive_ts;price_median
Новая строка добавляется только если медиана изменилась.


Требования
----------

- C++23
- CMake >= 3.23
- Boost (program_options)
- Зависимости через CMake FetchContent: toml++, spdlog, Catch2


Сборка и тесты (native)
-----------------------

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure


Запуск
------

1) Запуск с конфигом:

./build/csv_median_calculator -config ./examples/config.toml
или
./build/csv_median_calculator -cfg ./examples/config.toml

2) Запуск без аргументов:
Если ключи не заданы, приложение ищет config.toml рядом с бинарём.

cp ./examples/config.toml ./build/config.toml
./build/csv_median_calculator


Конфигурация (TOML)
-------------------

examples/config.toml:

[main]
input = "examples/input"
output = "examples/output"
filename_mask = ["level", "trade"]

- input: директория со входными CSV
- output: директория для выходного CSV
- filename_mask: какие типы файлов обрабатывать (level, trade или оба)


Формат входных CSV
------------------

trade.csv
- Колонки: receive_ts;exchange_ts;price;quantity;side
- side: bid/ask

level.csv
- Колонки: receive_ts;exchange_ts;price;quantity;side;rebuild
- rebuild: 0 или 1


Формат выхода
-------------

median_result.csv
- Колонки: receive_ts;price_median
- price_median выводится с фиксированной точностью (пример в ТЗ — 8 знаков после точки)
- строка добавляется только при изменении медианы


Docker
------

Сборка и запуск в чистом окружении:

make docker-test


Полная проверка одной командой
------------------------------

make check
