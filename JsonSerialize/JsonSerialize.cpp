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

// ���������㷨��ʱ�临�ӶȾ����ܱ�֤Ϊ n, ���Կ���д�ıȽϸ���
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
	JsonNodeMap* Child = nullptr;//ֻ��EMap��Child
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
			MarkStruct Mark = MarkStruct(ReadStr[BracketPos], BracketPos);//Push��һ��{[
			RangeStack.push(Mark);
			//Push�����һ������
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

	 //��ȡJson��
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

	 //��������ע�ͺͿո�
	 string cleanReadLine(const string& ReadLine, bool& inout_ChunkAnnotionStart)
	 {
		 int StartReadPos = findAnnotionStartPos(ReadLine, inout_ChunkAnnotionStart);//���Կ�ע�Ͳ���
		 string CleanString = "";
		 for (int i = StartReadPos; i < ReadLine.size(); ++i)
		 {
			 if (ReadLine[i] == BlockMark)//���Կո�
				 continue;
			 bool IsPassTheLine = false;//��ע�Ϳ�,�Ƿ���Ҫ������
			 bool IsPassTheLineBreak = false;//��ע�Ϳ�,�Ƿ���Ҫ������
			 if (checkMarkListEqual(ReadLine, i, FindInvalidBeginChunk))//ע�Ϳ�ʼ
				 IsPassTheLine = true;
			 int findAnnotionEnd = ReadLine.find(ChunkAnnotationEnd, i + 2);
			 if (IsPassTheLine && findAnnotionEnd != -1)//��ע�ͽ���
				 IsPassTheLineBreak = true;
			 if (IsPassTheLine)//��ע��
			 {
				 if(ReadLine[i+1] == LineAnnotationMark)//ֱ�Ӻ���line  //���ﵥ��д����Ϊ�Ҿ���˫б�����ȼ��ȿ�ע�͸�
					 break;
				 //��ע��,��ע�ͱ�־λ
				 if (IsPassTheLineBreak)//��ע���н���
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

	 //�ҵ����Կ�ʼ��ȡ�ĵط�
	 int findAnnotionStartPos(const string& ReadLine, bool& inout_ChunkAnnotionStart)
	 {
		 if (!inout_ChunkAnnotionStart)
			 return 0;
		 bool PassLine = false;
		 int FindMarkPos = ReadLine.find(ChunkAnnotationEnd);

		 if (FindMarkPos == -1)//  ����ע�� ��������
			 return ReadLine.size();
		 else
			 inout_ChunkAnnotionStart = false;
		 return FindMarkPos + 2;//�ߵ�ע�Ϳ����λ��
	 }

	 //�ҵ���������ݿ�
	 int findNearestBracket(const string& Target, int BeginPos, const unordered_set<string> &MarkList)
	 {
		 for (int i = BeginPos; i < Target.size(); ++i)
		 {
			 if (checkMarkListEqual(Target, i, FindValidChunkStart))
				 return i;
		 }
		 return -1;
	 }

	 //�ù�ϣ�ҵ�����
	 bool checkMarkListEqual(const string& Target, int CheckPos, const unordered_set<string>& MarkList)
	 {
		 for (int i = 0; i < MaxMarkSize; ++i)
		 {//֮ǰ�õı���,���ò���,�ù�ϣ��.//���ƥ���ַ�MaxMarkSize = 2, ����
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

	 //ֱ�ӵݹ����map, �����ṹ���ݹ�
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
			 skipReadPosition(Target);//��������
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

	 //��������
	 void skipReadPosition(const string& Target, bool IsEnd = false)
	 {
		 string Get = "_";
		 Get[0] = Target[ReadPositon];
		 auto FindSet = IsEnd ? FindValidChunkEndStr : FindValidChunkSkip;
		 if (FindSet.find(Get) != FindSet.end())
			 ++ReadPositon;
	 }

	 //vector �ṹ��ʼ��
	 void initVectorStructure(Content* VecContent, string* KeyName, const string& Target)
	 {
		 skipReadPosition(Target);//��������
		 VecContent->State = ContentState::EList;
		 VecContent->VectorContent = findVectorContent(Target);
	 }

	 //map �ṹ��ʼ��
	 void initMapStructure(Content* VecContent, string* KeyName, const string& Target)
	 {
		 MarkStruct NewMap = MarkStruct(Target[ReadPositon], ReadPositon);
		 skipReadPosition(Target);//��������
		 RangeStack.push(NewMap);
		 VecContent->State = ContentState::EMap;
		 VecContent->Child = beginSerialize(Target);//ֻ��Map�ݹ�
	 }
	 
	 //������key ��ʼ��
	 void initStringValue(Content* VecContent, string* KeyName, const string& Target)
	 {
		 VecContent->State = ContentState::EString;
		 VecContent->Content = new string;
		 findKeyName(Target, false, VecContent->Content);
		 cout << "---Value__ " << *VecContent->Content << endl;
	 }
	
	 //��Key��������  ����ֵֻ��Vectorʹ��,falseΪ����Vector�ķ���']'
	 bool findKeyName(const string& Target, bool IsKey, string* out_KeyName)
	 {
		 unordered_set<char>* FindTarget = IsKey ? &FindSplitMark : &FindSplitMarkEnd;
		 int BeginPos = ReadPositon;
		 int KeyLong = 0;
		 bool StartString = Target[ReadPositon] == StringMark ? true: false;// "xx" ��ķ���ȫ������ ע�� ת�� \\//
		 
		 for (; StartString || (Target.size() && FindTarget->find(Target[ReadPositon]) == FindTarget->end()); ++KeyLong)
		 {
			 ++ReadPositon;
			 if (Target[ReadPositon] == StringMark && (ReadPositon - 1 >= 0 && Target[ReadPositon - 1] != EscapeMark))
				 StartString = false;
		 }
		 *out_KeyName = Target.substr(BeginPos, KeyLong);
		 return Target[ReadPositon] != MiddleSplitMarkEnd;
	 }

	 //ת��Vector
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

