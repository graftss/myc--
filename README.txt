Running `make` builds the parser at `myc--`.

Example programs are found in the `programs` directory and
their corresponding example executions are found in the
`execution` directory.

Language features:
  - numeric, character, boolean, and string literals
  - common numeric and boolean operations
  - string concatenation with +
  - for, while, do/while loops
  - if/then/else statements
  - functions
  - single-dimensional arrays

Weaknesses:
  - Statement blocks, such as the bodies of loop and branch statements, 
    must be enclosed with { and } braces.
    
  - There is a single global scope shared by all functions.
  
  - Return statements are not passed upwards through nested blocks. In other
    words, any `return` must be in the outermost block, so that the following 
    wouldn't work:
    
    int sign(int x) {
      if (x > 0) { return 1; }
      if (x == 0) { return 0; }
      return -1;
    }
    
    It would have to be implemented like this:
    
    int sign(int x) {
      int result;
      if (x > 0) { result = 1; }
      if (x == 0) { result = 0; }
      result = -1;
      
      return result;
    }
    
  - Types are essentially meaningless as written, as they aren't being 
    checked by the grammar. For simplicity, therefore, I was using the `void`
    type to refer to function or array values that didn't have a simple-to-parse
    type.