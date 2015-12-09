/**************************************************************************
 * NAME        : NILES
 * COURSE      : ENGE300
 * DATE        : DEC. 1st, 2007
 * DESCRIPTION : Encodes/Decodes a message using book code; prints out decoded message.
 *               Originally written for my undergraduate C programming class; revised since.
 * FILES       : "structs.h"     -- struct definitions used in program
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"

#if 0
#define DBG_ENCODE 1
#endif

#define nextargi (--argc,atoi(*++argv))        // Handle Command Line
#define nextargf (--argc,atof(*++argv))
#define nextargs (--argc,*++argv)

char * get_msg();

char * get_line();
char ** get_para();
char *** get_chap();
char **** get_book();

void print_line(char *line);
void print_para(char **para);
void print_chap(char ***chap);
void print_book(char ****book);

FILE *mpd;
FILE *mpe;
FILE *bp;

CODE * get_code(char ****book, char *msg);
int get_num_chapters();
int get_num_paragraphs();
int get_num_lines();
int get_num_characters();

int
main(int argc, char **argv)
{
  //pointers to elements of chosen book
  char ****book;
  //DECODING: ints for addresses of encoded message
  int chn;
  int prn;
  int lnn;
  int ltrn;
  //ENCODING: variables
  char *msg;    //holds message to be encoded
  CODE *pcode;  //declaration of pointer to struct of typedef CODE 
  CODE *head;   //pointers for data trail
  CODE *tail;   //pointers for data trail

  // init pointers
  book  = NULL;
  msg   = NULL;
  pcode = NULL;
  head  = NULL;
  tail  = NULL;

  // command line handling
  if(argc<2){        // too few command-line arguments?
    printf("Command-line usage:\n");
    printf("    %s [book] [messageToEncode] [messageToDecode]\n",argv[0]);
    printf("        book is the plain text book file to use as a codec\n");
    printf("        messageToEncode is a plain text message file to encode\n");
    printf("        messageToDecode is a book-encoded file to decode\n");
    exit(0);
  }
  char bookFname[512], msgEncFname[512], msgDecFname[512];	// file names for arg handling
  unsigned char encode=0, decode=0;
  strcpy(bookFname,nextargs);
  printf("Code book: %s\n", bookFname);  
  *msgEncFname='\0';
  *msgDecFname='\0';
  argc--;                               // may not be an awesome idea, but it works
  if( argc ){
      strcpy(msgEncFname,nextargs);
      encode=1;
      printf("Preparing to Encode: %s\n", msgEncFname);
  }
  if( argc ){
      strcpy(msgDecFname,nextargs);
      decode=1;
      printf("Preparing to Decode: %s\n", msgDecFname);
  }

  //get book
  bp = fopen(bookFname, "r");           //book pointer
  if( encode ){
      mpe = fopen(msgEncFname, "r");    //readable message pointer (to encode)
  }
  if( decode ){
      mpd = fopen(msgDecFname, "r");    //encoded message pointer (to decode)
  }

  //build book
  book = get_book(); 

  /* ENCODING PATH */
  if( encode )
  {
      //get message to be encoded
      msg = get_msg( mpe );

      while( (pcode = get_code(book, msg)) )
      {
        if(!head){              //create the linked list, start at head
          tail = pcode;
          head = tail;          //put head and tail on same linked list
        }
        else{
          tail->next = pcode;  //self-reference to make linked list
          tail = pcode; 
        }
      }

      //now print out the encoding (linked list); begin at head and proceed thru *next until 
      //tail is reached
      pcode = head;
      while(pcode)
      {
        printf("%d %d %d %d\n", pcode->chapter, pcode->paragraph, pcode->line, pcode->letter);
        pcode = pcode->next;
      }
#ifdef DBG_ENCODE
      //read encoded from program, print decoded
      pcode = head;
      while(pcode != NULL)
      {
        printf("%c", book[pcode->chapter][pcode->paragraph][pcode->line][pcode->letter]);
        pcode = pcode->next;
      }
      fflush(stdout);
      printf("\n");
#endif
  }

  /* DECODING PATH */
  if( decode )
  {
      //read encoded, print decoded
      while((fscanf(mpd, "%d %d %d %d", &chn, &prn, &lnn, &ltrn)) != EOF)
      {
        printf("%c", book[chn][prn][lnn][ltrn]);
      }
      fflush(stdout);
  }

  if ( NULL != msg ) free(msg);        //free the memory used for the message
  if ( NULL != book ) free(book);       //free the memory used for the book
  if ( NULL != pcode ) free(pcode);      //free the memory used for pcode

  return (0);

}

