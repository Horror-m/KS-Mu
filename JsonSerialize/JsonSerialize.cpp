#include <iostream>
//#include <any>
#include <fstream>
#include <string>
#include <any>
#include <map>
#include <stack>
#include <vector>
#include <unordered_set>
using namespace std;

// 由于想让算法的时间复杂度尽可能保证为 n, 所以可能写的比较复杂
enum ContentState
{
	EString,
	EList,
	EMap
};
enum SplitStartMark
{
	KeyChunkStart = '{',
	KeyVectorStart = '['
};

struct Content;
struct MarkStruct;
typedef map<string*, Content*> JsonNodeMap;
typedef	unordered_set<char> charSet;
typedef unordered_set<string> strSet;
typedef const char cChar;

struct Content
{
	ContentState State;
	string* Content;
	vector<string*>* VectorContent = nullptr;
	JsonNodeMap* Child = nullptr;//只有EMap有Child
};

struct MarkStruct
{
	MarkStruct(char FindContent, int FindPos):Content(FindContent), Pos(FindPos){}
	char Content;
	int Pos;
};

class JsomSerialize
{
public:
	vector<JsonNodeMap*> AllJsonMap;

	void reserialize(const string& FilePath)
	{
		ReadPositon = 0;
		string ReadStr = readJsonTable(FilePath);
		for (int BracketPos = findNearestBracket(ReadStr, ReadPositon, FindValidChunkStart); ReadPositon < ReadStr.size() && BracketPos != -1; )
		{
			MarkStruct Mark = MarkStruct(ReadStr[BracketPos], BracketPos);//Push第一个{[
			RangeStack.push(Mark);
			//Push完过第一个括号
			skipReadPosition(ReadStr);

			JsonNodeMap* NewMap = new JsonNodeMap;
			Content* NewContent = new Content;
			NewContent->State = ContentState::EMap;
			string* Keyname = new string("");
			NewMap->insert(make_pair(Keyname, NewContent));
			NewContent->Child = beginSerialize(ReadStr);
			AllJsonMap.push_back(NewMap);
		}
	};
	
private:

	int Deep = 0;
	int ReadPositon = 0;
	 const int MaxMarkSize = 2;
	 stack<MarkStruct> RangeStack;
	 cChar BlockMark = ' ';
	 cChar KeyNameSplitMark = ':';
	 cChar ChunkAnnotationMark = '*';
	 cChar LineAnnotationMark = '/';
	 cChar MiddleSplitMarkEnd = ']';
	 cChar BigSplitMarkEnd = '}';
	 cChar EscapeMark = '\\';
	 cChar StringMark = '\"';

	 //const char KeyChunkStart = '{';
	 //const char KeyVectorStart = '[';
	 //const char MiddleValidChunkMark = '[';
	 //const char BigValidChunkMark = '{';
	 string LineAnnotationEnd = "\n";
	 string LineAnnotationStart = "//";
	 string ChunkAnnotationStart = "/*";
	 string ChunkAnnotationEnd = "*/";

	 charSet FindSplitMark = { ':' };
	 charSet FindSplitMarkEnd = { ']','}', ',' };
	 charSet FindValidChunkEnd = { '}',']' };
	 strSet FindValidChunkStart = { "{","[" };
	 strSet FindValidChunkEndStr = { "}","]" };
	 strSet FindValidChunkSkip = { "{","[" , ",", ":"};
	 strSet FindInvalidBeginChunk = { "//","/*" };

	 //读取Json表
	 string readJsonTable(const string& FilePath)
	 {
		 string Str;
		 ifstream  ReadFile;
		 ReadFile.open(FilePath);
		 string Get;
		 bool ChunkAnnotionStart = false;
		 while (getline(ReadFile, Get))
		 {
			 string CleanLine = cleanReadLine(Get, ChunkAnnotionStart);
			 Str = Str + CleanLine;
		 }
		 cout << Str << endl;
		 return Str;
	 }

