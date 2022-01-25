#include <iostream>
#include <cstring>
#include <fstream>
#include <cassert>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <cmath>
#include<math.h>

using namespace std;

unsigned long long Square_multiplies(int a, int k, int n)
{
    //a quick way to calculate the a^b mod n.
    //turn into binary;
    bool Binary[32] = {0};
    int i = 0;
    int flag = 0;
    while (k > 0)
    {
        Binary[i] = k - k / 2 - k / 2;
        k /= 2;
        if (Binary[i])
            flag = i;
        i++;
    }
    //Square-and-Multiply Algorithm
    unsigned long long z = 1;
    for (int j = flag; j >= 0; j--)
    {
        z = (z * z) % n;
        if (Binary[j])
            z = (z * a) % n;
    }
    return z;
}

bool isPrime(int decimal, int t)
{
    //Miller Robin test:
    decimal--;

    bool isEven = true;
    int r = 0;
    unsigned u = 0;

    int temp = decimal;

    while (isEven)
    {
        temp /= 2;
        r++;
        if (temp % 2 == 1)
        {
            isEven = false;
            u = temp;
        }
    };

    int a = 0;
    unsigned long long remainder = 0;
    bool composite = true;
    bool prime = true;
    for (int j = 0; j < t; j++)
    {
        composite = true;
        a = (rand() % (decimal)) + 1; //to ensure a start from 1.
        for (int k = 0; k < r; k++)
        {
            remainder = Square_multiplies(a, (int)u * pow(2, k), (decimal + 1));
            int result = u * pow(2, k);
            if (k == 0)
            {
                if (remainder == 1 || remainder == decimal)
                    composite = false;
            }
            else
            {
                if (remainder == decimal)
                    composite = false;
            }
        }
        if (composite)
        {
            prime = false;
            break;
        }
        else
        {
            continue;
        }
    }
    return prime;
}

int SecretDistribution(int n, int t, int s, char *filename, int upperBound)
{
    //除a0=s外，其余系数为1
    ofstream outfile;
    outfile.open(filename);
    //打印多项式
    outfile << "polynomial: f(x)=";
    outfile << s; //秘密，即a0
    for (int i = 1; i < t; ++i)
    {
        outfile << "+x";
        if (i >= 2)
        {
            outfile << '^' << i;
        }
    }
    outfile << endl;
    outfile << "n shares:" << endl;
    //打印n个（x,y）点
    for (int j = 1; j <= n; ++j)
    {
        double y = s;
        for (int k = 1; k < t; ++k)
        {
            y += pow (j, k);
            if (y >= upperBound)// To secure it maintains GF(q）
            {
                return -1;
            } //防止int越界
        }
        outfile << '(' << j << ',' << int(y) << ')' << endl;
    }
    outfile << "t=" << t << endl;
    outfile.close();
    return 0;
}

int SecretRecovery(char *filename)
{
    int t = 0;
    int x[100] = {0};
    int y[100] = {0};
    int xasix = 0;
    int yasix = 0;
    ifstream in(filename);
    ifstream infile;
    infile.open(filename);    //将文件流对象与文件连接起来
    assert(infile.is_open()); //若失败,则输出错误消息,并终止程序运行
    char c[1000] = {0};       //读入的字符
    int i = 0;
    while (!infile.eof())
    {
        infile >> c[i++];
    }
    c[i] = '\0';
    int k;
    int j;
    for (j = 0; c[j] != '\0'; ++j)
    {
        if ((c[j] == '(') && (c[j + 1] != 'x')) //读取x
        {
            for (k = j + 1; c[k] != ','; ++k)
            {
                x[xasix] = x[xasix] * 10 + (c[k] - '0');
            }
            for (k = k + 1; c[k] != ')'; ++k) //读取y
            {
                y[yasix] = y[yasix] * 10 + (c[k] - '0');
            }
            xasix += 1;
            yasix += 1;
        }
    }
    t = c[j - 1] - '0';
    x[xasix + 1] = '\0';
    y[yasix + 1] = '\0';
    in.close();
    double r = 0;
    int num = 0;
    cout << "请输入t的值。 "
         << "注意，如果输入的数少于" << t << "将无法得到秘密的任何信息" << endl;
    cin >> num;
    if (num < t)
    {
        cout << "无法恢复密钥" << endl;
        return -1;
    }
    else
    {
        for (int i = 0; i <= t - 1; ++i)
        {
            double R = 1;
            for (int m = 0; m <= t - 1; ++m)
            {
                if (m == i)
                    continue;
                R *= double(x[m]) / (double(x[m]) - double(x[i])); //避免舍入误差
            }
            r += R * y[i];
        }
    }
    return int(r);
}

int GeneratePrime()
{
    //including generating prime and Miller Robin Priminal Test.
    //Since we want to complete it by hand and do not get help with GMP, hope the size n is 32.

    //1.Generating the number:
    srand(time(NULL));
    clock_t ti;
    ti = clock();
    int n = 32;
    int t = 3 * (pow(n, 2));
    int l = 0;
    int decimal = 0;
    int result = 0;
    bool flag[2] = {0};
    for (l = 0; l < 2 * t; l++) // The times for the function to try to generate a number, if it exceeds,  it will be terminated.
    {
        //input and generate random n-1 bit numbers
        bool binary[32] = {0};
        decimal = 0;
        //Generate 0 or 1 and place it into the array
        for (int j = 0; j < n - 2; ++j)
        {
            binary[j] = rand() % 2;
        }
        //Convert to base 8
        for (int j = 0; j < n - 2; j++)
        {
            decimal = decimal * 2 + binary[j];
        }

        //2. Priminal Test:
        //it contains 3 parts:
        //1) if they are even, they are composite;
        //2) if they are perfect power, they are composite;
        //3) the main part of Miller Robin

        //1):
        decimal = decimal / 2 + decimal / 2 + decimal; //add "1" at the i bit.
        if (decimal % 2 == 0)
        {
            cout << "1composite" << decimal << endl;
            continue;
        }

        //2):
        int root = log2(decimal); //lower bound to calculate the perfect power.
        bool EndFlag = true;
        bool fail = false;
        while (EndFlag)
        {
            double temp = pow((double)decimal, 1.0 / root);
            int test = (int)temp;
            double origin = test + 0.0;
            if (temp - origin == 0)
                EndFlag = false;
            root--;
            if (root == 1)
            {
                fail = true;
                break;
            }
        }

        if (!fail)
        {
            cout << "2composite" << decimal << endl;
            continue;
        }

        //3)Main part of Miller Robin

        if (!isPrime(decimal, n))
        {
            cout << "3composite" << decimal << endl;
            continue;
        }
        result = decimal;
        cout << "prime found! :" << decimal << endl;
        break;
    }

    if (l == 2 * t) // Too many times, it failed.
    {
        cout << "failure" << endl;
        return -1;
    }
    return decimal;
}

int main()
{
    int upperBound = GeneratePrime()
    char filename[] = "test.txt";
    int n = 10;  //秘密分散成n份
    int t = 7;   //门限值
    int s = 123; //秘密
    if (SecretDistribution(n, t, s, filename,upperBound) != 0)
    {
        cout << "t值过大" << endl;
        return 0;
    }
    int r = SecretRecovery(filename);
    if (r == s)
        cout << "success" << endl;
    else
        cout << "fail" << endl;
    return 0;
}
