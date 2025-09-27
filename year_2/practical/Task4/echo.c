#include <stdio.h>

int striden(char* str1, char* str2)
{
	while(*str1 == *str2 && *str1 != '\0' && *str2 != '\0')
 	{
  		str1++;
  		str2++;
 	}	
	if (*str1 == '\0' && *str2 == '\0')
		return 1;
	else 
		return 0;
}

void print_with_escape(char *str) 
{
    while (*str) 
        if (*str == '\\') 
            switch (*(str + 1)) 
            {
                case 'n':
                    putchar('\n');
                    str += 2;
                    break;
                case 't':
                    putchar('\t');
                    str += 2;
                    break;
                case 'b':
                    putchar('\b');
                    str += 2;
                    break;
                case 'a':
                    putchar('\a');
                    str += 2;
                    break;  
                case 'f':
                    putchar('\f');
                    str += 2;
                    break;  
                case 'r':
                    putchar('\r');
                    str += 2;
                    break; 
                case 'v':
                    putchar('\v');
                    str += 2;
                    break;  
                case '\'':
                    putchar('\'');
                    str += 2;
                    break;
                case '\"':
                    putchar('\"');
                    str += 2;
                    break;       
                case '\?':
                    putchar('\?');
                    str += 2;
                    break;  
                case '\\':
                    putchar('\\');
                    str += 2;
                    break;         
                default:
                    putchar(*str++);
                    break;
            }
        else 
            putchar(*str++);
}

int main(int argc, char** argv) 
{
	int flagE = 1;
	int flagN = 1;
	for (int i = 1; i < argc; i++) 
	{
		if (striden(argv[i],"-n")) 
		{
			flagN = 0;
			continue;
		}
		if (striden(argv[i],"-e")) 
		{
			flagE = 0;
			continue;
		}
		if (striden(argv[i],"-E")) 
		{
			flagE = 1;
			continue;
		}
		if (flagE) 
            printf("%s", argv[i]);
        else 
            print_with_escape(argv[i]);
        printf(" ");
	}
	if (flagN)
		printf("\n");
}