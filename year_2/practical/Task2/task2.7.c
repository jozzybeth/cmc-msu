/* Вычисление значения выражения, содержащего цифры ’0’-’9’, знаки
* операций ’+’, ’*’ и скобки ’(’, ’)’. ( Предполагается, что коды цифр
* упорядочены по возрастанию цифр и справделиво равенство ’9’-’0’==9 как,
* например, в ASCII кодировке
*/
#include<stdio.h>
#include<setjmp.h>
jmp_buf begin; /* точка начала диалога с пользователем */
char curlex; /* текущая лексема */
void getlex(void); /* выделяет из входного потока очередную лексему */
int expr(void); /* распознает выражение и вычисляет его значение */
int add(void); /* распознает слагаемое и вычисляет его значение */
int sub(void); /* распознает слагаемое и вычисляет его значение */
int div(void); /* распознает множитель и вычисляет его значение */
int mult(void); /* распознает множитель и вычисляет его значение */
int pows(void); /* распознает множитель и вычисляет его значение */
void error(); /* сообщает об ошибке в выражении и передает управление
в начало функции main (точка begin) */

int main()
{
	int result;
	setjmp(begin);
	printf("==>");
	getlex();
	result = expr();
	if (curlex != '\n') error();
	printf("\n%d\n", result);
	return 0;
}

void getlex()
{
	while ((curlex = getchar()) == ' ');
}

void error(void)
{
	printf("\nОШИБКА!\n");
	while(getchar() != '\n');
	longjmp(begin, 1);
}

int expr()
{
	int e = add();
	while (curlex == '+')
	{ 
		getlex(); 
		e += add();
	}
	return e;
} 

int add()
{
	int a = sub();
	while (curlex == '-')
	{ 
		getlex(); 
		a -= sub();
	}
	return a;
}

int sub()
{
	int s = div();
	while (curlex == '/')
	{ 
		getlex(); 
		s /= div();
	}
	return s;
}

int div()
{
	int d = mult();
	while (curlex == '*')
	{ 
		getlex(); 
		d *= mult();
	}
	return d;
}

int mult()
{
	int m = pows();
	int l = 1;
	while (curlex == '^')
	{ 
		getlex(); 
		int k = mult();
		for (int i = 0; i < k; i++)
			l *= m;
		return l;
	}
	return m;	
}

int pows()
{
	int m;
	switch(curlex){
	case '0': case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': case '8': case '9': 
		m = curlex - '0'; 
		break;
	case '(': 
		getlex(); 
		m = expr();
		if (curlex == ')') 
			break;
	/* иначе ошибка - нет закрывающей скобки */
	default : error();
	}
	getlex();
	return m;
}
/* Сравните прототипы (в начале программы) и заголовки функций
* (в соответствующих определениях) error() и getlex() : информацию
* о параметрах (в данном случае пустые списки параметров - void) можно
* опускать в заголовке, если она есть в объявлении прототипа (как в случае
* с getlex) или не указывать в прототипе, а указать только в заголовке
* (как в случае с error). Тип обех функций - "функция, не возвращающая
* значения с пустым списком аргументов" */
