#include <iostream>
#include <cstring>
#include <fstream>
#include <cassert>
#include <string>

using namespace std;

int SecretDistribution(int n, int t, int s, char* filename)
{
    //除a0=s外，其余系数为1
    ofstream outfile;
    outfile.open(filename);
    //打印多项式
    outfile << "polynomial: f(x)=" ;
    outfile << s;  //秘密，即a0
    for (int i = 1; i < t; ++i)
    {
        outfile << "+x";
        if (i >= 2)
        {
            outfile << '^'<<i;
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
            y += pow(j, k);
            if (y >= 2147483647) { return -1; }//防止int越界
        }
        outfile << '(' << j << ',' << int(y) << ')' << endl;
    }
    outfile << "t=" << t << endl;
    outfile.close();
    return 0;
}

int SecretRecovery(char* filename)
{
    int t = 0;
    int x[100] = { 0 };
    int y[100] = { 0 };
    int xasix = 0;
    int yasix = 0;
    ifstream in(filename);
    ifstream infile;
    infile.open(filename);   //将文件流对象与文件连接起来 
    assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行
    char c[1000] = { 0 };//读入的字符
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
        if ((c[j] == '(')&&(c[j+1] != 'x')) //读取x
        {           
            for (k = j + 1; c[k] != ','; ++k)
            {
                x[xasix] = x[xasix] * 10 + (c[k]-'0');              
            }
            for (k = k + 1; c[k] != ')'; ++k)//读取y
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
    cout << "请输入t的值。 " << "注意，如果输入的数少于" << t << "将无法得到秘密的任何信息" << endl;
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
                if (m == i) continue;
                R *= double(x[m]) / (double(x[m]) - double(x[i]));//避免舍入误差
            }
            r += R * y[i];
        }
        
    }
    return int(r);

}

int main()
{
    char filename[] = "test.txt";
    int n = 10;//秘密分散成n份
    int t = 7;//门限值
    int s = 123;//秘密
    if (SecretDistribution(n, t, s, filename) != 0)
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
