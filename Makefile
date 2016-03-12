parser:
	clang h264.c examples/parser.c -Wall -o build/parser
params:
	clang h264.c examples/parse_params.c -Wall -o build/parse_params
convert:
	clang h264.c examples/convert.c -Wall -o build/convert

make clean:
	rm build/*
