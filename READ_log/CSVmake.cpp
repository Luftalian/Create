#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    cout << "Hello, World!" << endl;
    string str, str1, str2;

    int n1, n2, n3, n4, n5, n6;
    ifstream time;
    // time.open("time.csv", ios::in);
    // ifstream acceleration;

    // acceleration.open("acceleration.csv", ios::in);
    // ifstream MPU_Aceleration;
    // MPU_Aceleration.open("MPU_Aceleration.csv", ios::in);
    // ifstream MPU_Angular_Velocity;
    // MPU_Angular_Velocity.open("MPU_Angular_Velocity.csv", ios::in);
    // ifstream MPU_Magnetic_Field;
    // MPU_Magnetic_Field.open("MPU_Magnetic_Field.csv", ios::in);
    // ifstream LPS_Pressure;
    // LPS_Pressure.open("LPS_Pressure.csv", ios::in);

    while (1)
    {
        cin >> str;
        if (str == "time")
        {
            cin >> n1;
            cin >> n2;
            cin >> n3;
            cin >> n4;
            cout << n1 << "," << n2 << "," << n3 << "," << n4 << endl;
        }
        if (str == "acceleration")
        {
            cin >> n1;
            cin >> n2;
            cin >> n3;
            cin >> n4;
            cin >> n5;
            cin >> n6;
            cout << n1 << "," << n2 << "," << n3 << "," << n4 << "," << n5 << "," << n6 << endl;
        }
        if (str == "MPU_Aceleration")
        {
            cin >> n1;
            cin >> n2;
            cin >> n3;
            cin >> n4;
            cin >> n5;
            cin >> n6;
            cout << n1 << "," << n2 << "," << n3 << "," << n4 << "," << n5 << "," << n6 << endl;
        }
        if (str == "MPU_Angular_Velocity")
        {
            cin >> n1;
            cin >> n2;
            cin >> n3;
            cin >> n4;
            cin >> n5;
            cin >> n6;
            cout << n1 << "," << n2 << "," << n3 << "," << n4 << "," << n5 << "," << n6 << endl;
        }
        if (str == "MPU_Magnetic_Field")
        {
            cin >> n1;
            cin >> n2;
            cin >> n3;
            cin >> n4;
            cin >> n5;
            cin >> n6;
            cout << n1 << "," << n2 << "," << n3 << "," << n4 << "," << n5 << "," << n6 << endl;
        }
        if (str == "LPS_Pressure")
        {
            cin >> n1;
            cin >> n2;
            cin >> n3;
            cout << n1 << "," << n2 << "," << n3 << endl;
        }
    }
}