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

    //��ȡλ��
    int BuffReadPos;
    //д��λ��
    int BuffWritePos;
    //��ʼ��С
    const int InitSize; 
    //��ǰ��С
    int CurrentSize;    
    //���ڵ��ַ���С
    int ExistChar;
    //���ݵı���
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
        DataLength -= 1;//ȥ\0
        if (ExistChar + DataLength > CurrentSize)
        {
            ExpandTime = (ExistChar + DataLength) / InitSize + 1;
            initNewBuffer(ExpandTime * InitSize);
            if (CurrentSize < ExistChar + DataLength)
                return;//�����о��棬˵������û�л�ȡ���ڴ�ռ�����
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
        std::cout << "----------------!!!!!!!!!!!!!!!!!!!!!!!!!! �_ʼPUSH" << "------------------------------------------------" << std::endl;
        int pushCount = rand() % 10;
        std::cout << pushCount << std::endl;
        for (int jj = 0; jj < pushCount; jj++)
        {
            buffer.push(data, length);    // Ҫ֧���Զ�����
            buffer.PrintAll();
        }
        std::cout << "----------------!!!!!!!!!!!!!!!!!!!!!!!!!! �_ʼPPPOOOP " << std::endl;
        char out[12];
        int popCount = rand() % 10;
        for (int jj = 0; jj < popCount; jj++)
        {
            buffer.pop(&out[0], sizeof(out));   // Ҫ֧���Զ�����
            buffer.PrintAll();
        }
    }
}
