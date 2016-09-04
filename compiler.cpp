#include<iostream>
#include<fstream>
#include<string>
#include<cctype>
#include<vector>
#include<stack>
#include<stdlib.h>

#define CONST_DIGIT       3      //integer
#define CONST_CHAR        4      //character
#define CONST_OPERATOR    8      //operator
#define CONST_IDENTIFIER  9      //identifier
#define CONST_BRACKET     10     //brackets
#define CONST_STRING      5      //string
#define CONST_TYPE        6      //type
#define CONST_KEYWORDS    7      //keywords


using namespace std;


typedef struct WordID_pair
{
	int       id;     //identifier
	string    word;   //word
}WordID_pair;

typedef struct Variable
{
	string   var;    //variable 
	string   value;  //initial val
}Variable;

//target
typedef struct Target
{
	string    dest;    //destination
	string    dsrc;    //source
	string    mark;   //token
	string    step;   //jump position
	string    dsf;    //results
	string    op;     //operation
}Target;

//vector for var declarations
vector<Variable>      var_table;
//vector for targets
vector<Target>        target_code;


char              lab='A'; 
char              vab='A'; 

//assembly file name
string  asmfile(string source)
{
	if(source.size()==0)
	{
		cout<<"File Name Cannot be Empty!"<<endl;
		exit(-1);
	}
	string tempStr="";
	int i,j;
	j = source.size();
    for(i = j-1;i>=0;i--)
	{

		if(source[i] == '.')
		{
			j = i;
			break;
		}
	}
	tempStr = source.substr(0,j) + ".asm";
	return tempStr;
}

//precedence of operators
int level(string s)
{
	if(s=="#")
		return 1;
	else if(s=="=")
		return 2;
	else if(s=="+" || s=="-")
		return 3;
	else if(s=="*" || s=="/")
		return 4;
	else
		return -1;
}


//save to target code
void add_target_code(string dsf,string op,string dest,string dsrc,string mark,string step)
{
	Target  tmp;
	tmp.dsf = dsf;
	tmp.op = op;
	tmp.dest = dest;
	tmp.dsrc = dsrc;
	tmp.mark = mark;
	tmp.step = step;
	target_code.push_back(tmp);
}

//Character to string
string char_to_str(char c)
{
	char s[2] = " ";
	s[0] = c;
	return string(s);
}

//check if operators
int is_operator(char c)
{
	if(c == '+' || c=='-'||c=='*'||c=='/'||c==','||c=='=' ||c=='>' || c=='<')
		return 1;
	else
		return 0;
}

//if brackets
int is_bracket(char c)
{
	if(c=='{' || c=='}' || c=='(' || c==')' ||c==';')
		return 1;
	else
		return 0;
}

//check blank
int is_blank(char c)
{
	if(c=='\n' || c=='\t' || c==' ' || c=='\r')
		return 1;
	else
		return 0;
}

//determine type of token
int word_token(string s)
{
	int    size = s.size();
	
	if(s[0]=='\'')
	{
		if(s[size-1] == '\'')
			return CONST_CHAR;
		else
		{
			cout<<"Wrong string of tokens： "<<s<<endl;
			exit(-1);
		}
	}
	
	else if(s[0]=='\"')
	{
		if(s[size-1]=='\"')
			return CONST_STRING;
		else
		{
            cout<<"Wrong string of tokens："<<s<<endl;
			exit(-1);
		}
	}
	//integer
	else if(isdigit(s[0]))
	{
		for(int i=1;i<size;i++)
		{
			if(!isdigit(s[i]))
			{
				cout<<"Illegal character ："<<s<<endl;
				exit(-1);
			}
		}
		return CONST_DIGIT;
	}
	else
	{
		for(int i=0;i<size;i++)
		{
			if(!isalnum(s[i]) && s[i]!='_')
			{
                cout<<"illegal character："<<s<<endl;
				exit(-1);
			}
		}
		//data type
		if(s=="int" || s=="char")
			return CONST_TYPE;
		//keyword
		else if(s=="if" || s=="else" || s=="printf" || s=="main")
			return CONST_KEYWORDS;
		//user-defined identifier
		else
			return CONST_IDENTIFIER;
	}
}

//add keywords
void keywords_addition(vector<WordID_pair> &v,int id,string word)
{
	WordID_pair    temp;
	temp.id = id;
	temp.word = word;
	v.push_back(temp);
}

