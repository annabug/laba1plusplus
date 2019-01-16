#include <iostream>
#include <string>
#include "math.h"
#include <algorithm>
#include <stdlib.h>
#include <iomanip>

using namespace std;

setNull(uint32_t* op)
{
    for(int i = 0; i < 64; i++)
    {
        op[i] = 0;
    }
}

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
    int i = 0;
    i = 63 - bitPos/32;
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
           setNull(kostil);
        }
    }
}

///////////////////////////////////////////laba2/////////////////////////////////////////////////////////////////////////////

void shiftRight(uint32_t* op)// /2
{

    for(int i = 63; i > 0; i--)
    {
       op[i]= (op[i]>>1)| (op[i-1] << 31);
    }
    op[0]=op[0]>>1;

}

void shiftLeft(uint32_t* op)// *2 mod 2048bit
{
    for(int i = 0; i < 63; i++)
    {
        op[i] = ((op[i] << 1) | (op[i+1] >> 31));
    }
    op[63] = op[63] << 1;
}

void shiftBitLeftK(uint32_t* op, int k)
{
    while(k!=0)
    {
       for(int i = 0; i < 63; i++)
    {
        op[i] = ((op[i] << 1) | (op[i+1] >> 31));
    }
    op[63] = op[63] << 1;
    k--;
    }

}
void gcd(uint32_t* result, uint32_t* operand1, uint32_t* operand2)//gcd from "Handbook of applied cryptography 14.4.1"
{
    uint32_t op1Temp[64] ={};
    uint32_t op2Temp[64] ={};
    setValue(op1Temp, operand1);
    setValue(op2Temp, operand2);

    uint32_t testZero[64] ={};
    uint32_t t[64] = {};

    int whoIsBigger = 0;

    uint32_t gcd[64] = {};
    gcd[63] = 1;

    while( (comparison(op1Temp, testZero) && comparison(op2Temp, testZero)) && !(op1Temp[63] & 1 ) && !(op2Temp[63] & 1))
    {

        shiftRight(op1Temp);
        shiftRight(op2Temp);
        shiftLeft(gcd);
    }
    while( comparison(op1Temp,testZero))
    {
        while((comparison(op1Temp, testZero)) && (!(op1Temp[63] & 1 )) )
        {
            shiftRight(op1Temp);
        }
        while((comparison(op2Temp, testZero)) && (!(op2Temp[63] & 1)))
        {
            shiftRight(op2Temp);
        }
        whoIsBigger = comparison(op1Temp, op2Temp);
        substraction(t, op1Temp, op2Temp);
        shiftRight(t);
        if((whoIsBigger == 1) || (whoIsBigger == 0))
        {
            setValue(op1Temp, t);
        }
        else if(whoIsBigger == -1)
        {
            setValue(op2Temp, t);
        }
        else{cout << "YOU ARE IN BIG TROUBLE" << endl;}

    }
    multiplication(testZero, gcd, op2Temp);// testZero  = g*y
    setValue(result, testZero);
}

void insertBit(uint32_t* op, int bitPos) // встановлює 1 не у вказану позицію а в наступну зліва, критичне місце в середині(звісно, якщо нумерація бітів не з 0)
{
    uint32_t bit[64] = {};
    uint32_t b  = 1; // можна було б змінну b взагалі не створювати, але все одно пам'ять вивільниться
    b = b << (bitPos%32); // МІСЦЕ НОМЕР ОДИН ДЛЯ ПОШУКУ ПОМИЛКИ (bitPos%32 - 1) (якщо нумерація не з 0
    bit[63 - bitPos/32] = b;

    uint32_t uncertainMoment[64] = {}; // теж не пам'ятаю чи коректно оброблюється випадок addition(A, A, B)

    addition(uncertainMoment, op, bit);
    setValue(op, uncertainMoment);
}

