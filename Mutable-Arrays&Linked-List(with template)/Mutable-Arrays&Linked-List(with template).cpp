#include <iostream>
#include <mutex>
#include <thread>

template<typename T>
class CustomArray
{
    T* coustomArray = nullptr;
    unsigned int arrySize = 0;
    unsigned int memberSize = 0;
public:
    CustomArray():arrySize(0), memberSize(0), coustomArray(nullptr)
    {
        std::cout << "初始化" << std::endl;
    }
    void reserve(const unsigned int& size)
    {
        arrySize = size;
        coustomArray = (T*)std::malloc(sizeof(T) * size);
        //coustomArray = new T()
        for (unsigned int i = 0; i < size; i++)
            coustomArray[i] = 0;
    }
    void push(const T& value)
    {
        if (arrySize <= memberSize)
            return;
        coustomArray[memberSize] = value;
        std::cout << "push操作" << coustomArray[memberSize] << std::endl;
        ++memberSize;
    }
    void insert(const int& index, const T& value)
    {
        if (arrySize < memberSize + 1 || index > memberSize)
        {
            std::cout << "无法插入" << std::endl;
            return;
        }
        T insertValue = value;
        T target;// = coustomArray[index];
        for (int i = index; i < arrySize; i++)
        {
            target = coustomArray[index];
            coustomArray[index] = insertValue;
            insertValue = target;
        }
        printAll();
        ++memberSize;
    }
    void remove(const int& index)
    {
        if (index >= memberSize)
        {
            std::cout << "无法删除" << std::endl;
            return;
        }
        for (int i = index; i < memberSize - 1; i++)
        {
            coustomArray[i] = coustomArray[i+1];
        }
        --memberSize;
        printAll();
    }
    void pop()
    {
        if (memberSize == 0)
        {
            std::cout << "无法Pop" << std::endl;
            return;
        }
        remove(memberSize - 1);
        printAll();
    }
    void clear()
    {
        free(coustomArray);
        arrySize = 0;
        memberSize = 0;
        printAll();
    }
    int findIndex(const T& value)
    {
        for (int i = 0; i < memberSize; i++)
        {
            if (coustomArray[i] == value)
            {
                return i;
            }
        }
        return -1;
    }

    void printAll()
    {
        if (memberSize == 0)
            std::cout << "数组为空" << std::endl;
        for (int i = 0; i < memberSize; i++)
        {
            std::cout << coustomArray[i];
        }
        std::cout << std::endl;
    }
};

template <typename T> 
struct Node
{
    T value;
    Node* front;
    Node* next;
};
template <typename T>
class CustomList
{
    Node<T>* headNode = nullptr;
    Node<T>* endNode = nullptr;
    int count = 0;
public:
    CustomList() :count(0), headNode(nullptr), endNode(nullptr)
    {
        std::cout << "链表初始化" << std::endl;
    }
    Node<T>* push(T value) 
    {
        Node<T>* newNode = (Node<T>*)malloc(sizeof(Node<T>));
        newNode->value = value;
        if (Node<T>* head = nullptr)
            headNode = newNode;
        insert(newNode, count);
        return newNode;
    }
    Node<T>* insert(Node<T>* newNode, int insertPos)
    {
        if (checkNodeExist(newNode) != nullptr || insertPos > count || newNode == nullptr)
            return nullptr;
        newNode->front = nullptr;
        newNode->next = nullptr;
        ++count;
        if (headNode == nullptr)
        {
            headNode = newNode;
            endNode = newNode;
            newNode->front = nullptr;
            newNode->next = nullptr;
            return newNode;
        }
        else if (insertPos == 0)
        {
            newNode->next = headNode;
            headNode->front = newNode;
            headNode = newNode;
            return newNode;
        }
        else if (insertPos == count - 1)
        {
            newNode->front = endNode;
            endNode->next = newNode;
            endNode = newNode;
            return newNode;
        }

        Node<T>* findNode = headNode;
        int curPos = 0;
        Node<T>* curFrontNode = nullptr;
        while (findNode != nullptr)
        {
            if (insertPos == curPos)
            {
                newNode->front = curFrontNode;
                newNode->next = findNode;
                if (curFrontNode != nullptr)
                    curFrontNode->next = newNode;
                if (findNode != nullptr)
                    findNode->front = newNode;
                break;
            }
            curFrontNode = findNode;
            findNode = findNode->next;
            ++curPos;
        }
        return newNode;
    }
    Node<T>* find(int findPos)
    {
        if (findPos > count)
            return nullptr;
        Node<T>* findNode = headNode;
        int pos = 0;
        while (findNode != nullptr)
        {
            if (pos == findPos)
                return findNode;
            findNode = findNode->next;
            ++pos;
        }
    }
    bool remove(Node<T>* removeNode)
    {
        Node<T>* find = checkNodeExist(removeNode);
        if (find == nullptr)
            return false;
        if (find == headNode)
            headNode = find->next;
        if (find == endNode)
            endNode = find->front;
        if (find->front != nullptr)
            find->front->next = find->next;
        if (find->next != nullptr)
            find->next->front = find->front;
        free(find);
        return true;
    }
    void popAll()
    {
        Node<T>* findNode = nullptr; headNode;
        while (headNode != nullptr)
        {
            findNode = headNode;
            headNode = headNode->next;
            free(findNode);
        }
    }
    Node<T>* checkNodeExist(Node<T>* findCheckNode)
    {
        Node<T>* findNode = headNode;
        while (findNode != nullptr)
        {
            if (findNode == findCheckNode)
                return findNode;
            findNode = findNode->next;
        }
        return nullptr;
    }
    void printAllNode()
    {
        Node<T>* findNode = headNode;
        while (findNode != nullptr)
        {
            std::cout << findNode->value << std::endl;
            findNode = findNode->next;
        }
        std::cout << "------------------------" << std::endl;
    }
};



int main() {
    /*实现数组以及一下api*/
    auto arr = CustomArray<int>();// = new CustomArray<int>();
    arr.reserve(6);
    arr.push(1);
    arr.push(2);
    arr.push(3);
    arr.insert(3, 4);
    arr.remove(1);
    arr.pop();
    //arr.clear();
    int index = arr.findIndex(3);
    std::cout << index << std::endl;

    //实现双向链表以下api
    auto list1 = CustomList<int>();
    auto node1 = list1.push(1);
    list1.printAllNode();
    auto node2 = list1.push(2);
    list1.printAllNode();
    auto node3 = list1.insert(node2, 3);
    list1.printAllNode();
    auto nodeDef1 = new Node<int>();
    nodeDef1->value = 9999;
    auto node4 = list1.insert(nodeDef1, 0);
    list1.printAllNode();

    auto nodeDef2 = new Node<int>();
    nodeDef2->value = 7777;
    list1.insert(nodeDef2, 0);
    list1.printAllNode();

    auto ret = list1.find(2);
    std::cout << ret->value << std::endl;
    std::cout << "------------------------" << std::endl;
    list1.remove(nodeDef2);
    list1.printAllNode();
    list1.popAll();
    list1.printAllNode();

    /*做完上面的再改成带模板的*/
}