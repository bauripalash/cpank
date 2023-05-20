#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

#include "../cpank/include/api.h"


string run_pankti(const string src){
	char * c_str = new char[src.length() + 1];
	strcpy(c_str, src.c_str());
	string result = run_code(c_str);
	delete[] c_str;
	return result;
}

int main(int argc , char ** argv){
	string s = run_pankti("show 1+3+4");
	cout << s << "\n";
}
