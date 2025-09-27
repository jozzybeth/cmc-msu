#include <iomanip>


#include "matrix.hpp"
// Imlementation of class Marix

//1. zero matrix nxm
matrix::matrix(int n, int m)
{
	if (n <= 0)
		throw ind_exc(n, 0);
	if (m <= 0)
		throw ind_exc(m, 0);
	r = n;
	c = m;
	vals = 0;
	try
	{
		vals = new double*[r];
		for (int i = 0; i < r; ++i)
		{
			vals[i] = 0;
			vals[i] = new double[c];
			for (int j = 0; j < c; ++j)
				vals[i][j] = 0;
		}
	}
	catch (...)
	{
		cerr<<"Got an exception in nxm matrix constructor!"<<endl; //ошибка создания динамической памяти
		throw matrix_exception(codes::dyn_mem_err);
	}
}

//copy
matrix::matrix(const matrix &m): matrix::matrix(m.r,m.c)
{

    for (int i = 0; i < r; ++i)
        for (int j = 0; j < c; ++j)
			vals[i][j] = m.vals[i][j];
}

//matrix = matrix
matrix & matrix::operator= (const matrix & m)
{
	for (int i = 0; i < r; ++i)
		delete[] vals[i];
	delete[] vals;
	vals = 0;
	r = m.r;
	c = m.c;
	try
	{
		vals = new double*[r];
		for (int i = 0; i < r; ++i)
		{
			vals[i] = 0;
			vals[i] = new double[c];
			for (int j = 0; j < c; ++j)
				vals[i][j] = 0;
		}
	}
	catch (...)
	{
		cerr<<"Got an exception in nxm matrix constructor!"<<endl; //ошибка создания динамической памяти
		throw matrix_exception(codes::dyn_mem_err);
	}
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
			vals[i][j] = m.vals[i][j];
	return *this;
}

//destructor
matrix::~matrix(void)
{
	for (int i = 0; i < r; ++i)
		delete[] vals[i];
	delete[] vals;
}

//2. matrix 1x1 from val
matrix::matrix(double val)
{
	vals = 0;
	r = 1;
	c = 1;
	try
	{
		vals = new double*[r];
		vals[0] = 0;
		vals[0] = new double[c];
	}
	catch (...)
	{
		cerr<<"Got an exception in nxm matrix constructor!"<<endl; //ошибка создания динамической памяти
		throw matrix_exception(codes::dyn_mem_err);
	}
	vals[0][0] = val;
}

//3. matrix 1xm from arr[m]
matrix::matrix(double* arr, int m): matrix::matrix(1,m)
{
	for (int i = 0; i < m; ++i)
		vals[0][i] = arr[i];
	
}

//4. matrix nx1 from arr[n]
matrix::matrix(int n, double* arr): matrix::matrix(n,1)
{
	for (int i = 0; i < r; ++i)
		vals[i][0] = arr[i];
}

//5. matrix from str
matrix::matrix(const char* str){ //{{1.2, 2.3, 4}, {1, 1, 1}, {2, 2, 2}}
	int open = 1, close = 0, size = 1, mainsize, cur = 1; //size - подсчет запятых, количество столбцов
	while (str[cur]) 
	{
		if (str[cur] == '{') 
			open++;
		else if (str[cur] == '}') 
		{
			close++;
			if (open == close)
			{
				cur++;
				break;
			}
			if (close == 1) 
				mainsize = size;
			if (mainsize != size) 
			{
				throw matrix_exception(codes::invalid_string);
			}
			size = 0;
		} 
		else if (str[cur] == ',') 
			size += 1;    
		cur++;
	}
	if (open != close || str[cur])
	{
		throw matrix_exception(codes::invalid_string);
		
	}
	cur = 0;
	r = open - 1;
	c = mainsize;
	vals = new double* [r];
	for (int i = 0; i < r; ++i) 
		vals[i] = new double [c];
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
		{
			double tmp = 0, tmp2 = 1;
			bool flag = true;
			while (!isdigit(str[cur]))
				cur++;
			while (str[cur] != ',' && str[cur] != '}' && str[cur]) 
			{
				if (str[cur] == '.') 
					flag = false; 
				else if (!isdigit(str[cur]))
					throw matrix_exception(codes::invalid_string);	
				else if (flag) //целая часть
				{
					tmp *= 10;
					tmp += str[cur] - '0';
				}
				else
				{
					tmp2 /= 10;
					tmp += (str[cur] - '0') * tmp2;
				}
				++cur;
			}
			vals[i][j] = tmp;
		}
}

