/**************************************************************************
 * NAME        : NILES
 * COURSE      : ENGE300
 * DATE        : NOV. 15th, 2007
 * DESCRIPTION : Decodes a book code and prints the decoded message
 * FILES       : "orthodoxy.txt" -- book to read in
 *               "encoded_message.txt" -- message to decode 
*
* Program Grading Criteria
* 
* PROGRAM CORRECTNESS
* a) the program listing is complete and correct    (10)_________
* b) verification tests are complete and documented (20)_________
* c) program flowchart is complete and correct      (20)_________
* c) functions correctly                            (80)_________
*
* PROGRAM DESIGN
* a) variable names/types are well chosen           (10)_________
* b) modularity/parameters are passed appropriately (10)_________
* c) overall logic                                  (20)_________
*
* PROGRAM READABILITY AND DOCUMENTATION
* a) header comments are complete and clear         (10)_________
* b) internal comments are complete and clear       (10)_________
* c) white space and indents are used appropriately (10)_________
*
*                                           TOTAL  (200)_________
 * 
**************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

char * get_line();
char ** get_para();
char *** get_chap();
char **** get_book();

void print_line(char *line);
void print_para(char **para);
void print_chap(char ***chap);
void print_book(char ****book);

FILE *mp;
FILE *bp;

main(){

  //pointers to elements of chosen book
  char *line; 
  char **para;
  char ***chap;
  char ****book;
  //ints for addresses of encoded message
  int chn;
  int prn;
  int lnn;
  int ltrn;

  mp = fopen("./encoded_message.txt", "r");  //encoded message pointer
  bp = fopen("./orthodoxy.txt", "r");       //book pointer
  
  //build book
  book = get_book(); 

  //read encoded, print decoded
  while((fscanf(mp, "%d %d %d %d", &chn, &prn, &lnn, &ltrn)) != EOF){
    printf("%c", book[chn][prn][lnn][ltrn]);
    fflush(stdout);
  }

  free(book);        //free the memory used for the book

}


/****************************************************
 *FUNCTION   : get_book
 *DESCRIPTION: gets a book by calling getchapter
 *ARGUMENTS  : 
 *RETURNS    : book, NULL on EOF
 ***************************************************/

char **** get_book(){
  
  char **** book;
  char *** chap;           
  char *** tempbook[255];   //temp array to hold pointers to chaps
  int i=0; 
  int j; 
  int len; 
  
  while(chap = get_chap()){  
    tempbook[i++] = chap;
  }  

  if (!i) return NULL;

  tempbook[i] = NULL;
  len = i;

  book = (char ****) malloc((len+1) * sizeof(book));  

  for(j=0; j <= len; j++){
    book[j] = tempbook[j];
  }

  return (book);

}


/****************************************************
 *FUNCTION   : get_chap
 *DESCRIPTION: gets a chapter by calling getpara 
 *ARGUMENTS  :  
 *RETURNS    : chap, NULL on EOF
 ***************************************************/

char *** get_chap(){

  char ***chap;
  char **para;
  char **tempchap[255];   //temp array to hold pointers to paragraphs
  int i=0; 
  int j; 
  int len; 
  static char **nextchap = NULL; //holds the title "paragraph" of nextchapter

  if(nextchap){
    tempchap[i++] = nextchap;
    nextchap = NULL;
  }

  while(para = get_para()){  
    if(!isspace(**para) && i){         //if the first col char is text, then
      nextchap = para;           //static store addr of new chap para
      break;                  //break because new chapter has begun
    }
    else{
      tempchap[i++] = para;
    }
  }

  if(!i) return NULL;
  
  tempchap[i] = NULL;
  len = i;               //length of pointer list

  chap = (char ***) malloc((len+1) * sizeof(char ***));

  for(j=0; j <= len; j++){
    chap[j] = tempchap[j];
  }

  return (chap);

}

/****************************************************
 *FUNCTION   : get_para
 *DESCRIPTION: gets a paragraph by calling getline 
 *ARGUMENTS  :  
 *RETURNS    : para, NULL on EOF
 ***************************************************/

