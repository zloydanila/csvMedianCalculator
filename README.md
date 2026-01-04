# csv_median_calculator

Консольное приложение для чтения CSV с биржевыми данными (`level.csv`/`trade.csv`) и расчёта инкрементальной медианы по полю `price`.  
Результат записывается в `median_result.csv` в формате `receive_ts;price_median`, причём строка добавляется только если медиана изменилась.

## Требования

- Компилятор C++23.
- CMake >= 3.23.
- Boost (используется `program_options`).
- Зависимости (через CMake FetchContent): `toml++`, `spdlog`, `Catch2`.

## Сборка

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure

