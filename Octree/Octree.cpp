// Octree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
//class Coordinate
//{
//public:
//    float X;
//    float Y;
//    float Z;
//    //int Hash();
//    Coordinate(float GetX, float GetY, float GetZ): X(GetX), Y(GetY), Z(GetZ) {
//        //calculateHash();
//    }
//    //int GetMyHash() {
//    //    //std::hash(X);
//    //    std::hash();
//    //}
//    float operator[] (const int& index)
//    {
//
//    }
//private:
//    /*void calculateHash() {
//
//    }*/
//};

#define Vec3f std::vector<float>// 0:X  1:Y  2:Z
#define ActorInfo std::pair<int ,Vec3f> //������Ϣ
//ͼ���ĵ���  ͼ���ĵ���  ͼ���ĵ���    ͼ���ĵ���  ͼ���ĵ���  ͼ���ĵ���  ͼ���ĵ���  ͼ���ĵ���  ͼ���ĵ���  ͼ���ĵ���  ͼ���ĵ���!!!!!!!!!!!!!!!!!!!
//Ȼ����ǰ���и���������Ķ���ħ��, 0��������ǰ,  1������ǰ, 2�����º�, 3�����Ϻ�, �ұ߶Գ�+4
//�Լ��ƶ����� �������������� 10,���Ҳ��ظ����������10�� ���+1

static const int OneLevelCubeAllSum = 8;
static const int Dimentional = 3;
struct MyOctree;//�۲��� �ҷ���classһ�������� ��struct��ͬ������ԭ������Ϊ������һ�ֶ��������Լ��ݰɣ�֮�����

/// <summary>
/// ��ÿ��Ҷ�Ľṹ
/// std::unordered_map<int, Vec3f> AllObj
/// MyOctree* NextOctree
/// </summary>
struct OctreeCell
{
    std::unordered_map<int, Vec3f> AllObj;//��һ������int ������actorID��
    MyOctree* NextOctree = nullptr;
};

/// <summary>
/// ���ṹ
/// std::vector(OctreeCell) OneLevelCube
/// Vec3f Size
/// Vec3f Center
/// </summary>
struct MyOctree
{
    std::vector<OctreeCell> OneLevelCube = std::vector<OctreeCell>(OneLevelCubeAllSum);
    Vec3f Size;
    Vec3f Center;
};

