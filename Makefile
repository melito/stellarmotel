parser:
	clang h264.c examples/parser.c -Wall -o build/parser

make clean:
	rm build/*