	 //清理表里的注释和空格
	 string cleanReadLine(const string& ReadLine, bool& inout_ChunkAnnotionStart)
	 {
		 int StartReadPos = findAnnotionStartPos(ReadLine, inout_ChunkAnnotionStart);//忽略块注释部分
		 string CleanString = "";
		 for (int i = StartReadPos; i < ReadLine.size(); ++i)
		 {
			 if (ReadLine[i] == BlockMark)//忽略空格
				 continue;
			 bool IsPassTheLine = false;//有注释块,是否需要跳整行
			 bool IsPassTheLineBreak = false;//有注释块,是否需要跳整行
			 if (checkMarkListEqual(ReadLine, i, FindInvalidBeginChunk))//注释开始
				 IsPassTheLine = true;
			 int findAnnotionEnd = ReadLine.find(ChunkAnnotationEnd, i + 2);
			 if (IsPassTheLine && findAnnotionEnd != -1)//有注释结束
				 IsPassTheLineBreak = true;
			 if (IsPassTheLine)//有注释
			 {
				 if(ReadLine[i+1] == LineAnnotationMark)//直接忽略line  //这里单独写是因为我觉得双斜杠优先级比块注释高
					 break;
				 //块注释,打开注释标志位
				 if (IsPassTheLineBreak)//块注释有结束
				 {
					 i = findAnnotionEnd + 1;
					 continue;
				 }
				inout_ChunkAnnotionStart = true;
				break;
			 }
			 CleanString += ReadLine[i];
		 }
		 return CleanString/* + LineAnnotationEnd*/;
	 }

	 //找到可以开始读取的地方
	 int findAnnotionStartPos(const string& ReadLine, bool& inout_ChunkAnnotionStart)
	 {
		 if (!inout_ChunkAnnotionStart)
			 return 0;
		 bool PassLine = false;
		 int FindMarkPos = ReadLine.find(ChunkAnnotationEnd);

		 if (FindMarkPos == -1)//  区块注释 整个忽略
			 return ReadLine.size();
		 else
			 inout_ChunkAnnotionStart = false;
		 return FindMarkPos + 2;//走到注释块结束位置
	 }

	 //找到最近的数据块
	 int findNearestBracket(const string& Target, int BeginPos, const unordered_set<string> &MarkList)
	 {
		 for (int i = BeginPos; i < Target.size(); ++i)
		 {
			 if (checkMarkListEqual(Target, i, FindValidChunkStart))
				 return i;
		 }
		 return -1;
	 }

	 //用哈希找到符号
	 bool checkMarkListEqual(const string& Target, int CheckPos, const unordered_set<string>& MarkList)
	 {
		 for (int i = 0; i < MaxMarkSize; ++i)
		 {//之前用的遍历,觉得不好,用哈希了.//最大匹配字符MaxMarkSize = 2, 不费
			 string Find = Target.substr(CheckPos, i + 1);
			 if (MarkList.find(Find) != MarkList.end())
				 return true;
		 }
		 return false;
		 /*for (string CompareString : MarkList)
		 {
			 int FindCount = 0;
			 int FindMarkSize = CompareString.size();
			 for (int i = 0; i < FindMarkSize; i++)
			 {
				 int findPos = CompareString.find(Target[CheckPos + i]);
				 FindCount = findPos >= 0 ? FindCount + 1 : FindCount;
			 }
			 if (FindCount == FindMarkSize) return true;
		 }
		 return false;*/
	 }

	 //直接递归的找map, 其他结构不递归
	 JsonNodeMap* beginSerialize(const string& Target)
	 {
		 ++Deep;
		 JsonNodeMap* RootMap = new JsonNodeMap;
		 bool canBeReturn = false;
		 for (; ReadPositon < Target.size() && RangeStack.size() > 0; )
		 {
			 //char Mark = ;
			 if (BigSplitMarkEnd == Target[ReadPositon])
			 {
				 RangeStack.pop();
				 skipReadPosition(Target,true);
				 break;
			 }
			 Content* VecContent = new Content;
			 string* KeyName = new string;
			 findKeyName(Target, true, KeyName);
			 cout << "---key---->" << *KeyName << endl;
			 skipReadPosition(Target);//跳过符号
			 switch (Target[ReadPositon])
			 {
			 case KeyVectorStart:
				 initVectorStructure(VecContent, KeyName, Target);
				 break;
			 case KeyChunkStart:
				 initMapStructure(VecContent, KeyName, Target);
				 break;
			 default:
				 initStringValue(VecContent, KeyName, Target);
				 break;
			 }
			 RootMap->insert(make_pair(KeyName, VecContent));
			 skipReadPosition(Target);
		 }
		 --Deep;
		 return RootMap;
	 };

