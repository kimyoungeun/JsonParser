#include <stdio.h>     
#include <stdlib.h>    
#include <stdbool.h>  
#include <string.h>  

typedef enum _TOKEN_TYPE {
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_BOOLEAN
} TOKEN_TYPE;

typedef struct _TOKEN {
    TOKEN_TYPE type;  
    union {    
        char *string;  
        double number; 
    };
    bool isArray;
    bool val_bool;
} TOKEN;

#define TOKEN_COUNT 30 //토큰의 최대 크기

typedef struct _JSON {
    TOKEN tokens[TOKEN_COUNT];
} JSON;

char *readFile(char *filename, int *readSize)    
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        return NULL;

    int size;
    char *buffer;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = malloc(size + 1);
    memset(buffer, 0, size + 1);

    if (fread(buffer, size, 1, fp) < 1)
    {
        *readSize = 0;
        free(buffer);
        fclose(fp);
        return NULL;
    }

    *readSize = size;
    fclose(fp);    
    return buffer;
}

void parseJSON(char *doc, int size, JSON *json)    // JSON 파싱 함수
{
    int tokenIndex = 0;    
    int pos = 0;           
    if (doc[pos] != '{')   //object case
        return;
    pos++;  

    while (pos < size)      
    {
        switch (doc[pos])   
        {
        case '"':        //string case 
        {
            char *begin = doc + pos + 1;
            char *end = strchr(begin, '"');
            if (end == NULL)    
                break;          
            int stringLength = end - begin;  

            json->tokens[tokenIndex].type = TOKEN_STRING;
            json->tokens[tokenIndex].string = malloc(stringLength + 1);
            memset(json->tokens[tokenIndex].string, 0, stringLength + 1);

            memcpy(json->tokens[tokenIndex].string, begin, stringLength);
            tokenIndex++;
            pos = pos + stringLength + 1;
        }
        break;

        case '[':     //array case
        {
            pos++;    

            while (doc[pos] != ']')   
            {
                if (doc[pos] == '"')  
                {
                    char *begin = doc + pos + 1;
                    char *end = strchr(begin, '"');
                    if (end == NULL)  
                        break;      
                    int stringLength = end - begin;  

                    json->tokens[tokenIndex].type = TOKEN_STRING;
                    json->tokens[tokenIndex].string = malloc(stringLength + 1);
                    json->tokens[tokenIndex].isArray = true;
                    memset(json->tokens[tokenIndex].string, 0, stringLength + 1);
                    memcpy(json->tokens[tokenIndex].string, begin, stringLength);
                    tokenIndex++;

                    pos = pos + stringLength + 1; 
                }
                pos++;    // 다음 문자로
            }
        }
        break;

        case '0': case '1': case '2': case '3': case '4': case '5': //number case 
        case '6': case '7': case '8': case '9': case '-':         
        {
            char *begin = doc + pos;
            char *end;
            char *buffer;
            end = strchr(doc + pos, ',');
            if (end == NULL)
            {
                end = strchr(doc + pos, '}');
                if (end == NULL)   
                    break;       
            }
            int stringLength = end - begin;   

            buffer = malloc(stringLength + 1);
            memset(buffer, 0, stringLength + 1);
            memcpy(buffer, begin, stringLength);
            json->tokens[tokenIndex].type = TOKEN_NUMBER;  
            json->tokens[tokenIndex].number = atof(buffer);
            
	    free(buffer); 
            tokenIndex++;  
            pos = pos + stringLength + 1; 
        }
        break;

        case 't' : //true case
        {
            char *begin = doc + pos;
            char *end;
            char *buffer;

            end = strchr(doc + pos, ',');
            if (end == NULL)
            {
                end = strchr(doc + pos, '}');
                if (end == NULL)   
                    break;     
            }
            int stringLength = end - begin; 

            buffer = malloc(stringLength + 1);
            memset(buffer, 0, stringLength + 1);
            memcpy(buffer, begin, stringLength);

            if(strncmp(buffer,"true",0)==0){
            json->tokens[tokenIndex].type = TOKEN_BOOLEAN;  
            json->tokens[tokenIndex].val_bool = true;
            json->tokens[tokenIndex].string = malloc(stringLength + 1);
            memset(json->tokens[tokenIndex].string, 0, stringLength + 1);
            memcpy(json->tokens[tokenIndex].string, begin, stringLength);
            }

            free(buffer);  
            tokenIndex++; 
            pos = pos + stringLength + 1; 
        }
        break;

        case 'f' : //false case
        {
            char *begin = doc + pos;
            char *end;
            char *buffer;

            end = strchr(doc + pos, ',');
            if (end == NULL)
            {
                end = strchr(doc + pos, '}');
                if (end == NULL)  
                    break;   
            }
            int stringLength = end - begin;

            buffer = malloc(stringLength + 1);
            memset(buffer, 0, stringLength + 1);
            memcpy(buffer, begin, stringLength);

            if(strncmp(buffer,"false",0)==0){
            json->tokens[tokenIndex].type = TOKEN_BOOLEAN;  
            json->tokens[tokenIndex].val_bool = false;       
            json->tokens[tokenIndex].string = malloc(stringLength + 1);
            memset(json->tokens[tokenIndex].string, 0, stringLength + 1);
            memcpy(json->tokens[tokenIndex].string, begin, stringLength);
            }
            free(buffer); 
            tokenIndex++;
            pos = pos + stringLength + 1;  
        }
        break;
        }
        pos++; // 다음 문자로
    }
}

void freeJSON(JSON *json)    // JSON 해제 함수
{
    for (int i = 0; i < TOKEN_COUNT; i++)          
    {
        if (json->tokens[i].type == TOKEN_STRING)  
            free(json->tokens[i].string);       
    }
}

int main(int argc, char** argv)
{
    int size; 
    char *doc = readFile(argv[1], &size);
    if (doc == NULL)
        return -1;

    JSON json = { 0, };  
    parseJSON(doc, size, &json);    

    printf("%s\n", json.tokens[0].string);            
    printf("%s\n", json.tokens[1].string);        
    printf("%s\n", json.tokens[2].string);            
    printf("%s\n", json.tokens[3].string);
    printf("%s\n", json.tokens[4].string);           
    printf("%s\n", json.tokens[5].string);  
    printf("%s\n", json.tokens[6].string);           
    printf("%s\n", json.tokens[7].string);
    printf("%s\n", json.tokens[8].string);           
    printf("%s\n", json.tokens[9].string);
    printf("%s\n", json.tokens[10].string); 
    printf("%s\n", json.tokens[11].string);           
    printf("%s\n", json.tokens[12].string);
    printf("%s\n", json.tokens[13].string);           
    printf("%s\n", json.tokens[14].string);            
    printf("%s\n", json.tokens[15].string);            
    printf("%s\n", json.tokens[16].string);   
    printf("%s\n", json.tokens[17].string);    
    printf("%f\n", json.tokens[18].number);
   
    freeJSON(&json);   
    free(doc);
    return 0;
}
