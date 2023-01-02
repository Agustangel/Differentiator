
INCLUDES=include
SOURCE=source

diff: main.o diff.o diff_tree.o
	gcc -o diff $^ -llogger -lonegin -lm

main.o: main.c
	gcc -g -O0 -I${INCLUDES}/ -c $^

diff.o: ${SOURCE}/diff.c
	gcc -g -O0 -I${INCLUDES}/ -c $^

diff_tree.o: ${SOURCE}/diff_tree.c
	gcc -g -O0 -I${INCLUDES}/ -c $^

valgrind: diff
	valgrind --leak-check=yes ./diff

clean:
	rm diff main.o diff.o