//Lexical analysis
void lexicalAnalysis(string source,vector<WordID_pair> &AnalyzedData)
{
	char       ch;
	ifstream   rfile(source.c_str());
	if(!rfile.is_open())
	{
		cout<<"Cannot Open Source File!"<<endl;
		exit(-1);
	}

	rfile>>noskipws;   //Not skipping whitespace
	while(rfile>>ch)
	{
		int         state=0;        //Determine states
		string      temp("");       //Token buffer
        char        try_ch;         //Token in the front
		
		switch(state)
		{
		case 0:
			if(ch=='/') // Possibility of start of comment
			{
				rfile>>try_ch;
				if(try_ch=='/')
				{
					while(rfile>>try_ch)
					{
						if(try_ch=='\n')
							break;   //This is a comment
					}
					break;
				}
				else if(try_ch=='*')
				{
					while(rfile>>try_ch)
					{
						if(try_ch=='*')
						{
							rfile>>try_ch;
							if(try_ch=='/')
								break; //Multiple comments
						}
					}
					break;
				}
				else
				{
					keywords_addition(AnalyzedData,CONST_OPERATOR,char_to_str(ch));
					ch = try_ch;  //State 1
				}
			}
		case 1:
		    if(is_operator(ch)) //Determine operator
			{
				keywords_addition(AnalyzedData,CONST_OPERATOR,char_to_str(ch));
				break;
			}
		case 2:
		    if(is_bracket(ch)) 
			{
				keywords_addition(AnalyzedData,CONST_BRACKET,char_to_str(ch));
				break;
			}
		case 3:
			if(is_blank(ch)) 
				break;
		case 4:
			if(ch=='#') //skip preprocessor
			{
				while(rfile>>ch)
				{
					if(is_blank(ch))
					{
						break;
					}
				}
				break;
			}
		default:// determine keyword type
			temp = temp + char_to_str(ch);
			while(rfile>>try_ch)
			{
				if(try_ch == '\"')
				{
					temp = temp + char_to_str(try_ch);
					if(ch == '\"')
					{
						keywords_addition(AnalyzedData,word_token(temp),temp);
						break;
					}
					else
					{
						cout<<"Illegal token："+temp<<endl;
						exit(-1);
					}
				}
				else if(is_blank(try_ch) )
				{
					if(ch != '\'' && ch != '\"')
					{
						keywords_addition(AnalyzedData,word_token(temp),temp);
						break;
					}
					else
						temp = temp + char_to_str(try_ch);
				}
				else if(is_operator(try_ch) )
				{
					if(ch !='\'' && ch != '\"' )
					{
						keywords_addition(AnalyzedData,word_token(temp),temp);
					    keywords_addition(AnalyzedData,CONST_OPERATOR,char_to_str(try_ch));
						break;
					}
					else
						temp = temp + char_to_str(try_ch);
				}
				else if(is_bracket(try_ch))
				{
					keywords_addition(AnalyzedData,word_token(temp),temp);
					keywords_addition(AnalyzedData,CONST_BRACKET,char_to_str(try_ch));
					break;
				}
				else
					temp = temp + char_to_str(try_ch);
			}
		}
	}
	rfile.close();
}

//print lexical analysis results
void lex_print(vector<WordID_pair> &v)
{
	vector<WordID_pair>::iterator  it;
	for(it = v.begin();it != v.end();it++)
		cout<<it->id<<" "<<it->word<<endl;
}

//obtain var declarations
void varTable_addition(vector<WordID_pair>::iterator &it)
{
	while(it->id == CONST_TYPE)
	{
		it++;
		while(it->word != ";")
		{
	
			if(it->id == CONST_IDENTIFIER)
			{
				Variable     tmp;
				tmp.var = it->word;
				string   tmp_var = it->word;
				if((it+1)->word=="=")   //see if var is initialized
				{
					it = it+2;
					tmp.value = it->word;
					add_target_code(tmp_var,"=",tmp.value," "," "," ");
				}
				var_table.push_back(tmp);
			}
			it++;
		}
		it++;
	}
}

