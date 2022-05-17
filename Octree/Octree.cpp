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
#define ActorInfo std::pair<int ,Vec3f> //物体信息
//图在文档下  图在文档下  图在文档下    图在文档下  图在文档下  图在文档下  图在文档下  图在文档下  图在文档下  图在文档下  图在文档下!!!!!!!!!!!!!!!!!!!
//然后正前面有个正对着你的二阶魔方, 0就是左下前,  1是左上前, 2是左下后, 3是左上后, 右边对称+4
//自己制定规则 区块里的物体多于 10,并且不重复的坐标大于10， 深度+1

static const int OneLevelCubeAllSum = 8;
static const int Dimentional = 3;
struct MyOctree;//观察者 我发现class一样不报错 和struct相同，可能原因是因为两个是一种东西的所以兼容吧，之后测试

/// <summary>
/// 树每个叶的结构
/// std::unordered_map<int, Vec3f> AllObj
/// MyOctree* NextOctree
/// </summary>
struct OctreeCell
{
    std::unordered_map<int, Vec3f> AllObj;//第一个参数int 假设是actorID，
    MyOctree* NextOctree = nullptr;
};

/// <summary>
/// 树结构
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
    //树转二进制
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
    //扩展树
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
        //&Temp;//initMyOctree(NextLevelSize, NextLevelCenter); //这有很奇怪的地方 以后要指针 就返回指针，否则生命周期可能就有问题
        CurCell->NextOctree = initMyOctree(NextLevelSize, NextLevelCenter);

        for (auto it = CurCell->AllObj.begin(); it != CurCell->AllObj.end(); it++)
            pushActorWithPosition(&ExpandOctree, *it);
        CurCell->AllObj.clear();
    }
public:
    //插入数据
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
    //初始化八叉树
    MyOctree* initMyOctree(Vec3f Size, Vec3f Center) 
    {
        MyOctree* NewMyOctree = new MyOctree;
        NewMyOctree->OneLevelCube = std::vector<OctreeCell>(OneLevelCubeAllSum);
        NewMyOctree->Size = Size;
        NewMyOctree->Center = Center;
        return NewMyOctree;//这里之前用的实例，没返回指针，结果返回接的时候各种数据错误 ，所以直接用的指针
    }
    //寻找物体坐标相对于树中心点的Index
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

