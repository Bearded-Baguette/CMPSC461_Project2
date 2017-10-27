#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <algorithm>

using namespace std;


struct rule
{
	vector<string> item;
	string leadTerminal;
} rules[5];

struct state
{
	vector<string> items;
} states[9];

void readGrammar(ifstream &inputFile);
void createStateDiagram();
void stateClosure(struct state);
void addTerminal(string newTerminal);

vector<string> terminals;
vector<state> stateDiagram;
vector<tuple<int, int>> edges;

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
	createStateDiagram();
	cout << terminals.at(0);
	cout << rules[0].leadTerminal;
	cout << "Done";
	return 0;
}

void readGrammar(ifstream &inputFile)
{
	char in;
	int ruleNum = 0;
	string newItem;
	while (!inputFile.eof())
	{
		
	while (!inputFile.eof())
	{
		vector <char> newItem;
		while (inputFile.peek() != '\n' && !inputFile.eof())
		{
			inputFile.get(in);
			if (!isspace(in))
			{
				if (in == '-') // Sets the leadTerminal equal to the value of the current rule (only the terminal value).
				{
					rules[ruleNum].leadTerminal = newItem;
					addTerminal(newItem);
				}

				newItem += in; // Adds character to the new item.

				if (in == '>') // Adds a dot after the arrow in the rule.
				{
					newItem += '.';
				}
			}
		}
		rules[ruleNum].item.push_back(newItem);
		ruleNum++;
		inputFile.get(in);
		newItem = "";
	}
}

// Adds a new terminal to the list of terminals.
void addTerminal(string newTerminal)
{
	if (find(terminals.begin(), terminals.end(), newTerminal) == terminals.end())
	{
		terminals.push_back(newTerminal);
	}
}

// Start of the creation of the state diagram. Starts by passing the first grammar rule into stateClosure.
void createStateDiagram()
{
	states[0].items.push_back(rules[0].item.at(0));
	stateClosure(states[0]);
}

// Adds all relevant rules to the current state that was passed in.
void stateClosure(struct state currentState)
{
	int pos;
	for (int i = 0; i < currentState.items.size(); i++) // Iterates through all items in a state
	{
		pos = currentState.items.at(i).find('.');
		for (int k = 0; k < terminals.size(); k++) // Iterates through all terminals in a state
		{
			if (pos != currentState.items.at(i).size() - 1 && terminals.at(k).compare(currentState.items.at(pos + 1)))
			{

			}
		}
	}
}


void findGoTo() {} // This will add items to a state. The last part of this function should be to call stateClosure to close out the state.

void reduceStack() {} // This will essentially pop the states backwards. Might have to use a stack to track what state we are creating.
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