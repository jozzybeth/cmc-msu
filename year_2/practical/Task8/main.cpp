#include "matrix.hpp"
#include <iostream>

#define catch_and_print \
catch (ind_exc e) {cerr<<e<<endl;} \
catch (incomp_exc e) {cerr<<e<<endl;} \
catch (matrix_exception e) {cerr<<e<<endl;} \

//Showing valid constructors using
void try_valid_constructors(void)
{
	double arr[5] = {1.11, 2.22, 3.33, 4.44, 25};
	try
	{
		cout<<"  Valid ones:"<<endl;
		matrix m1(4, 5);
		cout<<"4x5 zero matrix:"<<endl<<m1;
		matrix m2;
		cout<<"Default constructor matrix:"<<endl<<m2;
		matrix m3(3.1428);
		cout<<"1x1 matrix:"<<endl<<m3;
		matrix m4("{{1.2, 2.3, 4}, {1, 1, 1}, {2, 2, 2}}");
		cout<<"M4:"<<endl<<m4;
		matrix m5(arr, 4);
		cout<<"Row-matrix:"<<endl<<m5;
		matrix m6(4, arr);
		cout<<"Column-matrix:"<<endl<<m6;
		matrix mi = matrix::identity(4);
		cout<<"Identity matrix:"<<endl<<mi;
		matrix md = matrix::diagonal(arr, 5);
		cout<<"Diagonal matrix:"<<endl<<md;
		matrix m_assign = m5;
		
		cout<<"Assigning row-matrix to another matrix variable:"<<endl<<m_assign;
		matrix m_cpy (m5);
		cout<<"...and copying it"<<endl<<m_cpy;
		cout<<"Rows in 4x5 matrix from beginning: "<<m1.rows()<<endl;
		cout<<"Columns in 4x5 this matrix: "<<m1.columns()<<endl;
		m1.set(1, 4, 5.5);
		cout<<"Setting its [2][5] elem to 5.5:"<<endl<<m1;
		cout<<"Getting its 2nd row as a new matrix:"<<endl<<m1[1];
		cout<<"Getting its 5th column as a new matrix:"<<endl<<m1[4];
	}
	catch_and_print
}

//Showing invalid constructors using
void try_invalid_constructors(void)
{
	double arr[5] = {1.11, 2.22, 3.33, 4.44, 25};
	cout<<"  Invalid ones:"<<endl;
	cout<<"matrix(0, 1)"<<endl;
	try
	{
		matrix m(0, 1);
	}
	catch_and_print
	cout<<"matrix(5, -2)"<<endl;
	try
	{
		matrix m(5, -2);
	}
	catch_and_print
	cout<<"matrix (arr, 0)"<<endl;
	try
	{
		matrix m(arr, 0);
	}
	catch_and_print
	cout<<"matrix (-1, arr)"<<endl;
	try
	{
		matrix m(-1, arr);
	}
	catch_and_print
	cout<<"matrix(\"{1a}, {2, 3}\")"<<endl;
	try
	{
		matrix m("{1a}, {2, 3}");
	}
	catch_and_print
	cout<<"indentity (0)"<<endl;
	try
	{ 
		matrix m = matrix::identity(0);
	}
	catch_and_print
	cout<<"diagonal (arr, -2)"<<endl;
	try
	{
		matrix m = matrix::diagonal(arr, -2);
	}
	catch_and_print
	cout<<"matrix m(3, 3); m.set(-1, 2, ...)"<<endl;
	try
	{
		matrix m(3, 3);
		m.set(-1, 2, 0);
	}
	catch_and_print
	cout<<"matrix m(3, 3); m.set(2, -1, ...)"<<endl;
	try
	{
		matrix m(3, 3);
		m.set(2, -1, 0);
	}
	catch_and_print
	cout<<"matrix m(3, 3); m.set(5, 2, ...)"<<endl;
	try
	{
		matrix m(3, 3);
		m.set(5, 2, 0);
	}
	catch_and_print
	cout<<"matrix m(3, 3); m.set(2, 5, ...)"<<endl;
	try
	{
		matrix m(3, 3);
		m.set(2, 5, 0);
	}
	catch_and_print
}

//Showing + += - -= operations
void try_plus_minus(matrix & m1, matrix & m2)
{
	matrix m3 = m1;
	cout<<"=================================================="<<endl;
	cout<<"   ~~~~~~~~~~ + ~~~~~~~~~~"<<endl;
	try
	{
		cout<<m1<<"   +   "<<endl<<m2<<"   =   "<<endl<<m1 + m2;
	}
	catch_and_print
	cout<<"   ~~~~~~~~~~ += ~~~~~~~~~~"<<endl;
	try
	{
		cout<<m1<<"   +=   "<<endl<<m2<<"   =>   "<<endl<<(m3 += m2);
	}
	catch_and_print
	m3 = m1;
	cout<<"   ~~~~~~~~~~ - ~~~~~~~~~~"<<endl;
	try
	{
		cout<<m1<<"   -   "<<endl<<m2<<"   =   "<<endl<<m1 - m2;
	}
	catch_and_print
	cout<<"   ~~~~~~~~~~ -= ~~~~~~~~~~"<<endl;
	try
	{
		cout<<m1<<"   -=   "<<endl<<m2<<"   =>   "<<endl<<(m3 -= m2);
	}
	catch_and_print
	cout<<"   ~~~~~~~~~~ unary - ~~~~~~~~~~"<<endl;
	cout<<"   -"<<endl<<m1<<"   =   "<<endl<<-m1;
}

//Showing * *= operations
void try_multiply(matrix & m1, matrix & m2)
{
	matrix m3 = m1;
	cout<<"=================================================="<<endl;
	cout<<"   ~~~~~~~~~~ * scalar ~~~~~~~~~~"<<endl;
	try
	{
		cout<<m1<<"   *  "<<4.56<<"   =>   "<<endl<<(m1 * (double) 4.56);
	}
	catch_and_print
	cout<<"   ~~~~~~~~~~ *= scalar ~~~~~~~~~~"<<endl;
	try
	{
		cout<<m1<<"   *=   "<<4.56<<"   =>   "<<endl<<(m3 *= 4.56);
	}
	catch_and_print
	m3 = m1;
	cout<<"   ~~~~~~~~~~ * ~~~~~~~~~~"<<endl;
	try
	{
		cout<<m1<<"   *   "<<endl<<m2<<"   =   "<<endl<<m1 * m2;
	}
	catch_and_print
	cout<<"   ~~~~~~~~~~ *= ~~~~~~~~~~"<<endl;
	try
	{
		cout<<m1<<"   *=   "<<endl<<m2<<"   =>   "<<endl<<(m3 *= m2);
	}
	catch_and_print
}

//Showing logic operations
void try_logic(matrix & m1, matrix & m2)
{
	cout<<"=================================================="<<endl;
	cout<<"   ~~~~~~~~~~ == ~~~~~~~~~~"<<endl;
	cout<<"   <eps = "<<matrix::eps<<">"<<endl;
	cout<<m1<<"   ==   "<<endl<<m2<<"   =   "<<endl<<(m1 == m2 ? "true" : "false")<<endl;
	cout<<m1<<"   ==   "<<endl<<m1<<"   =   "<<endl<<(m1 == m1 ? "true" : "false")<<endl;
	cout<<"   ~~~~~~~~~~ != ~~~~~~~~~~"<<endl;
	cout<<"   <eps = "<<matrix::eps<<">"<<endl;
	cout<<m1<<"   !=   "<<endl<<m2<<"   =   "<<endl<<(m1 != m2 ? "true" : "false")<<endl;
	cout<<m1<<"   !=   "<<endl<<m1<<"   =   "<<endl<<(m1 != m1 ? "true" : "false")<<endl;
}

//Showing concatenation operations
void try_cat(matrix & m1, matrix & m2)
{
	cout<<"=================================================="<<endl;
	cout<<"   ~~~~~~~~~~ | ~~~~~~~~~~"<<endl;
	cout<<m1<<"   |   "<<endl<<m2<<"   =   "<<endl<<(m1 | m2);
	cout<<"   ~~~~~~~~~~ / ~~~~~~~~~~"<<endl;
	cout<<m1<<"   /   "<<endl<<m2<<"   =   "<<endl<<(m1 / m2);
}

double matrix::eps = 0.001;
int main(int argc, char* argv[])
{
	if (argc < 2 || argv[1][0] == 'c')
	{
		cout<<"   ~~~Contructors and some functions~~~"<<endl;
		try_valid_constructors();
		try_invalid_constructors();
	}
	else if (argc >= 3)
		try
		{
			matrix m1(argv[1]), m2(argv[2]);
			if (argc == 3 || (argc > 3 && (argv[3][0] == '+' || argv[3][0] == '-')))
			{
				cout<<"   ~~~~~~~~~~~Plus and minus~~~~~~~~~~~"<<endl;
				try_plus_minus(m1, m2);
			}
			if (argc == 3 || (argc > 3 && argv[3][0] == '*'))
			{
				cout<<"   ~~~~~~~~~~~Multiplication~~~~~~~~~~~"<<endl;
				try_multiply(m1, m2);
			}
			if (argc == 3 || (argc > 3 && argv[3][0] == '='))
			{
				cout<<"  ~~~~~~~~~Equality/inequality~~~~~~~~~"<<endl;
				try_logic(m1, m2);
			}
			if (argc == 3 || (argc > 3 && (argv[3][0] == '|' || argv[3][0] == '/'))) 
			{
				cout<<"  ~~~~~~~~~~~~~~~Merging~~~~~~~~~~~~~~~"<<endl;
				try_cat(m1, m2);
			}
		}
		catch_and_print
}
