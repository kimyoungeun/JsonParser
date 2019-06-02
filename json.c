#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define TOKEN_COUNT 1024

typedef enum {
    UNDEFINED = 0,
    OBJECT = 1,
    ARRAY = 2,
    STRING = 3,
    PRIMITIVE = 4
}type_t;

typedef struct {
    type_t type;
    int start;
    int end;
    int size;
    char *string;
}tok_t;

typedef struct _JSON{
    tok_t tokens[TOKEN_COUNT];
} JSON;

char* readfile(char* filename, int* filesize); //read file name, contents
void array_parse(char*, JSON*, int*, int*); //array parsing function
void object_parse(char*, JSON*, int*, int*); //object parsing function
void json_parse(char *doc, int size, JSON *json, int *b_cnt); //json parsing function
void freeJson(JSON *json, int totalcnt); //json free function
void result(JSON *json, int totalcnt); //json string result print function

void categoryPrint(JSON *json, char category[10], int totalcnt);
void searchValue(JSON *json,char search[100], int totalcnt);

void printDoneYes(JSON *json, int totalcnt);
void printDoneNo(JSON *json, int totalcnt);

void print_importance1(JSON *json, int totalcnt);
void print_importance2(JSON *json, int totalcnt);
void print_importance3(JSON *json, int totalcnt);
void print_importance4(JSON *json, int totalcnt);
void print_importance5(JSON *json, int totalcnt);

void print_yes_proportion(JSON *json, int totalcnt);

char* readfile(char* filename, int* filesize)
{
    FILE* fp = fopen(filename, "r");
    if (fp ==  NULL){
        return NULL;
    }

    int size;
    char* buffer;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = (char*)malloc(size+1);
    memset(buffer, 0, size+1);

    if(fread(buffer,1, size, fp)<1){
        *filesize = 0;
        free(buffer);
        fclose(fp);
        return NULL;
    }

    *filesize = size;
    fclose(fp);
    return buffer;
}

void array_parse(char *doc, JSON *json, int *initialPos, int *initialTokenIndex)
{
    int pos = *initialPos;
    int tokenIndex = *initialTokenIndex;
    int s, e;
    int arraytokenIndex = tokenIndex;        
    json->tokens[tokenIndex].type = ARRAY;   
    tokenIndex++;                            
    s = pos;                                
    json->tokens[arraytokenIndex].start = s; 
    int arrSize = 0;
    while (doc[pos] != ']')
    { 
        pos++;
        switch (doc[pos])
        {
        case '"':
            arrSize++;                              
            json->tokens[tokenIndex].type = STRING; 
            s = pos + 1;                            
            json->tokens[tokenIndex].start = s;     
            while (doc[pos + 1] != '"'){
                pos++;
            }
            e = ++pos;                        
            json->tokens[tokenIndex].end = e;  
            json->tokens[tokenIndex].size = 0; 
            json->tokens[tokenIndex].string = (char *)malloc(e - s + 1);
            memset(json->tokens[tokenIndex].string, 0, e - s + 1);
            memcpy(json->tokens[tokenIndex].string, doc + s, e - s);
            tokenIndex++; //increase tokenIndex for element found insdie array
            break;

        case '[':
            arrSize++;
            array_parse(doc, json, &pos, &tokenIndex);
            break;

        case '{':
            arrSize++;
            object_parse(doc, json, &pos, &tokenIndex);
            break;

        case '-': case '0': case '1': case '2':
        case '3': case '4': case '5': case '6':
        case '7': case '8': case '9': case 't':
        case 'f': case 'n':
            arrSize++;
            json->tokens[tokenIndex].type = PRIMITIVE;
            s = pos;
            json->tokens[tokenIndex].start = s;
            while (doc[pos + 1] != ','){
                if (doc[pos + 1] == '\n') break;
                else pos++;
            }
            e = ++pos; // the word ends when doc[pos] meets ',' or NULL
            json->tokens[tokenIndex].end = e;
            json->tokens[tokenIndex].size = 0;
            json->tokens[tokenIndex].string = (char *)malloc(e - s + 1);
            memset(json->tokens[tokenIndex].string, 0, e - s + 1);
            memcpy(json->tokens[tokenIndex].string, doc + s, e - s);
            pos++;
            tokenIndex++;
            break;

        default:
            break;
        }
    }
    e = ++pos;
    json->tokens[arraytokenIndex].end = e;
    json->tokens[arraytokenIndex].size = arrSize;
    json->tokens[arraytokenIndex].string = (char *)malloc(e - json->tokens[arraytokenIndex].start + 1);
    memset(json->tokens[arraytokenIndex].string, 0, e - json->tokens[arraytokenIndex].start + 1);
    memcpy(json->tokens[arraytokenIndex].string, doc + json->tokens[arraytokenIndex].start, e - json->tokens[arraytokenIndex].start);
    *initialTokenIndex = tokenIndex;
    *initialPos = pos;
}

