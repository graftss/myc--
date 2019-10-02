
CCC = g++
CCFLAGS= -O2
LEX = flex
LFLAGS= -8     
YACC= bison 
YFLAGS= -d -t -y

RM = /bin/rm -f

myc--: y.tab.o lex.yy.o myc--.o
	${CCC} ${CCFLAGS} lex.yy.o y.tab.o myc--.o -o myc-- 

myc--.o: myc--.cpp myc--.h
	${CCC} -c myc--.cpp
y.tab.o: myc--.yacc
	${YACC} ${YFLAGS} myc--.yacc
	${CCC} ${CCFLAGS} y.tab.c -c 

lex.yy.o: myc--.lex
	${LEX} $(LFLAGS) myc--.lex
	${CCC} ${CCFLAGS} lex.yy.c -c 

clean:
	/bin/rm -f lex.yy.* y.tab.* *.o myc--
