�?#include <iostream>

class CustomString
{
public:
	CustomString(const char* inString);
	void operator = (const char* inString);
	friend std::ostream &operator<<(std::ostream &os, CustomString &pt);
	friend std::istream &operator>>(std::istream &os, CustomString &pt);
	int len();
	char* sub(int star, int end);
	void append(const char* inString);
	bool operator == (const CustomString &inString);
	bool find(const char* inString);
	CustomString* split(const char* flag);
private:
	char* str_MyContent;
	int findStringIndexInContent(const char* inString);
};
CustomString::CustomString(const char* inString)
{
	if (inString == nullptr)return;
	int int_Lenth = strlen(inString);
	if (int_Lenth == 0) return;
	str_MyContent = new char[int_Lenth + 1];
	strcpy_s(str_MyContent, int_Lenth + 1, inString);
}
std::istream & operator>>(std::istream & is, CustomString & pt)
{
	return is;
}
std::ostream & operator<<(std::ostream & os, CustomString & pt)
{
	if (&pt == nullptr)
		return os;
	if (pt.str_MyContent != nullptr and *(pt.str_MyContent))
		os << pt.str_MyContent;
	return os;
}
void CustomString::operator=(const char * inString)
{
	CustomString::CustomString(inString);
}
char * CustomString::sub(int star, int end)
{
	if (star >= end || str_MyContent == nullptr||len()< end)return (char*)"\0";
	char* pointer = str_MyContent;
	int index = 0;
	char* str_MyContent = new char[end - star + 1];
	str_MyContent[end - star] = '\0';
	for (int i = 0; i < end; i++)
	{
		if (!*pointer)return nullptr;
		if (i >= star)
		{
			str_MyContent[index] = *pointer;
			index++;
		}
		pointer++;
	}
	return str_MyContent;
}
int CustomString::len()
{
	if (str_MyContent != nullptr)
		return strlen(str_MyContent);
	else
		return 0;
}
void CustomString::append(const char * inString)
{
	if (inString == nullptr || !*inString)return;
	int addStringLength = strlen(inString);
	int selfLength = len();
	char *newString = new char[addStringLength + selfLength + 1];
	newString[addStringLength + selfLength] = '\0';

	char* insertPointer = newString;
	char* getPointer;// = str_MyContent;
	for (int i = 0; i < 2; i++)
	{
		switch (i)
		{
		case 0:
			getPointer = str_MyContent;
			break;
		default:
			getPointer = (char *)inString;
			break;
		}
		while (*getPointer)
		{
			*insertPointer = *getPointer;
			insertPointer++;
			getPointer++;
		}
	}
	str_MyContent = newString;
}
bool CustomString::operator==(const CustomString & inString)
{
	if (&inString == nullptr ||inString.str_MyContent == nullptr || str_MyContent == nullptr)
	{
		if (inString.str_MyContent == nullptr && str_MyContent == nullptr)
			return true;
		return false;
	}
	char* pointer1 = str_MyContent;
	char* pointer2 = inString.str_MyContent;
	while (pointer1 != nullptr && *pointer1 && pointer2 != nullptr && *pointer2)
	{
		if (*pointer1 != *pointer2)return false;
		pointer1++;
		pointer2++;
	}
	if (*pointer1 == *pointer2)
		return true;
	return false;
}
bool CustomString::find(const char * inString)
{
	if (inString == nullptr)return false;
	if (!*inString and *inString == *str_MyContent)return true;
	int result = findStringIndexInContent(inString);
	if (result >= 0)
		return true;
	return false;
}
int CustomString::findStringIndexInContent(const char * inString)
{
	char* selfPointer = str_MyContent;
	char* soursePointer = (char*)inString;
	int index = 0;
	while (selfPointer != nullptr && *selfPointer)
	{
		if (*selfPointer == *soursePointer)
		{
			char* curPosition = selfPointer;
			bool findFlag = false;
			while (*soursePointer and *selfPointer)
			{
				if (*soursePointer != *selfPointer)
					break;
				soursePointer++;
				selfPointer++;
			}
			if (!*soursePointer)
				return index;
			else
			{
				selfPointer = curPosition;
				soursePointer = (char*)inString;
			}
		}
		selfPointer++;
		index++;
	}
	return -1;
}
CustomString * CustomString::split(const char* flag)
{
	if (flag == nullptr || !*flag)
		return nullptr;
	int findPos = findStringIndexInContent(flag);
	if (findPos >=0)
	{
		char* targetString = sub(0, findPos);
		return new CustomString(targetString);
	}
	return nullptr;
}
int main() {
	auto str1 = CustomString("dsadbv");
	CustomString str2 = "gdsgaretebvcaaxdsa";
	std::cout << str1 << std::endl;
	std::cout << str2 << std::endl;
	std::cout << str2.len() << std::endl;
	std::cout << str2.sub(1, 8) << std::endl;
	str1.append("sa");
	std::cout << str1 << std::endl;
	bool equal = str1 == str2;
	if (equal)
		std::cout << "是相同数�?" << std::endl;
	else
		std::cout << "数组不同" << std::endl;
	bool findflag = str2.find("aa");
	if (findflag)
		std::cout << "找到�?" << std::endl;
	else
		std::cout << "没找�?" << std::endl;
	CustomString* ret = str2.split("a");
	std::cout << *ret << std::endl;
}


//auto str1 = CustomString("test1");
//auto str2 = CustomString("test2, test3");
//str1 = "test3";
//int len = str1.len();
//CustomString sub1 = str1.sub(1, 2);
//str1.append("append");
//bool equal = str1 == str2;
//int index = str1.find("es");
//CustomString* ret = str2.split(",");