//6. I matrix
matrix matrix::identity(int n) 
{
	matrix temp(n, n);
	for (int i = 0; i < n; ++i)
			temp.vals[i][i] = 1.0;
	return temp;
}

//7. diag matrix from arr[n]
matrix matrix::diagonal(double* vals, int n)
{
	matrix temp(n, n);
	for (int i = 0; i < n; ++i)
			temp.vals[i][i] = vals[i];
	return temp;
}

//8. rows number
int matrix::rows() const
{
	return r;
}

//9. columns number
int matrix::columns() const
{
	return c;
}

//10. set matrix[i][j]=val
void matrix::set(const int i, const int j, const double val)
{
	if (i < 0)
		throw ind_exc(i, 0);
	if (i >= r)
		throw ind_exc(i, r);
	if (j < 0)
		throw ind_exc(j, 0);
	if (j >= c)
		throw ind_exc(j, c);
	vals[i][j] = val;
	return;
}

//11. i'th row as a new matrix; 12. i'th column as a new matrix (if there is no i'th row)
matrix matrix::operator[] (const int i) const
{
	if (i >= 0 && i < r)
	{
		matrix temp(1,c);
		for (int j = 0; j < c; ++j)
			temp.vals[0][j] = vals[i][j];
		return temp;
	}
	else if (i >= 0 && i < c)
	{
		matrix temp(r,1);
		for (int j = 0; j < r; ++j)
			temp.vals[j][0] = vals[j][i];
		return temp;
	}
	else if (i < 0)
		throw ind_exc(i, 0);
	else
		throw ind_exc(i, c);

}

//13. matrix x scalar
matrix matrix::operator*= (const double s)
{
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
			vals[i][j] *= s;
	return *this;
}

//13. matrix x scalar
matrix matrix::operator* (const double s) const
{
	matrix res(r, c);
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
		{
			res.vals[i][j] = vals[i][j] * s;
		}
	return res;
}

//14. print matrix
ostream & operator<< (ostream & s, const matrix & m)
{
	s<<setprecision(6);
	for (int i = 0; i < m.r; ++i)
	{
		s<<"|";
		for (int j = 0; j < m.c; ++j)
			s<<' '<<setw(6)<<m.vals[i][j];
		s<<" |"<<endl;
	}
	return s;
}

//Arithmetic

//15. operation +
matrix matrix::operator+ (const matrix & m) const //не изменяет членов-данных объекта
{
	if (r != m.r)
		throw incomp_exc('+', 'r', r, m.r);
	if (c != m.c)
		throw incomp_exc('+', 'c', c, m.c);
	matrix res(r, c);
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
			res.vals[i][j] = vals[i][j] + m.vals[i][j];
	return res;
}

//16. operation +=
matrix matrix::operator+= (const matrix & m)
{
	if (r != m.r)
		throw incomp_exc('+', 'r', r, m.r);
	if (c != m.c)
		throw incomp_exc('+', 'c', c, m.c);
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
			vals[i][j] += m.vals[i][j];
	return *this;
}

//17. operation -
matrix matrix::operator- (const matrix & m) const //не изменяет членов-данных объекта
{
	if (r != m.r)
		throw incomp_exc('-', 'r', r, m.r);
	if (c != m.c)
		throw incomp_exc('-', 'c', c, m.c);
	matrix res(r, c);
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
			res.vals[i][j] = vals[i][j] - m.vals[i][j];
	return res;
}

// 18. operation -=
matrix matrix::operator-= (const matrix & m)
{
	if (r != m.r)
		throw incomp_exc('-', 'r', r, m.r);
	if (c != m.c)
		throw incomp_exc('-', 'c', c, m.c);
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
			vals[i][j] -= m.vals[i][j];
	return *this;
}

//19. operation *
matrix matrix::operator* (const matrix & m) const
{
	if (c != m.r)
	{
		throw incomp_exc('*', 'm', c, m.r);
	}
	matrix temp(r, m.c);
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < m.c; ++j)
			for (int k = 0; k < c; ++k)
				temp.vals[i][j] += vals[i][k] * m.vals[k][j];
	return temp;
}