void divisionBImodQ(uint32_t* result, uint32_t* operand1, uint32_t* operand2) // result = Q ; operand1 = R
{
    uint32_t R[64] ={};
    uint32_t Q[64] ={};
    uint32_t C[64] ={};

    uint32_t uncertainMoment[64] = {};
    int k = getBitLength(operand2);
    int temp = 0;
    setValue(R, operand1);
    while(comparison(R, operand2) != -1) // (comparison(R, operand2) == 1) || (comparison(R, operand2) == 0)
    {
        temp = getBitLength(R);
        setValue(C, operand2);
        shiftBitLeftK(C, temp - k);
        if(comparison(R, C) == -1)
        {
            temp--;
            setValue(C, operand2);
            shiftBitLeftK(C, temp - k);
        }
        substraction(uncertainMoment, R, C);
        setValue(R, uncertainMoment);

        insertBit(Q, temp-k);

    }
    setValue(result, Q);
}
void shift(uint32_t* op, int shift)//сдвигання масиву
{
    uint32_t temp[64] = {};

    for(int i = 0; i+shift < 64; i++)
    {
        temp[i+shift] = op[i];
    }
    for(int i = 0; i < 64; i++)
    {
        op[i] = temp[i];
    }
}

void meow(uint32_t* op, uint32_t* n, int k) //грекі саме так і говорили "мю" інфа 100%
{
    uint32_t temp[64] = {};
    uint32_t uncertainMoment[64] = {};

    if(k == 32 )
    {
      temp[0] = 1;
    }
    else{    temp[63-2*k] = 1; }

    divisionBImodQ(uncertainMoment, temp, n);

    setValue(op, uncertainMoment);

}

void redBurito(uint32_t* op, uint32_t* modN, uint32_t* meowBI)//Barett modular reduction
{

    uint32_t Q[64] = {};
    uint32_t temp[64]= {};
    setValue(temp, op);
    setValue(Q, op);
    int len = 0;

    for(int i = 0; i < 64; i++)
    {
        if(modN[i]!=0)
        {
            len = 64 - i;
            break;
        }
    }

    shift(Q,(len-1));

    uint32_t uncertainMoment[64] = {};

    multiplication(uncertainMoment, Q, meowBI);
    setValue(Q, uncertainMoment);

    shift(Q, len+1);

    setNull(uncertainMoment);
    multiplication(uncertainMoment, Q, modN);
    setValue(Q, uncertainMoment);
    setNull(uncertainMoment);
    substraction(op, temp, Q);

    while(comparison(op, modN) != -1) // (comparison(op, modN) == 1) || (comparison(op, modN) == 0)
    {
        //cout << "point " ;// бесконечный циклл на 1024 DEBUG
        substraction(uncertainMoment, op, modN);
        setValue(op, uncertainMoment);
        setNull(uncertainMoment);
    }

}
void powModBI(uint32_t* result, uint32_t* operand1, uint32_t* operand2, uint32_t* modN)
{
    uint32_t uncertainMoment[64] ={};
    uint32_t temp[64] = {};
    temp[63] = 1;
    setValue(result, temp);
    setNull(temp);


   int len = 0;
    for(int i = 0; i < 64; i++)
    {
        if(modN[i]!=0)
        {
            len = 64 - i;
            break;
        }
    }
    meow(temp, modN, len);
    len = getBitLength(operand2);
    for(int i = len-1; i > -1; i--)
    {
        if(getBit(operand2, i) == 1)
        {
            multiplication(uncertainMoment, result, operand1);
            setValue(result, uncertainMoment);
            setNull(uncertainMoment);
redBurito(result, modN, temp);

        }
        if(i != 0)
        {
            multiplication(uncertainMoment, result, result);

            setValue(result, uncertainMoment);
            setNull(uncertainMoment);

            redBurito(result, modN, temp);


        }
    }
}
void NSKolimpiyskiy(uint32_t* result, uint32_t* operand1, uint32_t* operand2)
{
     uint32_t uncertainMoment[64] = {};
    multiplication(uncertainMoment, operand1, operand2);
    uint32_t gcd1[64] ={};
    gcd(gcd1, operand1, operand2);
    divisionBImodQ(result, uncertainMoment, gcd1);
}

