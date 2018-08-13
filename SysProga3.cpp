// SysProga3.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <regex>
#include <iterator>
#include <map>
#include<vector>
#include<fstream> 
#include<algorithm>
#include<locale>

using namespace std;

void deleteSubstrFromText(string substr, string& text){
	bool btext=true;
	while(btext){
	std::string::size_type index = text.find(substr);
	if (index != std::string::npos) text.erase(index, substr.size());
	else btext=false;}
}
string readFile(string s)
{
	ifstream fin;
	fin.open(s);
	string text=" ";
	
	
	bool check = false;
	if(fin.is_open())
	{
		while(!fin.eof())
		{
			string temp;
			fin>>temp;
			if(temp[0]=='{') 
			{if((temp.size()>1 && temp[1]!='*') || temp.size()==1 ) {
																		check=true;
																	}
			}
			
			if(check){
				text+=temp;
				string str;
				getline(fin, str);
				cout<<str<<endl;
				check=false;
				text+= str+"<>  ";
			}		
			else text+=temp+"  ";
			
		}
	}
	else(std::cout<<"File was not opened.\n");
	fin.close();
	return text;
}

regex readReservedWords(string s)
{
	ifstream fin;
	fin.open(s);
	string text="";
	
	if(fin.is_open())
	{
		while(!fin.eof())
		{
			string temp;
			fin>>temp;
			text+="([\\s\\(\\;\\)]"+ temp+ "[\\s \\. \\( \\) \\;])|";// big and small
		}
	}
	else(cout<<"File was not opened.\n");
	fin.close();
	text.pop_back();
	regex reg(text);
	return (reg);
}

void lexAnalyzer(regex re, string& text,  vector<string> types)
{
	string newText=text;
	auto words_begin = std::sregex_iterator(text.begin(), text.end(), re);
	
	
    
	auto words_end= std::sregex_iterator();
	auto tempRes = words_begin;

	
	std::cout<<endl;

	while (tempRes!=words_end) {
		for( auto i= tempRes->size()-1; i>0; i--){
		auto temp=tempRes->str(i);		
		 
	    if(!temp.empty()) {						
								std::string::size_type index = newText.find(temp);
								if(types[0]!="COMMENTS2") 
									std::cout<<temp<<"\t"<<types[i-1]<<endl;
								else 
									std::cout<<temp.substr(0,temp.size()-2)<<"\t"<<types[i-1]<<endl;
								if (index != std::string::npos) newText.erase(index, temp.length()); 
						}
		}
    ++tempRes;	
}
	text=newText;

}

void lexAnalyzerReserved(regex re, string& text)
{
	string newText=text;
	string lowerCase = text; string newLowCase;	
	std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), tolower); 

	auto words_begin_lcase = std::sregex_iterator(lowerCase.begin(), lowerCase.end(), re);   
	auto words_end= std::sregex_iterator();	

	auto tempRes = words_begin_lcase; newLowCase = lowerCase;
	std::cout<<endl;

	while (tempRes!=words_end) {
		for( auto i= tempRes->size()-1; i>0; i--){
		auto temp=tempRes->str(i);		
		 
	    if(!temp.empty()) { std::string::size_type index = newLowCase.find(temp);
							std::cout<<newText.substr(index+1, temp.size()-2)<<"\t"<<"RESERVED WORDS"<<endl; //lower case
							if (index != std::string::npos) {newText.erase(index+1, temp.length()-2);
															newLowCase.erase(index+1, temp.length()-2);}

							}
		}
    ++tempRes;	
}
	text=newText;
}

string makeStringFromMap(map<string,string> a, vector<string>& types)
{
	std::string reg;
    for(auto const& x : a){
        reg += "(" + x.first + ")|"; // parenthesize the submatches
		types.push_back(x.second);
	}

    reg.pop_back();
	return reg;
}
void checkUnknownSymbols(string& text){

	deleteSubstrFromText(" ",text);	
	if(text.size()!=0){ cout<<"\nThere are mistakes:"<<endl;
		cout<<"Length = "<<text.size()<<" NewText: "<<text<<endl;}
}

bool checkDyckWord(string expr)
{
	int sum1=0;
	int sum2=0;
	int sum3=0;
	int i=0;
	while(((sum1>=0)&&(sum2>=0)&&(sum3>=0))&& (i<expr.size())){

		switch(expr[i]){
		case '(': {sum1++; break;}
		case '[':{sum2++; break;}
		case '{' :{sum3++; break;}
		case ')': {sum1--; break;}
		case ']':{sum2--; break;}
		case '}' :{sum3--; break;} 
		}

		i++;
	}
	return( (i==expr.size()) && (sum1+sum2+sum3==0));
}

void autoComment(string& text){
	int q=0;
	string newText = text;
	bool beginComment=false;
	bool print = false;
	int indexL=std::string::npos;
	int indexR=std::string::npos;

	for( int i = 0; i<text.size(); i++)

	{
		switch(q)
		{
		case 0:
			{
				if (text[i]!='(') {break;}
				else {q=1; ; break;}
			}
		case 1:
			{
				if (text[i]=='*') {q=2; beginComment=true; break;}
				else {q=0; break;} 
				break;
			}
		case 2:
			{
				beginComment = false;
				if (text[i]!='*') { break;}
				else {q=3; break;}
			}
		case 3:
			{
				if (text[i]!=')') { q=2 ;break;}
				else {q=4; break;}
			}
		
		case 4:
			{
				if (text[i]!='(') {q=0; break;}
				else {q=1; break;}
			}
		}
		if(q==2 && beginComment) { //cout<<"index Begin ="<<i-1<<endl; 
			indexL = i-1; print=false;}
		if(q==4) {//cout<<"index End = "<< i<<endl<<endl; 
			indexR = i; print=false;}
		if( indexL< indexR && (!print) ){ 
											std::cout<<text.substr(indexL, indexR -indexL+1)<<"\t"<<"COMMENTS"<<endl;
											print = true;									
											text.erase(indexL, indexR -indexL+1);
											i=indexL-1;
											indexL=std::string::npos;
											indexR=std::string::npos;
											
		}
				
	}
}

