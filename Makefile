parser:
	clang  h264.c parser.c -Wall -o build/parser

make clean:
	rm build/*
