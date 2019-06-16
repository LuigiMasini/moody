ifeq ($(OS),Windows_NT)
all:
	echo not supported
else
all:
	$(CC) diff.c file_lines_count.c -o moody
debug:
	$(CC) -D DEBUG diff.c file_lines_count.c -o moody
endif