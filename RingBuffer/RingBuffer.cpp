// ringbuffa.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <crtdbg.h>
#include <stdlib.h>
#define _CRTDBG_MAP_ALLOC

class RingBuffer
{
private:
    char* BuffBeginPos = nullptr;

    //读取位置
    int BuffReadPos;
    //写入位置
    int BuffWritePos;
    //初始大小
    const int InitSize; 
    //当前大小
    int CurrentSize;    
    //存在的字符大小
    int ExistChar;
    //扩容的倍数
    int ExpandTime;

    void strCopy(char* NewChar, const char* OldChar)
    {
        for (int i = BuffReadPos; i < BuffWritePos; i++)
            NewChar[i- BuffReadPos] = OldChar[i];
    }

    void initNewBuffer(const int& NewSize)
    {
        char* NewBuffer = nullptr;
        NewBuffer = (char*)std::malloc(sizeof(char) * (NewSize + 1));

        if (NewBuffer and BuffBeginPos)
        {
            strCopy(NewBuffer, BuffBeginPos);
            free(BuffBeginPos);

            BuffBeginPos = NewBuffer;
            BuffBeginPos[ExistChar] = '\0';
            CurrentSize = NewSize;
        }
        else if (NewBuffer and !BuffBeginPos)
            BuffBeginPos = NewBuffer;
    }
public:
    RingBuffer(const int& ArraySize):InitSize(ArraySize), CurrentSize(ArraySize) , ExistChar(0) , ExpandTime(1) , BuffWritePos(0) , BuffReadPos(0) {
        initNewBuffer(ArraySize);
    }
    void push(const char* & Data, int DataLength)
    {
        if (!Data)return;
        DataLength -= 1;//去\0
        if (ExistChar + DataLength > CurrentSize)
        {
            ExpandTime = (ExistChar + DataLength) / InitSize + 1;
            initNewBuffer(ExpandTime * InitSize);
            if (CurrentSize < ExistChar + DataLength)
                return;//上面有警告，说可能有没有获取到内存空间的情况
        }
        for (int i = 0; i < DataLength and Data[i] != '\0'; i++)
        {
            BuffBeginPos[BuffWritePos] = Data[i];
            ++BuffWritePos;
        }
        ExistChar += DataLength;
    }
    void pop(char* OutChar, int OutSize)
    {
        if (!OutChar or ExistChar == 0)
            return;
        OutSize -= 1;
        OutChar[OutSize] = '\0';

        for (int i = 0; i < OutSize && BuffReadPos <= ExistChar; i++)
        {
            OutChar[i] = BuffBeginPos[BuffReadPos];
            ++BuffReadPos;
        }
        int DeleteSize = ExistChar > OutSize ?  OutSize : ExistChar;
        std::cout<< "OUT ->" << OutChar << std::endl;
        ExistChar -= DeleteSize;
        if (OutSize / InitSize >= 1 and ExpandTime > 1)
        {
            int ClearTime = OutSize / InitSize;
            ExpandTime -= ClearTime;
            initNewBuffer(ExpandTime * InitSize);
            BuffWritePos -= DeleteSize;
            BuffReadPos = ExistChar == 0? 0:BuffReadPos -DeleteSize;
        }
    }
    void PrintAll()
    {
        for (int i = BuffReadPos; i < BuffWritePos; i++)
            std::cout << BuffBeginPos[i];
        std::cout<< std::endl;
    }
};


int main()
{
    int initSize = 5;
    const char* data = "123456789";
    RingBuffer buffer = RingBuffer(initSize);
    srand(time(NULL));
    int length = strlen(data) + 1;
    for (int ii = 0; ii < 12; ii++)
    {
        std::cout << "----------------!!!!!!!!!!!!!!!!!!!!!!!!!! 開始PUSH" << "------------------------------------------------" << std::endl;
        int pushCount = rand() % 10;
        std::cout << pushCount << std::endl;
        for (int jj = 0; jj < pushCount; jj++)
        {
            buffer.push(data, length);    // 要支持自动扩容
            buffer.PrintAll();
        }
        std::cout << "----------------!!!!!!!!!!!!!!!!!!!!!!!!!! 開始PPPOOOP " << std::endl;
        char out[12];
        int popCount = rand() % 10;
        for (int jj = 0; jj < popCount; jj++)
        {
            buffer.pop(&out[0], sizeof(out));   // 要支持自动缩容
            buffer.PrintAll();
        }
    }
}