void additionModN(uint32_t* result, uint32_t* operand1, uint32_t* operand2, uint32_t* modN)
{
    int len = 0;
     for(int i = 0; i < 64; i++)
    {
        if(modN[i]!=0)
        {
            len = 64 - i;
            break;
        }
    }
    uint32_t mw[64] = {};
    meow(mw, modN, len);
    addition(result, operand1, operand2);
    redBurito(result, modN, mw);
}

void substractionModN(uint32_t* result, uint32_t* operand1, uint32_t* operand2, uint32_t* modN)
{
    int len = 0;
     for(int i = 0; i < 64; i++)
    {
        if(modN[i]!=0)
        {
            len = 64 - i;
            break;
        }
    }
    uint32_t mw[64] = {};
    meow(mw, modN, len);
    substraction(result, operand1, operand2);
    redBurito(result, modN, mw);
}

void multiplicationModN(uint32_t* result, uint32_t* operand1, uint32_t* operand2, uint32_t* modN)
{
    int len = 0;
     for(int i = 0; i < 64; i++)
    {
        if(modN[i]!=0)
        {
            len = 64 - i;
            break;
        }
    }
    uint32_t mw[64] = {};
    meow(mw, modN, len);
    multiplication(result, operand1, operand2);
    redBurito(result, modN, mw);
}


 int main()
{
    setlocale(LC_ALL, "rus");
    uint32_t masBI[64] = {};
    uint32_t masBI2[64] = {};
    uint32_t resultBI[64] = {};
    uint32_t modN[64] = {};


    string strNum = "F13BBF47D24E9F5F2356983F760F99ED0B52AFDC796E484B2D55738DD21F70B081FE6788F9B91DA3DB4AD64437667F85FEFA1EBB5B762C8885BC5C6CBD7835A93E6315E67DE54B575EDB91ADAC0580111A001241EB375F8618AB00C30C9F29A357CE61B47741647B07D536D2326B1069AC75519783DDC45C59568D3F7CA629AE";//"21E8D922A2A123DD";
    hexStrToDecInt(strNum, masBI);

    strNum = "E5CA2B1204CEC6C2502788E53F5B5BFCB831D14A21AD5615136D1319DE293A8BC05A710DC59B95628176A3095936F69213911760F57815D19044C56EC090E148E0DE9B9FC166BA72B0F488E83A451ACC742BF1FFDD0C1DDC3DD76F1CA33D0883932067E3B55A49DEDCE12648FF84F728ABCCC67769DEC895CBCA69209602222";//"21E8D922A";
    hexStrToDecInt(strNum, masBI2);

    strNum = "2A12C6DB78A41652BAE8A4568CE7528B745542051619CE3D67522CB14BEA13B815124F5CCF2FB6313FADB9ADDF01868A9671DE014C1F35DADA5CF424E1C2ECF2E2F7CB64CFB7C4F4968E664DC6910250C6B63A04C81CB2333E8F83D1B41C3D2AC955EB3610BF6CD02D66D28450B11FDBF53430CAE59787553B8003654685F2C1";
    hexStrToDecInt(strNum, modN);

    outputHexBI(masBI);
    outputHexBI(masBI2);
    outputHexBI(modN);
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

//діленння на 2
//     shiftRight(resultBI);
//     outputHexBI(resultBI);

    // множення на 2 mod 2048bit
//    shiftLeft(masBI2);

    //gcd
//    gcd(resultBI, masBI, masBI2);
//    outputHexBI(resultBI);

    //pow modN
    powModBI(resultBI, masBI, masBI2, modN);
    outputHexBI(resultBI);

    //NSK
//    NSKolimpiyskiy(resultBI, masBI, masBI2);
//    outputHexBI(resultBI);

    //+ modN
//    additionModN(resultBI, masBI, masBI2, modN);
//    outputHexBI(resultBI);

    //- modN
//    substractionModN(resultBI, masBI, masBI2, modN);
//    outputHexBI(resultBI);

    // * modN
//    multiplicationModN(resultBI, masBI, masBI2, modN);
//   outputHexBI(resultBI);


    cout << endl << endl;

    outputHexBI(masBI);
    outputHexBI(masBI2);
    outputHexBI(modN);



    cin.get();
    cin.get();
    return 0;
}