/****************************************************
 *FUNCTION   : get_code 
 *DESCRIPTION: finds a random character value assignment 
 *ARGUMENTS  : ****book, *msg 
 *RETURNS    : pcode -- pointer to struct code_struct
               NULL  -- on end of message 
 ***************************************************/
CODE *
get_code(char ****book, char *msg)
{
  CODE *pcode;          //pointer to struct of typedef CODE 
  static int i=0;      //static counter for message to encode
  int j = i;          //temp counter for conditionals
  //random indexing variables
  int numChapters   = get_num_chapters(book);
  int chapter       = (random() + 12) % numChapters;
  int numParagraphs = get_num_paragraphs(book[chapter]);
  int paragraph     = random() % numParagraphs;
  int numLines      = get_num_lines(book[chapter][paragraph]);
  int line          = random() % numLines;
  int numCharacters = get_num_characters(book[chapter][paragraph][line]);
  int character     = random() % numCharacters;
  //original index #'s are:
  int o_chap = chapter;
  int o_para = paragraph;
  int o_line = line;
  int o_char = character;
  //c is letter from the book 
  char c = book[chapter][paragraph][line][character];

  if(*(msg + i) == '\0'){   //this works! leave it alone!
    return NULL;           //good for EOF i.e. end of message
  }
  
  //proceed to find a match for the desired character
  if(c != *(msg + i++)){
    //printf("Not a match; do something\n");
    while(c != *(msg + j))
    { //using j counter--reqd since i++ above
      c = book[chapter][paragraph][line][++character];  //inc by 1 thru characters
      if(c == '\0'){               //end of line has been reached
        line++;
        character = 0;
      }
      //'equal to' because numLines is an integer count, whereas line is an array index
      if(line == (numLines = get_num_lines(book[chapter][paragraph]))){
        paragraph++;
        line      = 0;
        character = 0;
      }
      //'equal to' because numParagraphs is an integer count, whereas paragraph is an array 
      //index
      if(paragraph == (numParagraphs = get_num_paragraphs(book[chapter]))){
        chapter++;
        paragraph = 0;
        line      = 0;
        character = 0;
      }
      if((chapter == numChapters) || (c == EOF)){        //EOF handling
        chapter   = 0;
        paragraph = 0;
        line      = 0;
        character = 0;
      }
      //if original #'s, exit w/error
      if(o_chap == chapter)
        if(o_para == paragraph)
          if(o_line == line)
            if(o_char == character){
              printf("PUNT: required character does not exist in text\n");
              printf("Required character is %c\n", *(msg + j));
              exit (0);
            }
    }
  }
 
  //MATCH HAS NOW BEEN FOUND; malloc for struct pointer, assign members, return pointer
  //printf("MATCH FOUND!!!\n");

  pcode = (CODE *) malloc (sizeof(CODE));

  pcode->chapter   = chapter;
  pcode->paragraph = paragraph;
  pcode->line      = line;
  pcode->letter    = character;

  return (pcode);

}

/****************************************************
 *FUNCTION   : get_num_characters
 *DESCRIPTION: finds # of characters for a given line
 *ARGUMENTS  : *line 
 *RETURNS    : i -- count of characters
 ***************************************************/
int
get_num_characters(char *line)
{
  int i;

  for(i=0; *(line + i) != '\0'; i++){
  }

  return i;
}

/****************************************************
 *FUNCTION   : get_num_lines
 *DESCRIPTION: finds # of lines for a given paragraph
 *ARGUMENTS  : **paragraph 
 *RETURNS    : i -- count of lines
 ***************************************************/
int
get_num_lines(char **paragraph)
{
  int i;

  for(i=0; *(paragraph + i) != '\0'; i++){
  }

  return i;
}

/****************************************************
 *FUNCTION   : get_num_paragraphs
 *DESCRIPTION: finds # of paragraphs for a given chapter 
 *ARGUMENTS  : ***chapter
 *RETURNS    : i -- count of paragraphs
 ***************************************************/
int
get_num_paragraphs(char ***chapter)
{
  int i;

  for(i=0; *(chapter + i) != '\0'; i++){
  }

  return i;
}