void object_parse(char *doc, JSON *json, int *initialPos, int *initialTokenIndex)
{   
    int pos = *initialPos;
    int tokenIndex = *initialTokenIndex;
    int s, e;
    int objtokenIndex = tokenIndex; 
    json->tokens[tokenIndex].type = OBJECT; 
    tokenIndex++;                           
    s = pos;                                
    json->tokens[objtokenIndex].start = s;  
    int objSize = 0;
    
    while (doc[pos] != '}')
    { 
        pos++;
        switch (doc[pos])
        {
        case '"':
            json->tokens[tokenIndex].type = STRING;
            s = pos + 1;                          
            pos++;
            json->tokens[tokenIndex].start = s;     
            while (doc[pos] != '"'){
                pos++;
            }
            e = pos;                         
            json->tokens[tokenIndex].end = e;  
            json->tokens[tokenIndex].size = 0; 
            while (doc[pos] != ':' && doc[pos] != '\n'){
                pos++;
            }
            if (doc[pos] == ':') json->tokens[tokenIndex].size = 1;
            else objSize++;

            json->tokens[tokenIndex].string = (char *)malloc(e - s + 1);
            memset(json->tokens[tokenIndex].string, 0, e - s + 1);
            memcpy(json->tokens[tokenIndex].string, doc + s, e - s);
            tokenIndex++; //increase tokenIndex for element found insdie array
            break;

        case '[':
            objSize++;
            array_parse(doc, json, &pos, &tokenIndex);
            break;

        case '{':
            objSize++;
            object_parse(doc, json, &pos, &tokenIndex);
            break;

        case '-': case '0': case '1': case '2':
        case '3': case '4': case '5': case '6':
        case '7': case '8': case '9': case 't':
        case 'f': case 'n':
            json->tokens[tokenIndex].type = PRIMITIVE;
            s = pos;
            json->tokens[tokenIndex].start = s;
            while (doc[pos + 1] != ','){
                if (doc[pos + 1] == '\n') break;
                else pos++;
            }
            objSize++;
            e = ++pos; // the word ends when doc[pos] meets ',' or NULL
            json->tokens[tokenIndex].end = e;
            json->tokens[tokenIndex].size = 0;
            json->tokens[tokenIndex].string = (char *)malloc(e - s + 1);
            memset(json->tokens[tokenIndex].string, 0, e - s + 1);
            memcpy(json->tokens[tokenIndex].string, doc + s, e - s);
            pos++;
            tokenIndex++;
            break;

        default:
            break;

        }
    }
    e = ++pos;
    json->tokens[objtokenIndex].end = e;
    json->tokens[objtokenIndex].size = objSize;
    //put doc[s]~doc[e+1] in token.string for the array
    json->tokens[objtokenIndex].string = (char *)malloc(e - json->tokens[objtokenIndex].start + 1);
    memset(json->tokens[objtokenIndex].string, 0, e - json->tokens[objtokenIndex].start + 1);
    memcpy(json->tokens[objtokenIndex].string, doc + json->tokens[objtokenIndex].start, e - json->tokens[objtokenIndex].start);
    *initialTokenIndex = tokenIndex;
    *initialPos = pos;
}

