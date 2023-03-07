#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
//#include <grapheme.h>
#include "lexer.h"
char* s = "নাম";
char* x = "()";
int main(){
	/*size_t ret , off;
	for (off = 0; s[off] != '\0'; off += ret) {
		ret = grapheme_next_character_break_utf8(s + off, SIZE_MAX);
		
		printf("%.*s\n" , (int)ret ,s + off);
	}*/

	init_lexer(x);
	lex();

}