/****************************************************
 *FUNCTION   : get_num_chapters
 *DESCRIPTION: determine the number of chapters in book 
 *ARGUMENTS  : ****book 
 *RETURNS    : i -- count of chapters
 ***************************************************/
int
get_num_chapters(char ****book)
{
  int i;

  for(i=0; *(book + i) != '\0'; i++){
  }

  return i;
}

/****************************************************
 *FUNCTION   : get_msg
 *DESCRIPTION: gets the message to be encoded
 *ARGUMENTS  : fh -- pointer to file resource
 *RETURNS    : msg  -- pointer to message 
 ***************************************************/
char *
get_msg( FILE * fh )
{
  char c; 
  char * msg;
  char tempmsg[2048];
  int  i=0;
  int  len=0;
  int  j;

  while( ((c=fgetc(fh)) != '\n') && (c != EOF) )
  {
    tempmsg[i++] = c;
  }

  tempmsg[i] = '\0';
  len = strlen(tempmsg);  //find length for malloc purposes
  
  msg = (char *) malloc((len+1) * sizeof(c));

  for(j=0; j <= len; j++)
  {
    msg[j] = tempmsg[j];
  }

  return (msg);

}

/****************************************************
 *FUNCTION   : get_book
 *DESCRIPTION: gets a book by calling getchapter
 *ARGUMENTS  : 
 *RETURNS    : book, NULL on EOF
 ***************************************************/
char ****
get_book()
{
  char **** book;
  char *** chap;           
  char *** tempbook[256];   //temp array to hold pointers to chaps
  int i=0; 
  int j; 
  int len; 
  
  while( (chap = get_chap()) )
  {
    tempbook[i++] = chap;
  }  

  if (!i) return NULL;

  tempbook[i] = NULL;
  len = i;

  book = (char ****) malloc((len+1) * sizeof(book));  

  for(j=0; j <= len; j++)
  {
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
char ***
get_chap()
{
  char ***chap;
  char **para;
  char **tempchap[256];   //temp array to hold pointers to paragraphs
  int i=0; 
  int j; 
  int len; 
  static char **nextchap = NULL; //holds the title "paragraph" of nextchapter

  if(nextchap){
    tempchap[i++] = nextchap;
    nextchap = NULL;
  }

  while( (para = get_para()) )
  {
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

  for(j=0; j <= len; j++)
  {
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
char **
get_para()
{
  char **para;
  char *line;
  char *temppara[256];   //temp array to hold pointers to lines
  int i=0; 
  int j; 
  int len; 

  while( (line = get_line()) )
  {
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

  for(j=0; j <= len; j++)
  {
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
char *
get_line()
{
  char *line;
  char c; 
  char templine[256]; 
  int len; 
  int i=0, j; 

  while(((c=fgetc(bp)) != '\n') && (c != EOF))
  {
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

  for(j=0; j <= len; j++)
  {
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
void
print_line(char *line)
{
  printf("%s",line);
}

/****************************************************
 *FUNCTION   : print_para
 *DESCRIPTION: prints a paragraph
 *ARGUMENTS  : **para -- pointer holding lines
 *RETURNS    : void 
****************************************************/
void
print_para(char **para)
{
  int i;

  //condition: para is a NULL terminated list of pointers to lines
  for(i=0; *(para + i) != '\0'; i++)
  {
    print_line(*(para + i));
  }

}

/****************************************************
 *FUNCTION   : print_chap
 *DESCRIPTION: prints a chapter
 *ARGUMENTS  : ***chap -- pointer list of paragraphs 
 *RETURNS    : void 
****************************************************/
void
print_chap(char ***chap)
{
  int i;

  //condition: chap is a NULL terminated list of pointers to paragraphs 
  for(i=0; *(chap + i) != '\0'; i++)
  {
    print_para(*(chap + i));
  }

}

/****************************************************
 *FUNCTION   : print_book
 *DESCRIPTION: prints a book
 *ARGUMENTS  : ****book -- pointer list of chapters 
 *RETURNS    : void 
****************************************************/
void
print_book(char ****book)
{
  int i;

  //condition: chap is a NULL terminated list of pointers to paragraphs 
  for(i=0; *(book + i) != '\0'; i++)
  {
    print_chap(*(book + i));
  }

}

