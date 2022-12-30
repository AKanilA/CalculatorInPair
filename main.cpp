#include "calculator.h"
#include <string>
using std::string;

int main() {

	string exp = "2 + 2 * 2";
	int a = Calculator::Calculate(exp);
	std::cout << a;

	return 0;
}