char ** get_para(){

  char **para;
  char *line;
  char *temppara[255];   //temp array to hold pointers to lines
  int i=0; 
  int j; 
  int len; 

  while(line = get_line()){  
    if(*line == '\n'){
      temppara[i++] = line;
      break;       //need this to let the rest of getpara run
    }
    else{
      temppara[i++] = line;
    }
  }  

  if (!i) return NULL;

  temppara[i] = NULL;
  len = i;

  para = (char **) malloc((len+1) * sizeof(char **));

  for(j=0; j <= len; j++){
    para[j] = temppara[j];
  }

  return (para);

}


/****************************************************
 *FUNCTION   : get_line
 *DESCRIPTION: gets a line from stdin and transfers it to a pointer 
 *ARGUMENTS  :  
 *RETURNS    : line, NULL on EOF
 ***************************************************/

char * get_line(){

  char *line;
  char c; 
  char templine[255]; 
  int len; 
  int i=0, j; 

  while(((c=fgetc(bp)) != '\n') && (c != EOF)){  
    templine[i++] = c;
  }

  if (c == EOF) {           //omitted to make get_para work; needs to return
    if (!i) return NULL;    //an EOF in a line and then read it
  }
  if (c == '\n') {
    templine[i++] = c;
  }

  templine[i] = '\0';
  len = strlen(templine);       //find length for malloc purposes

  line = (char *) malloc((len+1) * sizeof(char));  //malloc w/room for NULL

  for(j=0; j <= len; j++){
    *(line + j) = templine[j];       //populate pointers list
  }

  return (line);        //return list of pointers

}

/****************************************************
 *FUNCTION   : print_line
 *DESCRIPTION: prints a line from a pointer 
 *ARGUMENTS  : *line -- pointer holding string of chars 
 *RETURNS    : void 
****************************************************/

void print_line(char *line){

  printf("%s",line);

}

/****************************************************
 *FUNCTION   : print_para
 *DESCRIPTION: prints a paragraph
 *ARGUMENTS  : **para -- pointer holding lines
 *RETURNS    : void 
****************************************************/

void print_para(char **para){

  int i;

  for(i=0; *(para + i) != '\0'; i++){   //condition: para is a NULL terminated list of pointers to lines
    print_line(*(para + i));
  }

}

/****************************************************
 *FUNCTION   : print_chap
 *DESCRIPTION: prints a chapter
 *ARGUMENTS  : ***chap -- pointer list of paragraphs 
 *RETURNS    : void 
****************************************************/

void print_chap(char ***chap){

  int i;

  for(i=0; *(chap + i) != '\0'; i++){   //condition: chap is a NULL terminated list of pointers to paragraphs 
    print_para(*(chap + i));
  }

}

/****************************************************
 *FUNCTION   : print_book
 *DESCRIPTION: prints a book
 *ARGUMENTS  : ****book -- pointer list of chapters 
 *RETURNS    : void 
****************************************************/

void print_book(char ****book){

  int i;

  for(i=0; *(book + i) != '\0'; i++){   //condition: chap is a NULL terminated list of pointers to paragraphs 
    print_chap(*(book + i));
  }

}

/****************************************************
//MODULE FOR TESTING getbook AND printbook
main(){ 

  char *line; 
  char **para;
  char ***chap;
  char ****book;

  while(book = get_book()){
    print_book(book);
    if(book){
      free(book);
    }
  }

}
****************************************************/

/****************************************************
main(){      //MODULE FOR TESTING getchap AND printchap

  char *line; 
  char **para;
  char ***chap;

  while(chap = get_chap()){
    print_chap(chap);
    if(chap){
      free(chap);
    }
  }

}
****************************************************/

/****************************************************
main(){      //MODULE FOR TESTING getpara AND printpara

  char *line; 
  char **para; 

  while(para = get_para(para, line)){ 
    print_para(para, line);
    free(para);
  }
}
*****************************************************/

/****************************************************
main(){     //MODULE FOR TESTING getline AND printline

  char *line; 

  while(line = get_line()){
    print_line(line);
  }

}
****************************************************/

