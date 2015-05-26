%{
#include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 int k=0;
 int j=-1;
 char* total[100];
 int tot[1000]={0};
void count(char*);
%}

digits [0-9]
numbers {digits}+
realnumbers {digits}*"."{digits}+
char '(\\.)*'
strings \"([^\\\"]|\\.)*\"
characters [a-zA-Z ]+
charandnumbers [a-zA-Z0-9]
conditionalstatements "if"|"else"|"else if"|"switch"|"case"
keywords "int"|"float"|"char"|"string"|"unsigned"|"signed"|"short"|"long"|"double"|"break"|"continue"|"goto"|"sprintf"|"sscanf"|"fopen"|"fwrite"|"fread"|"fclose"|"write"|"read"|"open"|"close"|"return"
loops "for"|"while"|"do"
preprossesors "#"|"#line"|"#undef"|"#error"|"#elif"|"#else"|"#endif"|"#if"|"#define"|"#include"|"#pragma"|"#ifndef"|"#ifdef"
delimiters [; :\t\n()"]
identifiers [a-zA-Z]{charandnumbers}*|[a-zA-Z]{charandnumbers}*[[{numbers}+]]
nonidentifiers {numbers}+[A-Za-z]+
formatspecifiers "%"{charandnumbers}+
files "<"{identifiers}.h">"
comments "/*"[a-zA-Z0-9 \t\n;.~!@#$%^&*()_+=<>?:"{}]*"*/"
operators "+"|"-"|"*"|"/"|"="
startbrace "{"
endbrace "}"
ignore .|\n 
unaryoperators "++"|"--"
loperators "&"|"|"|"&&"|"~"|"||"|">"|"<"|">="|"<="|"=="
function {identifiers}+"("{delimiters}*{characters}{charandnumbers}*{delimiters}*")"
%option noyywrap
%%
{conditionalstatements} {}
{loops} {}
{digits} { count(yytext); }
{numbers} { count(yytext); }
{realnumbers} { count(yytext); }
{preprossesors} {}
{delimiters} {}
{keywords} {}
{strings}   {count(yytext);}
{nonidentifiers} {}
{identifiers} { count(yytext); }
{comments} {}
{operators} {}
{loperators} {}
{startbrace} {}
{endbrace} {}
{files} {}
{unaryoperators} {}
{function} {}
{ignore} {}
{formatspecifiers} {}
%%
void count(char* str)
{
  int l=0; 
  int m=0;  
for(l=0;l<j;l++)
  {
   if(strcmp(total[l],str)==0) 
   {   tot[l]++;
       m=1; } 
   } 
if(m==0)
 {  total[j+1] = (char*)malloc(100);
    strcpy(total[j+1],str);
    j++;
	tot[j]++;
 }
} 
int main(int argc, char *argv[]) {
	extern FILE * yyin;
if(argc > 1) {
        yyin  = fopen(argv[1], "r");
		 
    }
    yylex();
    int m=0;
	yyout = fopen("ot.txt", "a");
    for(m=0;m<j+1;m++){
      fprintf(yyout,"%s with frequency :%d\n", total[m],tot[m]);
    }
    fclose(yyin);
    fclose(yyout);
return 0;
}
