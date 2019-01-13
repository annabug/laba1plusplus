#include <iostream>
#include <string>
#include "math.h"
#include <algorithm>
#include <stdlib.h>
#include <iomanip>

using namespace std;



void outputHexBI(uint32_t* masBI)//функція виводу
{
    int notNullPos = 0;
    while(masBI[notNullPos] == 0)
    {
        notNullPos++;
        if(notNullPos == 64)
        {
            cout << 0 << endl ;
            return;
        }
    }
    for(int i = notNullPos; i < 64; i++)
    {

        cout << hex <<  masBI[i];
    }
    cout << endl;
}

void hexStrToDecInt(string input, uint32_t* masBI)
{
     string temp = "00000000";
    int sizeOfBI = 63;
    int sizeTemp = 7;


    for (int i = input.length() - 1; i > -1; i--)
    {
        if( sizeTemp > 0)
        {
            temp[sizeTemp] = input[i];
            sizeTemp--;
            masBI[sizeOfBI] = strtoul(temp.c_str(), nullptr, 16);
        }
        else
        {
            temp[sizeTemp] = input[i];
            masBI[sizeOfBI] = strtoul(temp.c_str(), nullptr, 16);
            sizeOfBI--;
            sizeTemp = 7;
            temp = "00000000";
        }


    }
}

int comparison(uint32_t* operand1, uint32_t* operand2)// якщо op1 = op2 - то  0.    op1 > op2 - то  1. если op1 < op2 - то -1.
{
    int iter = 0;
    while (operand1[iter] == operand2[iter])
    {
        iter++;
        if(iter == 64)
        {
            return 0;
        }
    }
    if(operand1[iter] > operand2[iter])
    {
        return 1;
    }
    else
    {
        return -1;
    }

}

void addition(uint32_t* resultBI, uint32_t* operand1, uint32_t* operand2)
{
    uint64_t temp, a, b, carry = 0;
    for(int i = 63; i> -1; i--)
    {
        a = operand1[i];
        b = operand2[i];
        temp = a +b + carry;
        resultBI[i] = temp & 0xFFFFFFFFUL;
        carry = temp >> 32;
    }

}

void substraction(uint32_t* result, uint32_t* operand1, uint32_t* operand2)
{

    int cmpRes = comparison(operand1, operand2);
    int64_t r = 0, temp = 0;
    uint32_t borrow = 0;
    if(cmpRes == 0)
    {
        for(int i = 0; i < 64; i++)
        {
            result[i] = 0;
        }
    }
    else if(cmpRes == 1 )
    {
     for(int i = 63; i > -1; i--)
     {
         temp = (uint64_t)operand1[i] - operand2[i] - borrow;
         if(temp >= 0)
         {
             r = temp;
             borrow = 0;
         }
         else
         {
             r = temp + 4294967296UL;
             borrow = 1;
         }
         result[i] = r;
     }
    }
    else if( cmpRes == -1)
    {
          for(int i = 63; i > -1; i--)
     {
         temp = (uint64_t)operand2[i] - operand1[i] - borrow;
         if(temp >= 0)
         {
             r = temp;
             borrow = 0;
         }
         else
         {
             r = temp + 4294967296UL;
             borrow = 1;
         }
         result[i] = r;
     }
    }
    else
    {
        cout << "COMPARISON ERROR!" << endl;
    }
}

void setValue(uint32_t* operand1, uint32_t* operand2)//операція присвоєння
{
    for(int i = 0; i < 64; i++)
    {
        operand1[i] = operand2[i];
    }
}

void smallMul(uint32_t* opBI, uint32_t* operand1, uint32_t op)
{
    uint64_t temp, t, c = op;
    uint64_t carry = 0;

    for(int i = 63; i > -1; i--)
    {
        t = operand1[i];
        temp = t*c + carry;
        opBI[i] = temp & 0xFFFFFFFFUL;
        carry = temp >> 32;
    }

}

void longShiftToHigh(uint32_t* opBI, int shift)
{
    uint32_t newBI[64] = {};
    for(int i = 0; i < 64-shift; i++)
    {
        newBI[i] = opBI[i+shift];
    }
    for(int i = 0; i < 64; i++)
    {
        opBI[i] = newBI[i];
    }
}