//20. operation *=
matrix matrix::operator*= (const matrix & m)
{
	if (c != m.r)
	{
		throw incomp_exc('*', 'm', c, m.r);
	}
	matrix temp(r, m.c);
		for (int i = 0; i < r; ++i)
			for (int j = 0; j < m.c; ++j)
				for (int k = 0; k < c; ++k)
					temp.vals[i][j] += vals[i][k] * m.vals[k][j];
	for (int i = 0; i < r; ++i) //необходимо изменить размер матрицы
		delete[] vals[i];
	delete[] vals;
	c = m.c;
	try
	{
		vals = new double*[r];
		for (int i = 0; i < r; ++i)
		{
			vals[i] = 0;
			vals[i] = new double[c];
			for (int j = 0; j < c; ++j)
				vals[i][j] = temp.vals[i][j];
		}
	}
	catch (...)
	{
		throw matrix_exception(codes::dyn_mem_err);
	}
	return *this;
}

//21. operation unary -
matrix matrix::operator- ()
{
	matrix res = *this;
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
			res.vals[i][j] = -res.vals[i][j];
	return res;
}

//22. operation ==
bool matrix::operator== (const matrix & m) const
{
	if (r != m.r)
		throw incomp_exc('=', 'r', r, m.r);
	if (c != m.c)
		throw incomp_exc('=', 'c', c, m.c);
	bool flag = true;
	for (int i = 0; i < r; ++i) 
	{
		for (int j = 0; j < c; ++j)
			if (abs(vals[i][j] - m.vals[i][j]) >= eps) 
			{
				flag = false;
				break;
			}
		if (!flag)
			break;
	}
	return flag;
}

//23. operation ==
bool matrix::operator!= (const matrix & m) const
{
	return !(*this == m);
}

//24. add to right
matrix matrix::operator| (const matrix & m) const
{
	matrix res(r > m.r ? r : m.r, c + m.c);
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
			res.vals[i][j] = vals[i][j];
	for (int i = 0; i < m.r; ++i)
		for (int j = c; j < res.c; ++j)
			res.vals[i][j] = m.vals[i][j-c];
	return res;
}

//25. add below
matrix matrix::operator/ (const matrix & m) const
{
	matrix res(r + m.r, c > m.c ? c : m.c);
	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j)
			res.vals[i][j] = vals[i][j];
	for (int i = r; i < res.r; ++i)
		for (int j = 0; j < m.c; ++j)
			res.vals[i][j] = m.vals[i-r][j];
	return res;
}


ostream & operator<< (ostream & s, const matrix_exception & e)
{
	switch (e.code)
	{
		case codes::dyn_mem_err:
			s<<"<exc> A dynamic memory error occured!";
			break;
		case codes::invalid_string:
			s<<"<exc> Invalid string used in constructor!";
			break;
		case codes::unknown:
			s<<"<exc> An unknown error occured in a matrix method!";
			break;
		default:
			s<<"<exc> An exception throwed but caught inproperly!";
			break;
	}
	return s;
}

ostream & operator<< (ostream & s, const ind_exc & e)
{
	s<<"<exc> Invalid index used!";
	if (e.ind >= 0)
	{
		s<<" (" << e.ind <<" used, while ";;
		if (e.valid < e.ind)
			s<<"maximum possible is "<<e.valid;
		else	
			s<<"minimum possible is "<<e.valid+1;
		s<<')';
	}
	else
		s << " You used negative index";
	return s;
}

ostream & operator<< (ostream & s, const incomp_exc & e)
{
	s<<"<exc> Arithmetical operation ";
	if (e.operation != '\0')
		s<<e.operation<<' ';
	s<<"operands are incompatible!";
	switch(e.category)
	{
		case 'r': case 'c':
			s<<endl<<"      (Different ";
			if (e.category == 'r')
				s<<"rows";
			else
				s<<"columns";
			s<<" number";
			if (e.first >= 0)
				s<<": the first operand's is "<<e.first;
			if (e.second >= 0)
				s<<", while the second operand's is "<<e.second;
			s<<')';
			break;
		case 'm':
			if (e.first >= 0)
				s<<endl<<"      (First operand has " <<e.first<<" columns";
			if (e.second >= 0)
				s<<" while the second one has "<<e.second<<" rows";
			s<<')';
			break;
	}
	return s;
}
