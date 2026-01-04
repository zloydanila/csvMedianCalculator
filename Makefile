.PHONY: build test run docker-test check ci

build:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build

test:
	ctest --test-dir build --output-on-failure

run:
	./build/csv_median_calculator -config ./examples/config.toml
	@echo "Output:"
	@cat ./examples/output/median_result.csv

docker-test:
	docker build -t csv-median-calculator .
	docker run --rm csv-median-calculator

ci:
	docker build -t csv-median-calculator:ci .

check: build test run docker-test
