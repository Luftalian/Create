#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    cout << "Hello, World!" << endl;
    string str, str1, str2;
    while (1)
    {
        cin >> str;
        if (str == "time")
        {
            cin >> str1;
            cin >> str2;
        }
        if (str == "acceleration")
        {
            cin >> str1;
            cin >> str2;
        }
        if (str == "MPU_Aceleration")
        {
            cin >> str1;
            cin >> str2;
        }
        if (str == "MPU_Angular_Velocity")
        {
            cin >> str1;
            cin >> str2;
        }
        if (str == "MPU_Magnetic_Field")
        {
            cin >> str1;
            cin >> str2;
        }
        if (str == "LPS_Pressure")
        {
            cin >> str1;
            cin >> str2;
        }
    }
}