//expression analysis
void expression(vector<WordID_pair>::iterator &it)
{
	string dsf,op,dest,dsrc;         
	
    stack<string>         word_stack;
    
    stack<string>         oper_stack;
	oper_stack.push("#");
	while(it->word != ";")       //statement ends at ";"
	{
	
		if(it->word == "(")
			oper_stack.push(it->word);
		else if(it->word == ")")
		{

			while(oper_stack.top() != "(")
			{
				op = oper_stack.top();
			
			    oper_stack.pop();
		
			    dsrc = word_stack.top();
                word_stack.pop();
			    dest = word_stack.top();
			    word_stack.pop();
			    vab = vab+1;
				if(vab == 91)
					vab = '0';
			    dsf = "tmp" + char_to_str(vab);

				Variable     tmp;
				tmp.var = dsf;
				var_table.push_back(tmp);

			    word_stack.push(dsf);
                add_target_code(dsf,op,dest,dsrc," "," ");
			}
			oper_stack.pop();
	
		}
		else if(it->id != CONST_OPERATOR)
			word_stack.push(it->word);
		else if(oper_stack.top() == "(")
		{
			oper_stack.push(it->word);
		}
		else if(level(it->word) < level(oper_stack.top()))  //low precedence
		{
			op = oper_stack.top();
			oper_stack.pop();
			oper_stack.push(it->word);
			dsrc = word_stack.top();
            word_stack.pop();
			dest = word_stack.top();
			word_stack.pop();
			vab = vab+1;
			if(vab == 91)
				vab = '0';
			dsf = "tmp" + char_to_str(vab);

            Variable     tmp;
			tmp.var = dsf;
			var_table.push_back(tmp);

			word_stack.push(dsf);
            add_target_code(dsf,op,dest,dsrc," "," ");
		}
		else   
			oper_stack.push(it->word);
		it++;
	}
	//pop rest of stack
	while(oper_stack.top() != "#")
	{
		op = oper_stack.top();
        oper_stack.pop();
		dsrc = word_stack.top();
        word_stack.pop();
		dest = word_stack.top();
		word_stack.pop();
	
		if(op=="=")//Assignment
		{
			add_target_code(dest,op,dsrc," "," "," ");
		}
		else
		{
            vab = vab+1;
			if(vab == 91)
				vab = '0';
			dsf = "tmp" + char_to_str(vab);

            Variable     tmp;
			tmp.var = dsf;
			var_table.push_back(tmp);

			word_stack.push(dsf);
            add_target_code(dsf,op,dest,dsrc," "," ");
		}
	}
}

//analyzing print
void analysis_print(vector<WordID_pair>::iterator &it)
{
	int j,i=1;
	it = it+2;
	string str = it->word; 
	string strvar;         

	Variable       tmp;
	
	for(j=1;j<str.size()-1;)
	{
		if(str[j]=='%')
        {
			if(i != j)
			{
				vab = vab + 1;
				if(vab == 91)
					vab = '0';
			    add_target_code("\'"+str.substr(i,j-i)+"$\'","p"," "," ","tmp"+char_to_str(vab)," ");
			    tmp.var = "tmp"+char_to_str(vab);
			    tmp.value = "\'"+str.substr(i,j-i)+"$\'";
			    var_table.push_back(tmp);
            }

			i = j+2;
			it = it+2;  
			strvar = it->word;
            add_target_code(strvar,"p"," "," ",str.substr(j,2)," ");
			j = i;
			continue;
		}
		j++;
	}
	if(i!=j)
	{
		vab = vab+1;
		if(vab == 91)
			vab = '0';
		add_target_code("\'"+str.substr(i,j-i)+"$\'","p"," "," ","tmp"+char_to_str(vab)," ");
		tmp.var = "tmp"+char_to_str(vab);
		tmp.value = "\'"+str.substr(i,j-i)+"$\'";
		var_table.push_back(tmp);
	}
	it = it+2; //skip“)}”
}

//if analysis
void if_analysis(vector<WordID_pair>::iterator &it)
{
	string  op,mark,dest,dsrc;
	it++;
	if(it->word != "(")
	{
		cout<<"incorrect if statement： lack of'('"<<endl;
		exit(-1);
	}
	it++;
	dest = it->word;
	it++;
	mark = it->word;
	it++;
	dsrc = it->word;
	op = "if";

	add_target_code(" ",op,dest,dsrc,mark,"step"+char_to_str(lab+1));
	it++;
	if(it->word != ")")
	{
		cout<<"incorrect if statement： lack of ')'"<<endl;
		exit(-1);
	}
	it++; //skip‘{’
	it++;
    
	//analyzing else
	vector<WordID_pair>::iterator   it2 = it;
    while(it2->word != "}")
	{
		it2++;
	}
	it2++;
	//see if there is else
	if(it2->word == "else")
	{
		it2++; //skip‘{’
        while(it2->word != "}")
		{
			expression(it2);
			it2++;
		}
	}// end of else
	else
		it2--;
	lab = lab + 2;
    add_target_code(" ","jmp"," "," "," ","step"+char_to_str(lab));

	add_target_code(" ","pstep"," "," "," ","step"+char_to_str(lab-1));
	
	while(it->word != "}")
	{
		expression(it); //expression analysis
		it++;
	}
    add_target_code(" ","jmp"," "," "," ","step"+char_to_str(lab));
    add_target_code(" ","pstep"," "," "," ","step"+char_to_str(lab));

	it = it2;
}

