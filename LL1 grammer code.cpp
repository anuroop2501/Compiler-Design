#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stack>
#include <set>
#include <algorithm>
#include <map>
using namespace std;

vector<string> one_d;
vector< vector<string> > two_d;
vector<string> terminals, non_terminals;
vector< vector<string> > First;
vector< vector<string> > Follow;
map<string,int> firstMake;
vector< vector<string> > followMake;
map<string, int> nullProd;
vector< vector<string> > prodFirst;
vector< vector<string> > grammarTable;
vector< vector<string> > firstCheck;
string startSymbol;
int sizeTerm;
int isLL1Grammar=1;

void printVecTwoD(vector< vector<string> > v)
{
    int h=v.size();
    for(int i=0;i<h;i++)
    {
        int l=v[i].size();
        for(int j=0;j<l;j++)
        {
            cout << v[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

void printVecOneD(vector<string> v)
{
    int l=v.size();
    for(int j=0;j<l;j++)
    {
        cout << v[j] << " " ;
    }
    cout << endl;
}

int findInVector(vector<string> v, string s)
{
    int i=v.size();
    for(int j=0;j<i;j++)
    {
        if(v[j]==s)
        {
            return j;
        }
    }
    return -1;
}

void readGrammar()
{
	fstream ingrammar;
	char input[] = "grammar.txt";
	string buffer;
	ingrammar.open(input, ios::in);
	if (!ingrammar.is_open()) {
	  cerr << "Can't open input file " << input << endl;
	  return;
	}
	one_d.push_back(" ");
	grammarTable.push_back(one_d);
	one_d.clear();
	while(getline(ingrammar, buffer))
	{
		size_t prev = 0, pos;
    		while ((pos = buffer.find_first_of(":| ", prev)) != string::npos)
    		{
        		if (pos > prev)
        		{
            			one_d.push_back(buffer.substr(prev, pos-prev));
            	}
        		prev = pos+1;
    		}
    		if (prev < buffer.length())
        	{
        		one_d.push_back(buffer.substr(prev, string::npos));
        	}
        	non_terminals.push_back(one_d.front());
            two_d.push_back(one_d);
            one_d.clear();
        	one_d.push_back(non_terminals.at(non_terminals.size()-1));
        	First.push_back(one_d);
        	Follow.push_back(one_d);
        	followMake.push_back(one_d);
        	grammarTable.push_back(one_d);
	    	one_d.clear();
    }
    startSymbol = non_terminals[0];
	ingrammar.close();
}

int checkNonTerminals(string s)
{
    int l = non_terminals.size();
    for(int j=0;j<l;j++)
    {
        if(s.compare(non_terminals[j]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void checkForTerminals(string s)
{
    int l = s.length();
    string r;
    for(int i=0;i<l;i++)
    {
        r = s.at(i);
        if(checkNonTerminals(r) == 0 && findInVector(terminals, r) == -1)
        {
            terminals.push_back(r);
        }
    }

}

void makeTerminalsList()
{
    int h=two_d.size();
    for(int i=0;i<h;i++)
    {
        int l=two_d[i].size();
        for(int j=1;j<l;j++)
        {
            checkForTerminals(two_d[i][j]);
        }
    }
    int d=terminals.size();
    for(int i=0;i<d;i++)
    {
        if(terminals[i]!="^")
        {
            grammarTable[0].push_back(terminals[i]);
        }
    }
    grammarTable[0].push_back("$");
}

int isTerminal(string s)
{
    int i=terminals.size();
    for(int j=0;j<i;j++)
    {
        if(terminals[j].compare(s)==0)
        {
            return 1;
        }
    }
    return 0;
}

vector<string> findFirstString(string s, int l)
{
    int m=two_d[l].size();
    vector<string> dummyVec;
    vector<string> dummyVec1;
    for(int j=1;j<m;j++)
    {
        string h=two_d.at(l).at(j);
        int k = h.length();
        for(int i=0;i<k;i++)
        {
            string t = h.substr(i,i+1);
            if(isTerminal(t) == 1)
            {
                dummyVec.push_back(t);
                break ;
            }
            if(isTerminal(t) == 0)
            {
                int r = findInVector(non_terminals, t);
                dummyVec1 = findFirstString(t, r);
                for (int z = 0; z < dummyVec1.size(); ++z)
                {
                    dummyVec.push_back(dummyVec1[z]);
                }
            }
            if(findInVector(dummyVec, "^") == -1)
            {
                break;
            }
        }
    }
    sort( dummyVec.begin(), dummyVec.end() );
    dummyVec.erase( unique( dummyVec.begin(), dummyVec.end() ), dummyVec.end() );
    firstMake[s] = 1;
    for (int z = 0; z < dummyVec.size(); ++z)
    {
        First[l].push_back(dummyVec[z]);
        if(dummyVec[z] == "^")
        {
            nullProd[s] = 1;
        }
    }
    return dummyVec;
}

void findFollowString()
{
    int noProd,l,o,r,noOption,len;
    noProd = two_d.size();
    for(int i=0;i<noProd;i++)
    {
        noOption = two_d[i].size();
        for(int j=1;j<noOption;j++)
        {
            string s = two_d[i][j];
            len = s.length();
            for(int k=0;k<len-1;k++)
            {
                char dummy = s.at(k);
                char dummy1 = s.at(k+1);
                if(!(isTerminal(string(1,dummy))))
                {
                    int l=findInVector(non_terminals, string(1,dummy));
                    int o=findInVector(non_terminals, string(1,dummy1));
                    int r=First[l-1].size();
                    if(isTerminal(string(1,dummy1)))
                    {
                        Follow[l].push_back(string(1,dummy1));
                    }
                    else
                    {
                        if(nullProd[First[o][0]] == 1)
                        {
                            followMake[l].push_back(two_d[i][0]);
                        }
                        int y = First[o].size();
                        for (int z = 1; z < y; ++z)
                        {
                            if(First[o][z] != "^")
                            {
                                Follow[l].push_back(First[o][z]);
                            }
                        }
                    }
                    }
            }
            char dummy = s.at(len-1);
            l=findInVector(non_terminals, string(1,dummy));
            if(dummy!='^' && l!=-1)
            {
                followMake[l].push_back(two_d[i][0]);
            }
        }
    }
}

vector<string> findFirst(string s)
{
    int k=s.length();
    vector<string> dum;
    for(int i=0;i<k;i++)
    {
        string d = string(1,s.at(i));
        if(isTerminal(d))
        {
            dum.push_back(d);
            break;
        }
        if(nullProd[d] == 1)
        {
            int y=findInVector(non_terminals, d);
            for (int z = 1; z < y; ++z)
            {
                dum.push_back(First[y][z]);
            }
        }
        if(nullProd[d] != 1)
        {
            int y=findInVector(non_terminals, d);
            int d=First[y].size();
            for (int z = 1; z < d; ++z)
            {
                dum.push_back(First[y][z]);
            }
            break;
        }
    }
    return dum;
}

void makeFirst()
{
    int k=First.size();
    string first;
    vector<string> firstPush;
    for(int i=0;i<k;i++)
    {
        first = First[i][0];
        if(firstMake[first] != 1)
        {
            firstPush = findFirstString(first, i);
        }
    }
}

void makeFollow()
{
    int k=Follow.size();
    string follow;
    vector<string> followPush;
    Follow[0].push_back("$");
    findFollowString();
    for(int i=0;i<k;i++)
    {
        int e = followMake[i].size();
        for(int j=1;j<e;j++)
        {
            int l=findInVector(non_terminals, followMake[i][j]);
            int y = Follow[l].size();
            for (int z = 1; z < y; ++z)
            {
                 Follow[i].push_back(Follow[l][z]);
            }
        }
        sort( Follow[i].begin()+1, Follow[i].end() );
        Follow[i].erase( unique( Follow[i].begin()+1, Follow[i].end() ), Follow[i].end() );
    }
}

void makeFirstProd()
{
    int r=two_d.size();
    vector<string> dum;
    vector<string> dum1;
    for(int i=0;i<r;i++)
    {
        int s=two_d[i].size();
        for(int j=1;j<s;j++)
        {
            string p = two_d[i][j];
            dum.push_back(p);
            dum1 = findFirst(p);
            int u=dum1.size();
            for (int z = 0; z < u; ++z)
            {
                if(dum1[z]=="^")
                {
                        nullProd[p]=1;
                }
                dum.push_back(dum1[z]);
                 }
            prodFirst.push_back(dum);
            dum.clear();
        }
    }
}
void makeGrammarTable()
{
    int len=grammarTable[0].size();
    int len1=non_terminals.size();
    for(int i=1;i<=len1;i++)
    {
        for(int j=1;j<=len;j++)
        {
            grammarTable[i].push_back("");
        }
    }
    int r=two_d.size();
    int u=0;
    for(int i=0;i<r;i++)
    {
        int s=two_d[i].size();
        for(int j=1;j<s;j++)
        {
            string s = two_d[i][j];
            int d=prodFirst[u].size();
            for(int k=1;k<d;k++)
            {
                    int l=findInVector(grammarTable[0], prodFirst[u][k]);
                    if(l!=-1)
                    {
                        if(grammarTable[i+1][l].length() > 0)
                        {
                            isLL1Grammar=0;
                            cout << "Not LL1 Grammar1: Ambiguous" << endl;
                            cout << grammarTable[i+1][0] << " " << grammarTable[i+1][l] << endl;
                            cout << grammarTable[i+1][0] << " " << s << endl;
                        }
                        grammarTable[i+1][l]=s;
                        }
                    if(l==-1)
                    {
                        if(grammarTable[i+1][len-1].length() > 0)
                        {
                            isLL1Grammar=0;
                            cout << "Not LL1 Grammar2: Ambiguous" << endl;
                            cout << grammarTable[i+1][0] << " " << grammarTable[i+1][len-1] << endl;
                            cout << grammarTable[i+1][0] << " " << s << endl;
                        }
                        grammarTable[i+1][len-1]=s;
                        int fol = Follow[i].size();
                        for(int z=1;z<fol;z++)
                        {
                            string fols=Follow[i][z];
                            int b=findInVector(grammarTable[0], fols);
                            grammarTable[i+1][b]=s;
                        }
                    }
            }
            u++;
        }
    }
}

void runString(string s)
{
    int len=s.length();
    stack<string> grammar;
    string grammars="$"+startSymbol;
    grammar.push("$");
    grammar.push(startSymbol);
    int slen=0,step=1;
    while(!(grammar.empty()))
    {
        cout << "Step " << step << ": ";
        step++;
        string sg =grammar.top();
        int st=findInVector(terminals, sg);
        string str=string(1,s[slen]);
        if(sg=="$")
        {
            cout << grammars << "\t\t" << s.substr(slen,len) << "\t\t" << "Accept" << endl;
            grammar.pop();
            grammars.pop_back();
            break;
        }
        if(st!=-1 && sg.compare(str) == 0)
        {
            cout << grammars << "\t\t" << s.substr(slen,len) << "\t\t" << "Match" << endl;
            grammar.pop();
            grammars.pop_back();
            slen++;
        }
        else
        {
            st=findInVector(non_terminals, sg);
            int strt=findInVector(grammarTable[0], str);
            if(strt==-1)
            {
                cout << "Symbol " << str << " doesnot exist in Grammar" << endl;
                break;
            }
            string dum=grammarTable[st+1][strt];
            if(dum=="")
            {
                cout << "String doesnot belong to grammar: " << sg << " " << str << endl;
                return ;
            }
            string action=grammarTable[st+1][0]+":"+dum;
            cout << grammars << "\t\t" << s.substr(slen,len) << "\t\t" << action << endl;
            if(dum=="^")
            {
                grammar.pop();
                grammars.pop_back();
            }
            else
            {
                grammar.pop();
                grammars.pop_back();
                int duml=dum.length();
                for(int z=duml-1;z>=0;z--)
                {
                    grammar.push(string(1,dum[z]));
                    grammars.push_back(dum[z]);
                }
            }
        }
    }
}

vector<string> intersection(vector<string> &v1, vector<string> &v2)
{
    vector<string> v3;
    sort(v1.begin(), v1.end());
    sort(v2.begin(), v2.end());
    set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v3));
    return v3;
}

void checkLLGrammar()
{
    cout << "Checking for LL1 Grammar..." << endl;
    int len=two_d.size(),m=0;
    vector<string> v;
    vector<string> v1;
    for(int i=0;i<len;i++)
    {
        int len1=two_d[i].size();
        int nullp = 0;
        vector<string> dum;
        for(int j=1;j<len1;j++)
        {
            string s = two_d[i][j];
            if(nullProd[s]==1)
            {
                nullp++;
            }
            if(nullp > 1)
            {
                isLL1Grammar=0;
                cout << "Violating Rule 2: Not LL1 Grammar" << endl;
                cout << "Null Production from multiple options: ";
                printVecOneD(two_d[i]);
            }
            int h=prodFirst[m].size();
            for(int z=1;z<h;z++)
            {
                dum.push_back(prodFirst[m][z]);
                if(prodFirst[m][z]=="^")
                {
                    v1 = intersection(Follow[i], prodFirst[m]);
                    if(v1.size() > 0)
                    {
                        isLL1Grammar=0;
                        cout << "Violating Rule 3: Not LL1 Grammar" << endl;
                    }
                    v1.clear();
                }
            }
            firstCheck.push_back(dum);
            dum.clear();
            m++;
            if(j==2 && !(j<2))
            {
                v = intersection(firstCheck[m-j], firstCheck[m-j+1]);
                if(v.size() > 0)
                {
                    isLL1Grammar=0;
                    cout << "Violating Rule 1: Not LL1 Grammar" << endl;
                    cout << two_d[i][0] << " starts from: ";
                    printVecOneD(v);
                }
            }
            if(j>2)
            {
                v = intersection(v, firstCheck[m-j]);
                if(v.size() > 0)
                {
                    isLL1Grammar=0;
                    cout << "Violating Rule 1: Not LL1 Grammar" << endl;
                    cout << two_d[i][0] << " starts from: ";
                    printVecOneD(v);
                }
            }
        }
        v.clear();
    }
}

void checkLeftRecursion()
{
    cout << "Checking for Left Recursion" << endl;
    int len=two_d.size();
    for(int i=0;i<len;i++)
    {
        int len1=two_d[i].size();
        for(int j=1;j<len1;j++)
        {
            string s = two_d[i][j];
            if(two_d[i][0]==string(1,s[0]))
            {
                isLL1Grammar=0;
                cout << "Not LL1 Grammar: Left Recursion: " << two_d[i][0] << " " << s << endl;
            }
        }
    }
}

int main()
{
    string input;
    cin>>input;
    cout << "Reading Grammar" << endl;
    readGrammar();
    checkLeftRecursion();
    if(isLL1Grammar==1)
    {
        makeTerminalsList();
        makeFirst();
        makeFollow();
        makeFirstProd();
        makeGrammarTable();
        checkLLGrammar();
        if(isLL1Grammar==1)
        {
            cout << "Given Grammar is LL1." << endl;
            cout << "Grammar" << endl;
            printVecTwoD(two_d);
            cout << "First" << endl;
            printVecTwoD(First);
            cout << "Follow" << endl;
            printVecTwoD(Follow);
            cout << "Prod First" << endl;
            printVecTwoD(prodFirst);
            cout << "Grammar Table" << endl;
            printVecTwoD(grammarTable);
            runString(input);
        }
    }
    return 0;
}
