
.PHONY: cmake build 
cmake:
	cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=y
build:
	make -C build
send:
	./build/chaat 1234 -s 4321
recv:
	./build/chaat 4321 -r
