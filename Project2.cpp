#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>

using namespace std;

vector<string> terminals;
vector<string> nonTerminals;
vector<rule> states;
vector<tuple<int, int>> edges;

struct rule
{
	vector<char> item;
	string leadTerminal;
} rules[5];


void readGrammar(ifstream &inputFile);
void createStateDiagram(vector<rule>& states);

int main()
{
	ifstream inputFile;
	inputFile.open("input.txt");

	// File is not found
	if (!inputFile.is_open())
	{
		cout << "ERROR! NO FILE FOUND!\n";
		system("PAUSE");
		return 1;
	}

	// File is found
	else
	{
		cout << "File opened successfully!\n";
	}

	readGrammar(inputFile);
	cout << rules[0].leadTerminal;
	cout << "Done";
	return 0;
}

void readGrammar(ifstream &inputFile)
{
	char in;
	int ruleNum = 0;
	while (!inputFile.eof())
	{
		vector <char> newItem;
		while (inputFile.peek() != '\n' && !inputFile.eof())
		{
			inputFile.get(in);
			if (!isspace(in))
			{
				if (in == '-') // Sets the leadTerminal equal to the value of the current rule (only the terminal value)
				{
					rules[ruleNum].leadTerminal.assign(rules[ruleNum].item.begin(), rules[ruleNum].item.end());
				}

				rules[ruleNum].item.push_back(in); // Adds a dot after the arrow in the rule

				if (in == '>')
				{
					rules[ruleNum].item.push_back('.');
				}
			}
		}
		ruleNum++;
		inputFile.get(in);
	}
}

void createStateDiagram(vector<rule>& states)
{

}