void json_parse(char *doc, int size, JSON *json, int *b_cnt)
{
    int cnt = 0;
    int pos = 0; //for checking position in doc.
    int e,s; //ending, starting position for each token
    int tokenIndex = 0; //index for token

    while(pos < size)
    {
        switch(doc[pos])
        {
            case '"':
            	json->tokens[tokenIndex].type = STRING; // token.type is STRING
            	s = pos + 1;
            	pos++;                                   // the word starts after "
            	json->tokens[tokenIndex].start = s;     // token.start = s
            	while (doc[pos] != '"'){ 
            	    pos++;
            	}

            	e = pos;                        // the word ends when doc[pos] meets ". (includes last ")
            	json->tokens[tokenIndex].end = e; // token.end = e
            	json->tokens[tokenIndex].size = 0; //if : is coming right after "" {size = 1}
            	while (doc[pos] != ':' && doc[pos] != '\n') {
            	    pos++;
            	}
            	if (doc[pos] != ':')
            	{ // else {size = 0}
            	    json->tokens[tokenIndex].size = 1;
            	}
            	json->tokens[tokenIndex].string = (char *)malloc(e - s + 1);
            	memset(json->tokens[tokenIndex].string, 0, e - s + 1);
            	memcpy(json->tokens[tokenIndex].string, doc + s, e - s);
	
                pos++;
           	tokenIndex++;
           	break;

             
            case '[':
                array_parse(doc, json, &pos, &tokenIndex);
                break;


            case '{':
                object_parse(doc, json, &pos, &tokenIndex);
                break;

            case '-': case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            case 't': case 'f': case 'n':
                json->tokens[tokenIndex].type = PRIMITIVE;
                s = pos;
                json->tokens[tokenIndex].start = s;
                while(doc[pos+1] != ',') { 
                    if(doc[pos+1] == '\n') break;
                    else pos++;
                }
                e = ++pos; 
                json->tokens[tokenIndex].end = e;
                json->tokens[tokenIndex].size = 0;
                json->tokens[tokenIndex].string = (char *)malloc(e-s+1);
                memset(json->tokens[tokenIndex].string, 0, e-s+1);
                memcpy(json->tokens[tokenIndex].string, doc+s, e-s);
                pos++;
                tokenIndex++;
                break;            

            default:
             pos++;
        }
    }
    *b_cnt = tokenIndex;
}

void freeJson(JSON *json, int totalcnt){
    for (int i = 0; i<totalcnt; i++){
        if (json->tokens[i].type == STRING)
            free(json->tokens[i].string);
    }
}

void result(JSON *json, int totalcnt) {
    char *typetype;
    for(int i = 0; i<totalcnt; i++){
        if(json->tokens[i].type == 0) typetype = "JSMN_UNDEFINED";
        else if(json->tokens[i].type == 1) typetype = "JSMN_OBJECT";
        else if(json->tokens[i].type == 2) typetype ="JSMN_ARRAY";
        else if(json->tokens[i].type == 3) typetype ="JSMN_STRING";
        else typetype = "JSMN_PRIMITIVE";
        printf("[%02d] %s (size=%d, %d~%d, %s)\n", i, json->tokens[i].string, json->tokens[i].size, json->tokens[i].start, json->tokens[i].end, typetype);
    }
}

int main(int argc, char** argv)
{
    int filesize=0;
    char* doc = readfile(argv[1], &filesize);
    int totalcnt=0;

    if(doc == NULL){
        return -1;
    }

    JSON json = {0, };
    json_parse(doc, filesize, &json, &totalcnt);
    result(&json, totalcnt);
    
    printf("\n-----------------Token Print End-----------------\n");
    printf("\nBucket List Manager\n");
    printf("It will help you mange bucket list efficiently\n");
    char select = ' ';
    do{
        printf("\n");
        printf("Menu List\n");
        printf("a: 버킷리스트 검색\n");
        printf("b: 카테고리\n");
        printf("c: 중요도\n");
        printf("d: 이행여부\n");
        printf("원하시는 메뉴 번호를 입력하세요(q를 입력하면 종료됩니다.): ");
        scanf("%c", &select);
        getchar();
       
            switch (select) {
                case 'a':
                    printf("\n");
                    char search[100];
                    printf("list에서 찾고자하는 항목을 입력하세요: ");
                    fgets(search, sizeof(search), stdin);
                    search[strlen(search)-1] ='\0';
                    searchValue(&json, search, totalcnt);
                    printf("\n");
                    break;
                    
                case 'b':
                    printf("\n");
                    char category[10];
                    printf("당신이 원하는 category는 무엇입니까? 입력하세요: ");
                    scanf("%s", category);
                    categoryPrint(&json, category, totalcnt);
                    printf("\n");
                    getchar();
                    break;
                    
                case 'c':
                    printf("\n");
                    char importance= ' ';
                    printf("input the number for selecting importance (1~5): ");
                    scanf("%c",&importance);
                    getchar();
                        switch(importance)
                        {
                        case '1':
                            print_importance1(&json, totalcnt);
                            break;
                        case '2':
                            print_importance2(&json, totalcnt);
                            break;
                        case '3':
                            print_importance3(&json, totalcnt);
                            break;
                        case '4':
                            print_importance4(&json, totalcnt);
                            break;
                        case '5':
                            print_importance5(&json, totalcnt);
                            break;
                        default:
                            printf("default" );
                            break;
                        }
                    printf("\n");
                    break;
                    
                case 'd':
                    printf("\n");
                    char implement = ' ';
                    printf("어떤 것을 프린트하기 원하나요?\n");
                    printf("a. 이행한 것\n");
                    printf("b. 이행하지 못한 것\n");
                    printf("c. 이행률\n");
                    scanf("%c",&implement);
                    getchar();
                    
                    if(implement == 'a')
                    {
                        printDoneYes(&json, totalcnt);
                    }
                    
                    else if(implement == 'b')
                    {
                        printDoneNo(&json, totalcnt);
                    }
                    else if(implement == 'c')
                    {
                        printf("\n");
                        print_yes_proportion(&json, totalcnt);
                    }
                    printf("\n");
                    break;

                default:
                    break;
            }
        
    }while(select !='q');
    printf("\nProgram End\n");
    printf("We always support your dream:)\n\n");
    
    freeJson(&json, totalcnt);
    return 0;
}

