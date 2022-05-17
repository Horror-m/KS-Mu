
#include <iostream>
#include <memory>
#include <stack>
using namespace std;

//void test()
//{
//    shared_ptr<SkipListNode> a = make_shared<SkipListNode>();
//}

struct SkipListNode
{
private:

public:
    /*~SkipListNode() {
        std::cout << "析构" << std::endl;
    };*/
    int Value;
    //shared_ptr<SkipListNode> Next = nullptr;
    //shared_ptr<SkipListNode> Down = nullptr;//想用智能指针,但是还是打算自己管理一下内存
    SkipListNode* Next = nullptr;
    //SkipListNode* Previous = nullptr;
    SkipListNode* Down = nullptr;
};

class SkipListInterface
{
private:
    SkipListNode* ValueHead = nullptr;
    SkipListNode* IndexHead = nullptr;
    const float FirstLevelRandom = 0.5f;
    const int MaxLevel = 10;
    int CurLevelHigh = 1;
    /// <summary>
    /// RAND_MAX = 32,767 = ob 0111111111111111 = 0x 7FFF 所以我取了后三位F
    /// </summary>
    /// <returns></returns>
    int getRandomLevel()
    {
        int RandomLevel = 0;
        while (rand() & 0xFFF > FirstLevelRandom * 0xFFF && RandomLevel < MaxLevel)
            ++RandomLevel;
        std::cout << RandomLevel << std::endl;
        return RandomLevel;
    }
    /// <summary>
    /// update new IndexHeadPointer
    /// </summary>
    /// <param name="CurUpdateLevel"></param>
    int updateLevel(int CurUpdateLevel)
    {
        int OldLevelHight = CurLevelHigh;
        if (CurLevelHigh < CurUpdateLevel)
        {
            SkipListNode* Temp = nullptr;
            do
            {
                Temp = IndexHead;
                IndexHead = new SkipListNode;
                IndexHead->Value = Temp->Value;
                IndexHead->Down = Temp;
                ++CurLevelHigh;
            } while (CurLevelHigh < CurUpdateLevel);
        }
        return OldLevelHight;
    }
    /// <summary>
    /// 横向查找
    /// </summary>
    /// <param name="find"></param>
    /// <param name="FindValue"></param>
    /// <returns></returns>
    bool findValueSuitPreviewNode(SkipListNode* &find, int FindValue)
    {
        SkipListNode* NextNode = find->Next;
        while (NextNode != nullptr && NextNode->Value <= FindValue)
        {
            if (NextNode->Value == FindValue)
                return false;
            find = NextNode;
            NextNode = NextNode->Next;
        }
        return true;
    }

    /// <summary>
    /// 已经有值的话返回 nullptr
    /// </summary>
    /// <param name="InsertPointer"></param>
    /// <param name="NewValue"></param>
    /// <returns></returns>
    SkipListNode* findAndInsertNewValue(SkipListNode* &InsertPointer, int NewValue)
    {
        if (!findValueSuitPreviewNode(InsertPointer, NewValue))
            return nullptr;
        SkipListNode* NewInsertNode = new SkipListNode;
        NewInsertNode->Value = NewValue;
        NewInsertNode->Next = InsertPointer->Next;
        InsertPointer->Next = NewInsertNode;
        return NewInsertNode;
    }

public:
    //临时测试
    //int a = 1;
    bool insertValue(int InsertValue)
    {

        if (-INT_MAX >= InsertValue)
            return false;
        int RandomLevel = getRandomLevel();
        //临时测试
        //switch (a)
        //{
        //case 1:
        //    RandomLevel = 1;
        //    break;
        //case 2:
        //    RandomLevel = 3;
        //    break;
        //case 3:
        //    RandomLevel = 2;
        //    break;
        //default:
        //    break;
        //}
        //++a;
        updateLevel(RandomLevel);

        int CurViewLevel = CurLevelHigh;
        SkipListNode* InsertPointer = IndexHead;
        SkipListNode* NewCreateNode = nullptr;

        while (InsertPointer != nullptr)
        {
            if (InsertValue == InsertPointer->Value)//|| CurViewLevel <= RandomLevel && !findAndInsertNewValue(InsertPointer, InsertValue))
                return false;
            if (CurViewLevel <= RandomLevel)
            {
                if (NewCreateNode == nullptr)
                    NewCreateNode = findAndInsertNewValue(InsertPointer, InsertValue);//这里说明已经有这个值
                else
                {
                    NewCreateNode->Down = findAndInsertNewValue(InsertPointer, InsertValue);
                    NewCreateNode = NewCreateNode->Down;
                }
                if (NewCreateNode == nullptr)
                    return false;
            }
            if (InsertPointer->Value >= InsertValue || InsertPointer->Next == nullptr || InsertPointer->Next->Value < InsertValue)
                if(!findValueSuitPreviewNode(InsertPointer, InsertValue))
                    return false;
            InsertPointer = InsertPointer->Down;
            --CurViewLevel;//
        }
        return true;
    }
    bool deleteValueInList(int Value)
    {
        SkipListNode* FindPointer = IndexHead;
        SkipListNode* FreeBolck = nullptr;
        bool FindValue = false;
        while (FindPointer != nullptr)
        {
            if (!findValueSuitPreviewNode(FindPointer, Value))
            {
                FindValue = true;
                FreeBolck = FindPointer->Next;
                FindPointer->Next = FindPointer->Next->Next;
                free(FreeBolck);
            }
            FindPointer = FindPointer->Down;
        }
        return FindValue;
    }
    SkipListInterface()
    {
        srand((unsigned)time(NULL));
        ValueHead = new SkipListNode;
        ValueHead->Value = -INT_MAX;

        IndexHead = new SkipListNode;
        IndexHead->Value = -INT_MAX;
        IndexHead->Down = ValueHead;
    }

};

int main()
{
    SkipListInterface SkipList;
    SkipList.insertValue(2);
    SkipList.insertValue(0);
    SkipList.insertValue(6);
    SkipList.insertValue(0);

    SkipList.deleteValueInList(1);
    SkipList.deleteValueInList(2);
    //SkipList.insertVaue();
    //SkipListNode b;// = new SkipListNode;
    //b.Value = 111;
    //shared_ptr<SkipListNode> a = make_shared<SkipListNode>(b);
    //std::cout << a.get()->Value << std::endl;
    ////test();
    ////stack<int>* TestStack = new stack<int>;
    ////TestStack->push(3);
    ////std::cout << TestStack << std::endl;

    srand(time(NULL));
    //std::cout << std::rand() << std::endl;
    //std::cout << RAND_MAX <<std::endl;
}