//syntax analysis
void syntaxAnalysis(vector<WordID_pair> &AnalyzedData)
{
	vector<WordID_pair>::iterator  it=AnalyzedData.begin();
    if(it->word != "main")
	{
		cout<<"lack of main"<<endl;
		exit(-1);
	}
	it = it+3; //skip“（）”
	if(it->word != "{")
	{
		cout<<"main lacks'{'"<<endl;
		exit(-1);
	}
	it++;
	
    varTable_addition(it);
    //operations on code segment
	while(it != AnalyzedData.end())
	{
		//printf
		if(it->word == "printf")
		{
			analysis_print(it);
		}
		// if 
		else if(it->word == "if")
		{
			if_analysis(it);
		}
		else if(it->word == "}")
			break;
		//expression analysis
		else
		{
			expression(it); //expression
		}
		it++;
	}
}

//print syntactic analysis results
void syntax_print()
{
	vector<Variable>::iterator  it;
	cout<<"Variable Declaration and Initialization"<<endl;
	for(it = var_table.begin();it != var_table.end();it++)
	{
		cout<<it->var<<"   "<<it->value<<endl;
	}

	vector<Target>::iterator it2;
	cout<<"Intermediate Code"<<endl;
	for(it2 = target_code.begin();it2 != target_code.end();it2++)
	{
		cout<<it2->dsf<<"  "<<it2->op<<"  "<<it2->dest<<"  "<<it2->dsrc<<"  "<<it2->mark<<"  "<<it2->step<<endl;
	}
}

//Addition and Subtraction
void asm_add_sub(ofstream &out,string dsf,string op,string dest,string dsrc)
{
	out<<"    mov BL,"<<dest<<endl;
	if(op == "+")
		out<<"    add BL,"<<dsrc<<endl;
	else
		out<<"    sub BL,"<<dsrc<<endl;
	out<<"    mov "<<dsf<<",BL"<<endl;
}

//Multiplication
void asm_mult(ofstream &out,string dsf,string dest,string dsrc)
{
	out<<"    mov AL,"<<dest<<endl;
	out<<"    mov BH,"<<dsrc<<endl;
	out<<"    mul BH"<<endl;
	out<<"    mov BL,1"<<endl;
	out<<"    div BL"<<endl;
	out<<"    mov "<<dsf<<",AL"<<endl;
}

//Division
void asm_division(ofstream &out,string dsf,string dest,string dsrc)
{
	out<<"    mov AL,"<<dest<<endl;
	out<<"    CBW"<<endl;
	out<<"    mov BL,"<<dsrc<<endl;
	out<<"    div BL"<<endl;
	out<<"    mov "<<dsf<<",AL"<<endl;
}

//Assignment
void asm_assignment(ofstream &out,string dsf,string dest)
{
	out<<"    mov BL,"<<dest<<endl;
	out<<"    mov "<<dsf<<",BL"<<endl;
}

//Printing
void asm_print(ofstream &out,string dsf,string mark)
{
	//In terms of character
	if(mark=="%c")
	{
		out<<"    mov DL,"<<dsf<<endl;
		out<<"    mov AH,02H"<<endl;
		out<<"    int 21H"<<endl;
	}
	//In terms of integers
	else if(mark=="%d")
	{
		out<<"    mov AL,"<<dsf<<endl;
		out<<"    CBW"<<endl;
		out<<"    mov BL,10"<<endl;
		out<<"    DIV BL"<<endl;
		out<<"    mov BH,AH"<<endl;
		out<<"    add BH,30H"<<endl;
		out<<"    add AL,30H"<<endl;
		out<<"    CMP AL,48"<<endl;
		//Check is tens digit is 0
		lab = lab + 2;
		string step2 = "step" + char_to_str(lab);
		out<<"    JE "<<step2<<endl;
        string step1 = "step" + char_to_str(lab-1);
		out<<"  "<<step1<<":"<<endl;
		out<<"    mov DL,AL"<<endl;
		out<<"    mov AH,2"<<endl;
		out<<"    int 21H"<<endl;

		//output digits
		out<<"  "<<step2<<":"<<endl;
		out<<"    mov DL,BH"<<endl;
		out<<"    mov AH,2"<<endl;
		out<<"    int 21H"<<endl;
	}
	//output string
	else
	{
		out<<"    LEA DX,"<<mark<<endl;
		out<<"    mov AH,09"<<endl;
		out<<"    int 21H"<<endl;
	}
}