class OctreeInterface
{
private:
    //��ת������
    char* intToBin(int VertToBin)
    {
        char* Bin = new char[Dimentional];
        for (int i = 0; i < Dimentional; i++)
        {
            int AntitoneIndex = Dimentional - i - 1;
            Bin[AntitoneIndex] = VertToBin % 2 == 0 ? '0' : '1';
            VertToBin /= 2;
        }
        return Bin;
    }
    //��չ��
    void expandMyOctree(MyOctree& ExpandOctree, int ExpandIndex)
    {
        char* Bin = intToBin(ExpandIndex);
        std::cout << Bin << std::endl;
        Vec3f NextLevelCenter = Vec3f(Dimentional), NextLevelSize = Vec3f(Dimentional);
        for (int i = 0; i < Dimentional; i++)
        {
            NextLevelSize[i] = ExpandOctree.Size[i] / 2;
            float ChangeValue = NextLevelSize[i] / 2;
            NextLevelCenter[i] = Bin[i] == '1' ? ChangeValue : ChangeValue * -1;
            NextLevelCenter[i] += ExpandOctree.Center[i];
        }
        OctreeCell* CurCell = &(ExpandOctree.OneLevelCube[ExpandIndex]);
        //MyOctree Temp = initMyOctree(NextLevelSize, NextLevelCenter);
        //&Temp;//initMyOctree(NextLevelSize, NextLevelCenter); //���к���ֵĵط� �Ժ�Ҫָ�� �ͷ���ָ�룬�����������ڿ��ܾ�������
        CurCell->NextOctree = initMyOctree(NextLevelSize, NextLevelCenter);

        for (auto it = CurCell->AllObj.begin(); it != CurCell->AllObj.end(); it++)
            pushActorWithPosition(&ExpandOctree, *it);
        CurCell->AllObj.clear();
    }
public:
    //��������
    void pushActorWithPosition(MyOctree* TargetOctree, const ActorInfo& Actor)
    {
        if (!TargetOctree)
            return;
        Vec3f Pos = Actor.second;
        OctreeCell* TargetCell = nullptr;
        MyOctree* LastOctree = TargetOctree;
        bool NotFinalPoint = true;
        int CubeIndex = -1;
        do
        {
            Vec3f Center = LastOctree->Center;
            CubeIndex = findOctreeIndex(Center, Pos);
            *&TargetCell = &LastOctree->OneLevelCube[CubeIndex];
            if (TargetCell->NextOctree != nullptr)
                LastOctree = TargetCell->NextOctree;
            else
                NotFinalPoint = false;
        } while (NotFinalPoint);
        TargetCell->AllObj.emplace(Actor.first,Actor.second);
        if (TargetCell->AllObj.size() > 10)
        {
            std::set<Vec3f> SamePosTest;
            for (auto it = TargetCell->AllObj.begin(); it != TargetCell->AllObj.end(); it++)
                SamePosTest.insert(it->second);
            if (SamePosTest.size()>10)
                expandMyOctree(*LastOctree, CubeIndex);
        }
    }
    //��ʼ���˲���
    MyOctree* initMyOctree(Vec3f Size, Vec3f Center) 
    {
        MyOctree* NewMyOctree = new MyOctree;
        NewMyOctree->OneLevelCube = std::vector<OctreeCell>(OneLevelCubeAllSum);
        NewMyOctree->Size = Size;
        NewMyOctree->Center = Center;
        return NewMyOctree;//����֮ǰ�õ�ʵ����û����ָ�룬������ؽӵ�ʱ��������ݴ��� ������ֱ���õ�ָ��
    }
    //Ѱ��������������������ĵ��Index
    int findOctreeIndex(const Vec3f& Center, const Vec3f& Position)
    {
        char* MyChar = new char[Dimentional];
        for (int i = 0; i < Dimentional; i++)
            MyChar[i] = Position[i] > Center[i] ? '1' : '0';
        std::cout << MyChar << std::endl;
        return strtol(MyChar, &MyChar, 2);
    }
    Vec3f initVec3f(float X, float Y, float Z)
    {
        Vec3f Vec = Vec3f(Dimentional);
        Vec[0] = X; Vec[1] = Y; Vec[2] = Z;
        return Vec;
    }
};

int main()
{
    OctreeInterface Octree;

    Vec3f Size = Octree.initVec3f(100, 100, 100);
    Vec3f Center = Octree.initVec3f(0, 0, 0);
    MyOctree* Root = Octree.initMyOctree(Size, Center);

    Vec3f Position1 = { -1,1,1 };
    Vec3f Position2 = { -5,5,5 };
    Vec3f Position3 = { -10,10,10 };
    Vec3f Position4 = { -15,15,15 };
    Vec3f Position5 = { -20,20,20 };
    Vec3f Position6 = { -25,25,25 };
    Vec3f Position7 = { -30,30,30 };
    Vec3f Position8 = { -35,35,35 };
    Vec3f Position9 = { -40,40,40 };
    Vec3f Position10 = { -45,45,45 };
    Vec3f Position11 = { -50,50,50 };
    Octree.pushActorWithPosition(Root, std::make_pair(111, Position1));
    Octree.pushActorWithPosition(Root, std::make_pair(222, Position2));
    Octree.pushActorWithPosition(Root, std::make_pair(333, Position3));
    Octree.pushActorWithPosition(Root, std::make_pair(444, Position4));
    Octree.pushActorWithPosition(Root, std::make_pair(555, Position5));
    Octree.pushActorWithPosition(Root, std::make_pair(666, Position6));
    Octree.pushActorWithPosition(Root, std::make_pair(777, Position7));
    Octree.pushActorWithPosition(Root, std::make_pair(888, Position8));
    Octree.pushActorWithPosition(Root, std::make_pair(999, Position9));
    Octree.pushActorWithPosition(Root, std::make_pair(901, Position10));
    Octree.pushActorWithPosition(Root, std::make_pair(902, Position11));

    int a = 2;
    //Octree.findOctreeIndex(Center, Position);
    // 
    // 
    //Octree.expandMyOctree(Root,3);
    // 
    // 
    //Vec3f Center = { 0,0,0 };
    //Octree.findOctreeIndex(Center, Position);
    //100 * 100 * 100;
    //Coordinate CenterPoint;
    //X< CenterPoint.X + - (100/2)
}

