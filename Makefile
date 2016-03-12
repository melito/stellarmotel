parser:
	clang h264.c examples/parser.c -Wall -o build/parser
params:
	clang h264.c examples/parse_params.c -Wall -o build/parse_params
convert:
	clang h264.c examples/convert.c -Wall -o build/convert
streaming_read:
	clang h264.c examples/streaming_read.c -Wall -o build/streaming_read

make clean:
	rm build/*
