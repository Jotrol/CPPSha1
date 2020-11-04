#include <iostream>
#include "SHA1.hpp"

using namespace std;

int main()
{
	std::string s = "";
	for (int i = 0; i < 64 * 80 + 56; i++) s += 'a';

	SHA1::hexdigest(SHA1(s).get_hash());
	cout << SHA1(s).get_hash() << endl;

	return 0;
}