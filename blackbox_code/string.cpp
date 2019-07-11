//vscode에서 to_string하기
#include <iostream>
#include <string>
using namespace std;

int main(void)
{
    int num1=10;
    string str1 = to_string(num1);
    cout<<"to_string(num1)" << str1 <<endl;
    
    return 0;
}