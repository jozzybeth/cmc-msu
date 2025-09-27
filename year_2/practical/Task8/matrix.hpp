#include <iostream>
#include <sstream>
#include <cstring>
using namespace std;

enum class codes
{
	dyn_mem_err,
	index_out_bounds,
	invalid_string,
	incomp_sizes,
	unknown
};

class matrix_exception
{
	codes code;
public:
	matrix_exception(codes c = codes::unknown) {code = c;}
	friend ostream & operator<< (ostream & s, const matrix_exception & e);
};

//Invalid index exception subclass
class ind_exc: public matrix_exception
{
	int ind, valid;
public:
	ind_exc(int i = -1, int v = -1): matrix_exception(codes::index_out_bounds) {ind = i; valid = v;}
	friend ostream & operator<< (ostream & s, const ind_exc & e);
};

//Incompatible matrices exception subclass
class incomp_exc: public matrix_exception
{
	char operation; //'+' / '-' / '*' /'='
	char category; //'r' for rows, 'c' for columns, 'm' for multiplication
	int first, second;
public:
	incomp_exc(char oper = '\0', char cat = '\0', int fst = -1, int snd = -1):
		matrix_exception(codes::incomp_sizes) {operation = oper; category = cat; first = fst; second = snd;}
	friend ostream & operator<< (ostream & s, const incomp_exc & e);
};

class matrix
{
	int r, c; //rows and columns
	double** vals; //content; array of rows
	//Addressing is like in maths, not in programming
public:
	static double eps; //accuracy in comparing
	matrix(int n, int m); //zero matrix nxm +
	matrix(double val = 0); //1x1 matrix with val element; includes default +
	matrix(double* arr, int m); //1xm row-matrix +
	matrix(int n, double* arr); //nx1 column-matrix + 
	matrix(const char* str); //from string +
	static matrix identity(int n); //identity matrix nxn +
	static matrix diagonal(double* arr, int n); //diagonal matrix nxn +
	matrix(const matrix & m); //copy +
	matrix & operator= (const matrix & m); //matrix = matrix +
	~matrix(); //destructor +
	int rows() const; //8. rows number +
	int columns() const;  //columns number +
	void set(const int i, const int j, const double val); //set matrix[i][j]=val +
	//1) i'th row as a new matrix; 2) i'th column as a new matrix (if there is no i'th row)
	matrix operator[] (const int i) const; // +
	matrix operator*= (const double s); //matrix x scalar +
	matrix operator* (const double s) const; //matrix x scalar +
	friend ostream & operator<< (ostream & s, const matrix & m); //print matrix +
	matrix operator+= (const matrix & m); //+
	matrix operator+ (const matrix & m) const; //+
	matrix operator-= (const matrix & m); //+
	matrix operator- (const matrix & m) const; //+
	matrix operator*= (const matrix & m); //+
	matrix operator* (const matrix & m) const; //+
	matrix operator- (void); //+
	bool operator== (const matrix & m) const; //+
	bool operator!= (const matrix & m) const; //+
	matrix operator| (const matrix & m) const; //add to right +
	matrix operator/ (const matrix & m) const; //add below +
};
