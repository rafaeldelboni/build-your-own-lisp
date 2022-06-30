setup:
	cmake -DIS_SETUP=ON -DCMAKE_BUILD_TYPE=Debug -B build/debug
	cmake -DIS_SETUP=ON -DCMAKE_BUILD_TYPE=Release -B build/release
.PHONY: setup

build:
	cmake -DBUILD_TESTING=OFF -DIS_SETUP=OFF -DCMAKE_BUILD_TYPE=Debug -B build/debug
	make --no-print-directory -C build/debug -j
.PHONY: build

release:
	cmake -DBUILD_TESTING=OFF -DIS_SETUP=OFF -DCMAKE_BUILD_TYPE=Release -B build/release
	make --no-print-directory -C build/release -j
.PHONY: release

test:
	cmake -DIS_SETUP=ON -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug -B build/debug
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
