%{

%type <stmt> stmt
  | expr PLUS expr { $$ = new NBinaryNumOp($1, $3, OP_PLUS); }
  | expr MINUS expr { $$ = new NBinaryNumOp($1, $3, OP_MINUS); }
  | expr TIMES expr { $$ = new NBinaryNumOp($1, $3, OP_TIMES); }
  | expr DIVIDE expr { $$ = new NBinaryNumOp($1, $3, OP_DIVIDE); }
  
  cout << endl << endl << "final state of program" << endl << endl;
  for (map<string,float>::iterator it=state.begin(); it!=state.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';