all:

Life.log:
	git log > Life.log

Doxyfile:
	doxygen -g

RunLife: Life.h Life.c++ RunLife.c++
	g++ -pedantic -std=c++11 -Wall Life.c++ RunLife.c++ -o RunLife

RunLife.tmp: RunLife
	RunLife < RunLife.in > RunLife.tmp
	diff RunLife.tmp RunLife.out

TestLife: Life.h Life.c++ TestLife.c++
	g++ -fprofile-arcs -ftest-coverage -pedantic -std=c++11 -Wall Life.c++ TestLife.c++ -o TestLife -lgtest -lgtest_main -lpthread

TestLife.out: TestLife
	valgrind TestLife     >  TestLife.out 2>&1
	gcov -b  Life.c++     >> TestLife.out
	gcov -b  TestLife.c++ >> TestLife.out

html: Doxyfile Life.h Life.c++ RunLife.c++ TestLife.c++
	doxygen Doxyfile

life-tests:
	git clone https://github.com/cs371p-spring-2015/life-tests.git

check:
	@[ -d html                        ] && echo "html/                       found" || echo "html/                       NOT FOUND"
	@[ -e life-tests/EID-RunLife.in   ] && echo "life-tests/EID-RunLife.in   found" || echo "life-tests/EID-RunLife.in   NOT FOUND"
	@[ -e life-tests/EID-RunLife.out  ] && echo "life-tests/EID-RunLife.out  found" || echo "life-tests/EID-RunLife.out  NOT FOUND"
	@[ -e life-tests/EID-TestLife.c++ ] && echo "life-tests/EID-TestLife.c++ found" || echo "life-tests/EID-TestLife.c++ NOT FOUND"
	@[ -e life-tests/EID-TestLife.out ] && echo "life-tests/EID-TestLife.out found" || echo "life-tests/EID-TestLife.out NOT FOUND"
	@[ -e Life.c++                    ] && echo "Life.c++                    found" || echo "Life.c++                    NOT FOUND"
	@[ -e Life.h                      ] && echo "Life.h                      found" || echo "Life.h                      NOT FOUND"
	@[ -e Life.pdf                    ] && echo "Life.pdf                    found" || echo "Life.pdf                    NOT FOUND"
	@[ -e Doxyfile                    ] && echo "Doxyfile                    found" || echo "Doxyfile                    NOT FOUND"
	@[ -e RunLife.c++                 ] && echo "RunLife.c++                 found" || echo "RunLife.c++                 NOT FOUND"
	@[ -e RunLife.out                 ] && echo "RunLife.out                 found" || echo "RunLife.out                 NOT FOUND"
	@[ -e TestLife.c++                ] && echo "TestLife.c++                found" || echo "TestLife.c++                NOT FOUND"
	@[ -e TestLife.out                ] && echo "TestLife.out                found" || echo "TestLife.out                NOT FOUND"

clean:
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	rm -f RunLife
	rm -f RunLife.tmp
	rm -f TestLife
	rm -f TestLife.out