void funcReserved(string& text, string& lowerCase, string substr){
				bool btext=true;
				while(btext){
					std::string::size_type index = lowerCase.find(substr);				
					
					if (index != std::string::npos) { 
													std::cout<<text.substr(index+1, substr.size()-2)<<"\t"<<"RESERVED WORDS"<<endl;
													text.erase(index+1, substr.size()-2);
													lowerCase.erase(index+1, substr.size()-2);
													}
													
					else btext=false;
							}
}

void lexAnalyzerReserved2(string& text, string fileName)
{
	ifstream fin;
	fin.open(fileName);
    string lowerCase = text;
	std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), tolower); 
	

	if(fin.is_open())
	{
		while(!fin.eof())
		{
			string temp;
			fin>>temp;
			string temp2=" "+temp+" ";
			string temp3="("+temp+" ";
			string temp4=" "+temp+";";
			string temp5=" "+temp+".";
			string temp6=" "+temp+")";
			funcReserved(text,lowerCase, temp2);
			funcReserved(text,lowerCase, temp3);
			funcReserved(text,lowerCase, temp4);
			funcReserved(text,lowerCase, temp5);
			funcReserved(text,lowerCase, temp6);
		}
	}
	else(cout<<"File was not opened.\n");
	fin.close();
}

int _tmain(int argc, _TCHAR* argv[])
{

	string text = readFile("input3.txt");
	//cout<<text<<endl;

	map<string,string> v,commentsLoc,separators,direct;

	//директиви препроцесора, константи(ч і р), коментарі, зарезервовані слова, оператори, розділові знаки, ідентифікатори
	v.insert(pair<string,string> ("[1-9][0-9]*" ,  "NUMBER CONSTANTS"));//
    v.insert(pair<string,string> ("[0-9]+[\\.][0-9]+" ,  "NUMBER CONSTANTS"));
	v.insert(pair<string,string> ("[a-zA-Z_][a-zA-Z_0-9]*" ,  "IDENTIFIERS"));
	v.insert(pair<string,string> ("\'[a-zA-Z0-9\\s\\t \\- \\._ = \\+ \\* \\| \\\\ / ><:; ~ ! @ # \\$ % ^ &\\[\\]\\{\\(\\)\\} ]*\'" ,  "CHAR CONSTANTS"));
	
	v.insert(pair<string,string> ("[\\+\\*/\\-;=><:]","OPERATORS"));
	v.insert(pair<string,string> ("[<>:]=","OPERATORS"));
	v.insert(pair<string,string> ("<>","OPERATORS"));
	v.insert(pair<string,string> ("><","OPERATORS"));
	v.insert(pair<string,string> ("\\*\\*","OPERATORS"));
		
	commentsLoc.insert(pair<string,string> ("\\{[a-zA-Z0-9\\s\\t\\n \\. \\- _ = \\+ \\* \\| \\\\ / ><:; ~ ! @ # \\$ % , ^ & \\[ \\] \\{ \\) \\( ]*\\}[\\s]*<>","COMMENTS2"));//Local end on 1 line
	direct.insert(pair<string,string>("\\$[A-Z]+","DIRECTIVES"));
	separators.insert(pair<string,string>("[\\(\\)\\[\\]\\{\\},\\.]","SEPARATORS"));

	vector<string> types;
	vector<string> typesGen;
	vector<string> typesLoc;
	vector<string> typesSepar;
	vector<string> typesDirect;

    string strAll= makeStringFromMap(v,types);
	string strCommentsLoc = makeStringFromMap(commentsLoc,typesLoc);
	string strSep = makeStringFromMap(separators,typesSepar);
	string strDirect = makeStringFromMap(direct,typesDirect);

	
	string newText = text;
	

     autoComment(newText);
	 //cout<<newText<<endl;


	
	regex regCommentsLoc(strCommentsLoc);
	lexAnalyzer(regCommentsLoc,newText,typesLoc);

	//std::cout<< "\n\nnewText:\n"<<newText;

	//
	//regex regReserved = readReservedWords("reservedWords.txt");
	//lexAnalyzerReserved(regReserved, newText);
	lexAnalyzerReserved2(newText,"reservedWords.txt");

	//cout<< "\n\nnewText:\n"<<newText<<endl;

   
	regex regDirect(strDirect);
	lexAnalyzer(regDirect,newText,typesDirect);
	
	regex regAll(strAll);
	lexAnalyzer(regAll, newText, types);

	cout<<"Words Dika NewText: "<<newText<<endl;
	cout<<"Dyck's words check:"<<checkDyckWord(newText)<<endl;

	regex regSep(strSep);
	lexAnalyzer(regSep, newText, typesSepar);
	checkUnknownSymbols(newText);
	
	
	system("pause");
	return 0;
}

