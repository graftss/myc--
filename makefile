
CCC = g++
CCFLAGS= -O2 -std=c++11
LEX = flex
LFLAGS= -8     
YACC= bison 
YFLAGS= -d -t -y

RM = /bin/rm -f

myc--: y.tab.o lex.yy.o myc--.o
	${CCC} ${CCFLAGS} lex.yy.o y.tab.o myc--.o -o myc-- 

myc--.o: myc--.cpp myc--.h
	${CCC} ${CCFLAGS} -c myc--.cpp
y.tab.o: myc--.y
	${YACC} ${YFLAGS} myc--.y
	${CCC} ${CCFLAGS} y.tab.c -c 

lex.yy.o: myc--.l
	${LEX} $(LFLAGS) myc--.l
	${CCC} ${CCFLAGS} lex.yy.c -c 

clean:
	/bin/rm -f lex.yy.* y.tab.* *.o myc--

runTest: myc--
	./myc-- < programs/test
  
runTest2: myc--
	./myc-- < programs/loop