void categoryPrint(JSON *json,char category[10], int totalcnt){
    for(int i = 0; i<totalcnt; i++){
        if(strcmp(json->tokens[i].string,category)==0)
            printf("%s\n", json->tokens[i+1].string);
    }
}

void searchValue(JSON *json,char search[100], int totalcnt){
    for(int i = 0; i<totalcnt; i++){
        if(strcmp(json->tokens[i].string,search)==0)
            printf("%s\n", json->tokens[i-2].string);
    }
}

void printDoneYes(JSON *json, int totalcnt) {
    for(int i = 0; i<totalcnt; i++){
        if(strcmp(json->tokens[i].string,"yes")==0)
            printf("%s\n", json->tokens[i-6].string);
    }
}

void printDoneNo(JSON *json, int totalcnt) {
    for(int i = 0; i<totalcnt; i++){
        if(strcmp(json->tokens[i].string,"no")==0)
            printf("%s\n", json->tokens[i-6].string);
    }
}

void print_importance1(JSON *json, int totalcnt) {
    for(int i = 0; i<totalcnt; i++){
        if(strcmp(json->tokens[i].string,"★☆☆☆☆")==0)
            printf("%s\n", json->tokens[i-4].string);
    }
}

void print_importance2(JSON *json, int totalcnt) {
    for(int i = 0; i<totalcnt; i++){
        if(strcmp(json->tokens[i].string,"★★☆☆☆")==0)
            printf("%s\n", json->tokens[i-4].string);
    }
}

void print_importance3(JSON *json, int totalcnt) {
    for(int i = 0; i<totalcnt; i++){
        if(strcmp(json->tokens[i].string,"★★★☆☆")==0)
            printf("%s\n", json->tokens[i-4].string);
    }
}

void print_importance4(JSON *json, int totalcnt) {
    for(int i = 0; i<totalcnt; i++){
        if(strcmp(json->tokens[i].string,"★★★★☆")==0)
            printf("%s\n", json->tokens[i-4].string);
    }
}

void print_importance5(JSON *json, int totalcnt) {
    for(int i = 0; i<totalcnt; i++){
        if(strcmp(json->tokens[i].string,"★★★★★")==0)
            printf("%s\n", json->tokens[i-4].string);
    }
}

void print_yes_proportion(JSON *json, int totalcnt){
    float total_count=0;
    float yes_count=0;
    float proportion=0;
    char percent='%';
    for(int i = 0; i<totalcnt; i++){
        if(strcmp(json->tokens[i].string,"yes")==0
           || strcmp(json->tokens[i].string,"no")==0)
            total_count++;
        if(strcmp(json->tokens[i].string,"yes")==0)
            yes_count++;
    }
    proportion=(yes_count/total_count)*100;
    printf("total yes and no is %.0f\n",total_count);
    printf("yes number is %.0f\n",yes_count);
    printf("yes proportion is %.1f%c\n",proportion,percent);
}