void multiplication(uint32_t* result, uint32_t* operand1, uint32_t* operand2)
{


    for(int i = 63; i > -1; i--)
    {
        uint32_t tempOpBI[64] = {};
        smallMul(tempOpBI,operand1, operand2[i]);
        longShiftToHigh(tempOpBI, 63-i);
        addition(result, result, tempOpBI);

    }

}

void powSquare(uint32_t* result, uint32_t* operand1)// не передавати один і той же ВІ як різні параметри (result != operandBI) бо множення починає множити фігню
{
    multiplication(result, operand1, operand1);
}

int getBitLength(uint32_t* opBI)
{
    uint32_t temp = 0;

    int notEmptyPos = 0;
    int bitLen = 0;

    for ( notEmptyPos; notEmptyPos < 64; notEmptyPos++)
    {
        if(opBI[notEmptyPos] != 0)
        {
            break;
        }
    }

    temp = opBI[notEmptyPos];

    for(int j = 31; j > -1; j--)
    {
        if( ( ( temp >> j ) & 1 ) == 1 )
        {
            j++;
            break;
        }
        bitLen = j;
    }
    bitLen += 32*(63 - notEmptyPos);

    return bitLen;
}

int getBit(uint32_t* opBI, int bitPos)
{
    int temp = getBitLength(opBI);
    int i = 63 - bitPos/32;
    if( bitPos < temp)
    {
        temp = bitPos%32;
        return ((opBI[i] >> temp ) & 1);
    }
}

void powBI(uint32_t* result, uint32_t* operand1, uint32_t* operand2)
{
    for(int i =0; i< 64; i++)
    {
        result[i] = 0;
    }
    hexStrToDecInt("1", result);
    uint32_t kostil[64] ={};
    int t = 0;
    int i = getBitLength(operand2);

    for( i; i > -1; i--)
    {
        t = getBit(operand2, i);
        if(t == 1)
        {
            multiplication(result, result, operand1);

        }
        if(i != 0)
        {
            powSquare(kostil, result);
            setValue(result, kostil);
            for(int i = 0; i < 64; i++)
            {
                kostil[i] = 0;
            }
        }
    }
}







 int main()
{
    setlocale(LC_ALL, "rus");
    uint32_t masBI[64] = {};
    uint32_t masBI2[64] = {};
    uint32_t resultBI[64] = {};

    string strNum = "F13BBF47D24E9F5F2356983F760F99ED0B52AFDC796E484B2D55738DD21F70B081FE6788F9B91DA3DB4AD64437667F85FEFA1EBB5B762C8885BC5C6CBD7835A93E6315E67DE54B575EDB91ADAC0580111A001241EB375F8618AB00C30C9F29A357CE61B47741647B07D536D2326B1069AC75519783DDC45C59568D3F7CA629AE";//"21E8D922A2A123DD";
    hexStrToDecInt(strNum, masBI);

    strNum = "E5CA2B1204CEC6C2502788E53F5B5BFCB831D14A21AD5615136D1319DE293A8BC05A710DC59B95628176A3095936F69213911760F57815D19044C56EC090E148E0DE9B9FC166BA72B0F488E83A451ACC742BF1FFDD0C1DDC3DD76F1CA33D0883932067E3B55A49DEDCE12648FF84F728ABCCC67769DEC895CBCA69209602222";//"21E8D922A";
    hexStrToDecInt(strNum, masBI2);


    outputHexBI(masBI);
    outputHexBI(masBI2);
    outputHexBI(resultBI);
//    int res = comparison(masBI, masBI2);
//    cout << endl << res << endl;

    //+
//    addition(resultBI, masBI, masBI2);
//    outputHexBI(resultBI);

//    for(int i = 0 ; i < 64; i++)
//    {
//        resultBI[i] = 0;
//    }
//    outputHexBI(resultBI);

      //-
//    substraction(resultBI, masBI, masBI2);
//
//    outputHexBI(resultBI);

    //*
//    multiplication(resultBI, masBI, masBI2);
//    outputHexBI(resultBI);

    //^2
//    powSquare(resultBI, masBI);
//    outputHexBI(resultBI);

    //^
//    powBI(resultBI, masBI, masBI2);
//    outputHexBI(resultBI);

    cout << endl << endl;

    outputHexBI(masBI);
    outputHexBI(masBI2);


    cin.get();
    cin.get();
    return 0;
}