//if statement
void asm_if(ofstream &out,string dest,string dsrc,string mark,string step)
{
	out<<"    mov AL,"<<dest<<endl;
	out<<"    CMP AL,"<<dsrc<<endl;
	if(mark == ">")
		out<<"    JG "<<step<<endl;
	else if(mark == "<")
		out<<"    JL "<<step<<endl;
	else
	{
		cout<<"No other predicates accepted at the moment"<<endl;
		exit(-1);
	}
}


//Generating asm file
void asm_create(string file)
{
	//Var declaration
	ofstream   wfile(file.c_str());
	if(!wfile.is_open())
		cout<<"Cannot generate asm file"<<endl;

	vector<Variable>::iterator  it_var;
	
	wfile<<"ASSUME CS:code segment,DS: data segment"<<endl;
	//data seg
	wfile<<"datasg segment"<<endl;
	for(it_var=var_table.begin();it_var!=var_table.end();it_var++)
	{
		wfile<<"    "<<it_var->var<<" DB ";
		if(it_var->value != "")
			wfile<<it_var->value<<endl;
		else
			wfile<<"\'?\'"<<endl;
	}
	wfile<<"datasg ends"<<endl;
	//code seg
	wfile<<"codesg segment"<<endl;
	wfile<<"  start:"<<endl;
	wfile<<"    mov AX,datasg"<<endl;
	wfile<<"    mov DS,AX"<<endl;

	vector<Target>::iterator     it;
	Target        tmp;
    for(it = target_code.begin();it != target_code.end();it++)
	{
		//summation and subtraction
		if(it->op == "+" || it->op=="-")
			asm_add_sub(wfile,it->dsf,it->op,it->dest,it->dsrc);
		//multiplication
		else if(it->op == "*")
			asm_mult(wfile,it->dsf,it->dest,it->dsrc);
		//division
		else if(it->op == "/")
			asm_division(wfile,it->dsf,it->dest,it->dsrc);
		//assignment
		else if(it->op == "=")
			asm_assignment(wfile,it->dsf,it->dest);
		//printing
		else if(it->op == "p")
			asm_print(wfile,it->dsf,it->mark);
		//if predicate
		else if(it->op == "if")
		{
			asm_if(wfile,it->dest,it->dsrc,it->mark,it->step);
		}
		else if(it->op == "else")
		{
			cout<<"else No matching if"<<endl;
			exit(-1);
		}
		//jump statement
		else if(it->op == "jmp")
		{
			wfile<<"    JMP "<<it->step<<endl;
		}
		
		else if(it->op == "pstep")
		{
			wfile<<"  "<<it->step<<":"<<endl;
		}
		
		else
		{
			cout<<"This compiler does not support this operation for now"<<endl;
			exit(-1);
		}
	}

	//End of code
	wfile<<"    mov ax,4C00H"<<endl;
	wfile<<"    int 21H"<<endl;
	wfile<<"codesg ends"<<endl;
	wfile<<"  end start"<<endl;

	wfile.close();
}


int main(int argc,char* argv[])
{
	vector<WordID_pair>  AnalyzedData;

	string source;

	if(argc == 1)
	{
		cout<<"*****************************************************"<<endl;
	        cout<<"A C compiler written by Heming Tian "<<endl;
           	cout<<"\n Generating assembly file under current directory\n"<<endl;
		cout<<"*****************************************************"<<endl;
		cout<<"\n please input source file name：";
		cin>>source;
                //Lexical Analysis 
	    lexicalAnalysis(source,AnalyzedData);
		//Syntactic Analysis
	    syntaxAnalysis(AnalyzedData);
		//Generating asm
	    asm_create(asmfile(source));
		cout<<"\n Finished generating "<<endl;
	}
	else if(argc == 2)
	{

        
	    lexicalAnalysis(argv[1],AnalyzedData);
		
	    syntaxAnalysis(AnalyzedData);
		
	    asm_create(asmfile(argv[1]));
	}
	else if(argc == 3)
	{
		//lexical analysis
	    lexicalAnalysis(argv[1],AnalyzedData);
		//syntactic analysis
	    syntaxAnalysis(AnalyzedData);
		//creating asm
	    asm_create(asmfile(argv[2]));
	}
	else
	{
		cout<<"Correct Format ：compile.exe [source file] [asm]"<<endl;
	}

	//print lex 
 //   lex_print(AnalyzedData);
	//print syntax
//	syntax_print();
    /* cout<<"Would you like to see the generated codes? y/n"<<endl;
	char response;
	cin>>response;
    if(response=="y"){
	   
	   string line;
       while (w)
		   getline()	   
	}
	else */
	return 0;
}
