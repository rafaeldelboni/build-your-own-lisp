setup:
	cmake -DCMAKE_BUILD_TYPE=Debug -B build/debug
	cmake -DCMAKE_BUILD_TYPE=Release -B build/release
.PHONY: setup

build:
	make --no-print-directory -C build/debug -j
.PHONY: build

release:
	make --no-print-directory -C build/release -j
.PHONY: release

test: export BUILD_TESTING = ON
test:
	make --no-print-directory -C build/debug -j
	make --no-print-directory -C build/debug test ARGS='--output-on-failure'
.PHONY: test

clean:
	make --no-print-directory -C build/debug clean
	make --no-print-directory -C build/release clean
.PHONY: clean

run:
ifneq (,$(findstring t,$(bin)))
	make --no-print-directory -C build/debug -j
	./build/debug/bin/$(bin)
else
	make --no-print-directory -C build/debug -j
	./build/debug/bin/main
endif
.PHONY: run
