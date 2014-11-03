#include <iostream>
#include<cstdlib>
#include<cmath>
//这是一个数据结构的课程设计，要求算出输入文件中的一些公式，这里我就把它做成一个支持文件格式的批处理计算器（这名字咋样哈哈,但是不支持负数计算）

//总的来说，犯的最大的错误就是用stack的top,pop等操作，没想到考虑其为空，所以导致各种崩溃，再一个，写程序先下逻辑，后具体到函数，即使暂时用注释代替也好
//免得到时候纠结于函数实现的细节，而忘了整体逻辑，导致逻辑上的失误和遗漏操作。
#include<stack>
#include<fstream>

using namespace std;

int IsOperator(char c); //判断是不是运算符
int OperatorPro(char Oc); //判断运算符的优先级
float ChartoFloat(stack<char>& CC); //char转换成float，针对多位数
float Compute(float a,float b, char c); //用取出的数字和运算符做运算
void PushWaitData(stack<char>& COperatorStack, stack<float>& CDataStack); //取出优先级低的运算符计算，后结果入栈
int main()
{
//    char shic = '8';
//    int a = (int)shic;
//    cout << "a is : " << a << endl;  //56,48+8我是验证一下，有种可以直接转换成8的错觉
    ifstream fin;
    ofstream fout;

    char tempc;
    stack<char> OperatorStack;
    stack<char> Datatemp;
    stack<float> DataStack;
    fin.open("input.txt");
    fout.open("result.txt",ios_base::out | ios_base::app);
    if(!fin.is_open())
    {
        cout <<"there is no such file !" << endl;
        exit(EXIT_FAILURE);
    }
    while(1)
    {

       tempc = fin.get();
        if(fin.eof()) break;
        if ('\n' == tempc)
            fout << ":    ";    //只是为了让格式好看点
        else
            fout << tempc;
         if(IsOperator(tempc) == 0)  //判断如果是数字的话,包括小数点
            {
                Datatemp.push(tempc);
            }
        if (IsOperator(tempc) == 1)  //判断如果是运算符
            {
                if (!Datatemp.empty())
                   {                            //这里问题是如何处理多位数的问题
                       DataStack.push(ChartoFloat(Datatemp));  //这里应该用引用传递，因为copy这里是不是还得清空元素（改成引用了，不用了,其函数内有pop）

                    }
                //按理说这里，为空时返回肯定少于，为什么还得判断（因为后面有栈的.top动作，为空就崩溃）
                if ( OperatorStack.empty()|| OperatorPro(tempc) >= OperatorPro(OperatorStack.top()) )  //如果运算符的级别高于栈定的级别则压入
                {                                               //上面变成>=时，避免了全是+++（同级别连续运算符）类型造成的崩溃,原因是没=就进入else里Pushdata了，连续几个+++就调用了,(未考虑边界（=）问题)
                    if (')' == tempc)        //是右括号时特殊
                    {
                        cout << "it is ) " << endl;         //这是检测是否查到（ 一开始总是不对，居然是因为输入是中文（,检测的是英文的(, 这里要求用英文，否则除了这里得判断，判断优先级也得判断
                        while(OperatorStack.size()!= 0 && OperatorStack.top() != '(')
                        {
                            try{

                                PushWaitData(OperatorStack,DataStack);
                            }
                            catch(const char* es)
                            {
                                fout << es << endl;
                                continue;
                            }
                        }
                        if (OperatorStack.size()!= 0) OperatorStack.pop();  //推出‘（’

                    }
                    else
                        {
                        OperatorStack.push(tempc);
                    }
                }
                else                                                         //如果优先级小于的话则要出栈运算，直到碰到大于
                {
                    if (OperatorStack.top() == '(')
                    {
                        cout << "it is ( " << endl;
                        OperatorStack.push(tempc);    //'('优先级是7 但是不能把前面的都计算了是不
                    }
                    else
                    {
                        while(OperatorStack.size() != 0 && OperatorPro(tempc) <= OperatorPro(OperatorStack.top()) )//这里多个运算符时一直崩溃，把判断0放到另一个判断条件前后就好了，因为判断的执行顺序，
                                                                                                                   //如果先判断优先级，top无值就会崩溃，把判0提前后不满足就不执行后面的判断了，细节
                         {
                            try{
                                PushWaitData(OperatorStack,DataStack);
                            }
                            catch(const char* es)
                            {
                                fout << es << endl;
                                continue;
                            }
                            cout << "OperateStack size after pushwait :" << OperatorStack.size() << endl;
                         }
                        OperatorStack.push(tempc);                      //把级别比其高的出栈后，把现有运算符压入（上面就纠结了好久就忘了这个，所以编程习惯，要先写上，再具体分析上面的函数，纠结过后就不会因为细节忘了逻辑了）
                    }

                 }
            }
            if('\n' == tempc)       //原本这里为了防止只有换行符崩溃的情形加了检查operat是否为空(错了，其实应该检查datastack，关于opera的操作前面都由有检查)，造成了漏下只有一个数字没有运算符的情形
            {
                cout << "in huan hang " << endl;
                cout << Datatemp.empty() << endl;
                    if (!Datatemp.empty())
                       {
                           DataStack.push(ChartoFloat(Datatemp));  // 处理最后一个数字，前面是碰到运算符处理，不适用于最后一个数字（没考虑边界问题）
                        }
                    cout << " DataStack.size():" <<DataStack.size() << endl;
                    cout << " Operator.size():" <<OperatorStack.size() << endl;

                   while ((DataStack.size() > 1)&&(!OperatorStack.empty()))  //加了大于1的判读是为了防止操作符数目大于数据个数时的错误输入导致崩溃（data为空的pop,top）,其错误信息在下面的排空operator中输出
                   {
                       try{
                            PushWaitData(OperatorStack,DataStack);
                       }
                       catch(const char* s)
                       {
                           fout << s ;
                           continue;
                       }
                   }
                   while(!DataStack.empty())                        //有空换行符崩溃是这里datastack为空而调了top,改到这里判断,考虑到只有一个数无运算符的情形if改成while是判断一行多个空格的数无操作符的情形，免得留在栈中影响下面的结果)，否则其实运算过后这里肯定有一个数的
                   {
                    fout << "The line result is : " << DataStack.top();            //这里是否也能把运算过程输出来(通过上面读字符串时直接输出解决)
                    DataStack.pop();          //全部推空，确保不影响下一行的计算
                   }
                   if (!OperatorStack.empty())
                   {
                        fout << "Bad input :";
                       while(!OperatorStack.empty())   //如果有运算符多的，必须排空，否则影响下行的计算
                       {
                           fout << OperatorStack.top() << " ";
                           OperatorStack.pop();
                       }
                   }
                   fout << endl;  //最后一个换行就好了
              }
    }
    fin.close();
    fout.close();
    return 0;
}
void PushWaitData(stack<char>& COperatorStack, stack<float>& CDataStack)
{
        cout << "in Pushwait Data " << endl;//为了检查运行到哪里崩溃的笨法
        char oc = COperatorStack.top();
        COperatorStack.pop();             //两个连续pop出错了。因为先前忘了推进最后一个数,而出了这个程序后还有个pop
        float oa,ob;
        if (CDataStack.empty())
        {
            return ;                      //排查到((()))时加上的，其实应该对于有运算符而数据不够的情形应该都适合 ,比如前面排除的+++
        }
        else
           {
            oa = CDataStack.top();
            CDataStack.pop();
        }
        if (CDataStack.empty())
        {
            return;
        }
        else{
            ob = CDataStack.top();
            CDataStack.pop();
        }
        cout << "ob: oa: oc :" << ob << " " << oa << " " << oc << endl;
        cout << "compute result: " << Compute(ob,oa,oc) << endl;
        CDataStack.push(Compute(ob,oa,oc));   //注意这里oa,ob的顺序，oa是后来的数



    cout << "out Pushwait" << endl;
}
float Compute(float a, float b, char c)
{
    float result = 0.0;
    switch (c) {
    case '+':
            result = a + b;
            break;
    case '-':
            result = a - b;
            break;
    case '*':
            result = a * b;
            break;
    case '/':
        if (0 == b) {throw "bad input cause a/b b = 0!";}
            result = a / b;                        //除数不能为0(这里可以用抛出异常来解决),先用-999999表示异常，成功
            break;
    case '%':
        if((a - (int)a) != 0 || ( b-(int)b ) != 0) throw "bad input cause float % !";
        else
        {
            result = (float)((int)a%(int)b);         //浮点数不能取余咋办？（也加上判断来抛出异常？）
            break;
        }
    case '^':
            result = pow(a,b);
            break;
    default:
        break;
    }
    return result;
}
float ChartoFloat(stack<char>& CC)
{
    int n = CC.size();
    cout << "Dataemp size is :" << n << endl;
    int flag = 0;     // 标记小数点的位置
    float Tdata = 0.0;
    char* tempcdata = new char[n];
    for(int i = 0 ; i < n; i++)
    {
        *(tempcdata + i ) = CC.top();
         CC.pop();
        if ('.' == *(tempcdata + i))
            flag = i;
    }
    for (int i = 0; i < flag ; ++i)  //找出小数点的位置后，小数点前乘和小数点后乘
        Tdata += ((int)(tempcdata[i] - '0')) * pow(0.1,double(flag - i));  //这里转换用这样么？？
    for (int i = flag ; i < n; ++i)
        Tdata += ((int)(tempcdata[i] -'0')) * pow(10.0,double(i - flag));
    cout << "Tdata is : " << Tdata << endl;
    return Tdata;
}
int IsOperator(char c)
{
    if ( '(' == c ||')' == c ||'+' == c ||'-' == c ||'*' == c ||'/' == c ||'^' == c ||'%' == c )
        return 1;
    else
    {
        //是数字或者小数点
        if('.' == c ||'0' == c ||'1' == c ||'2' == c ||'3' == c ||'4' == c ||'5' == c ||'6' == c ||'7' == c ||'8' == c ||'9' == c )
            return 0;
        else return -1;  //其他的返回-1
    }
}
int OperatorPro(char Oc)
{
    //其实这里我在想用switch还是用if效率高
    if ('(' == Oc) return 6;
    if (')' == Oc) return 5;
    if ('^' == Oc) return 3;
    if ('%' == Oc) return 2;
    if ('*' == Oc) return 2;
    if ('/' == Oc) return 2;
    if ('+' == Oc) return 1;
    if ('-' == Oc) return 1;
    return 0;
}