	 //跳过符号
	 void skipReadPosition(const string& Target, bool IsEnd = false)
	 {
		 string Get = "_";
		 Get[0] = Target[ReadPositon];
		 auto FindSet = IsEnd ? FindValidChunkEndStr : FindValidChunkSkip;
		 if (FindSet.find(Get) != FindSet.end())
			 ++ReadPositon;
	 }

	 //vector 结构初始化
	 void initVectorStructure(Content* VecContent, string* KeyName, const string& Target)
	 {
		 skipReadPosition(Target);//跳过符号
		 VecContent->State = ContentState::EList;
		 VecContent->VectorContent = findVectorContent(Target);
	 }

	 //map 结构初始化
	 void initMapStructure(Content* VecContent, string* KeyName, const string& Target)
	 {
		 MarkStruct NewMap = MarkStruct(Target[ReadPositon], ReadPositon);
		 skipReadPosition(Target);//跳过符号
		 RangeStack.push(NewMap);
		 VecContent->State = ContentState::EMap;
		 VecContent->Child = beginSerialize(Target);//只有Map递归
	 }
	 
	 //单纯的key 初始化
	 void initStringValue(Content* VecContent, string* KeyName, const string& Target)
	 {
		 VecContent->State = ContentState::EString;
		 VecContent->Content = new string;
		 findKeyName(Target, false, VecContent->Content);
		 cout << "---Value__ " << *VecContent->Content << endl;
	 }
	
	 //把Key名拉过来  返回值只在Vector使用,false为结束Vector的符号']'
	 bool findKeyName(const string& Target, bool IsKey, string* out_KeyName)
	 {
		 unordered_set<char>* FindTarget = IsKey ? &FindSplitMark : &FindSplitMarkEnd;
		 int BeginPos = ReadPositon;
		 int KeyLong = 0;
		 bool StartString = Target[ReadPositon] == StringMark ? true: false;// "xx" 里的符号全部忽略 注意 转义 \\//
		 
		 for (; StartString || (Target.size() && FindTarget->find(Target[ReadPositon]) == FindTarget->end()); ++KeyLong)
		 {
			 ++ReadPositon;
			 if (Target[ReadPositon] == StringMark && (ReadPositon - 1 >= 0 && Target[ReadPositon - 1] != EscapeMark))
				 StartString = false;
		 }
		 *out_KeyName = Target.substr(BeginPos, KeyLong);
		 return Target[ReadPositon] != MiddleSplitMarkEnd;
	 }

	 //转载Vector
	 vector<string*>* findVectorContent(const string& Target)
	 {
		 vector<string*>* Out = new vector<string*>();
		 while (FindValidChunkEnd.find(Target[ReadPositon]) == FindValidChunkEnd.end())
		 {
			 string* OutValue = new string;
			 if ( findKeyName(Target, false, OutValue))
				 ++ReadPositon;
			 Out->push_back(OutValue);
			 cout << "Vector_Value__ " << *OutValue << endl;
		 }
		 skipReadPosition(Target, true);
		 return Out;
	 }
};


int main()
{
	/*any a;
	a = 'A';
	cout << a.type().name() << endl;
	cout <<  a._Cast<char>() << endl;*/
	

	JsomSerialize tool;
	tool.reserialize("E:\\GITCheck\\mumu-dev\\tsconfig.json");
	cout << "--------------------" << endl;
	/*ifstream  ReadFile;
	ReadFile.open("E:\\GITCheck\\mumu-dev\\tsconfig.json");
	string Get;
	while (getline(ReadFile, Get))
	{
		cout << Get << endl;
	}*/
}

