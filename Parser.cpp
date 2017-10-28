#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <algorithm>

using namespace std;


struct rule
{
	string item;
	string startTerminal;
} rules[5];

struct state
{
	vector<string> items;
};

void readGrammar(ifstream &inputFile);
void createStateDiagram();
void stateClosure(struct state&);
void addNonTerminal(string newTerminal);
bool isNonTerminal(string);
bool ruleIsAlreadyInState(struct state, string);
void findEdges(string, string, int);
string advanceDot(string);

vector<string> terminals;
vector<string> nonTerminals;
vector<tuple<int, int>> edges;
vector<state> states;

int main()
{
	ifstream inputFile;
	inputFile.open("input.txt");

	// File is not found
	if (!inputFile.is_open())
	{
		cout << "ERROR! NO FILE FOUND!\n";
		return 1;
	}

	// File is found
	else
	{
		cout << "File opened successfully!\n";
	}

	readGrammar(inputFile);
	createStateDiagram();
	for (int k = 0; k < states.size();k++)
	{
		for (int i = 0; i < states.at(k).items.size(); i++)
		{
			cout << "State " << i << endl;
			cout << "-----------------\n";
			cout << states.at(k).items.at(i) << endl;
		}
	}
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
			while (inputFile.peek() != '\n' && !inputFile.eof())
			{
				inputFile.get(in);
				if (!isspace(in))
				{
					if (in == '-') // Sets the startTerminal equal to the value of the current rule (only the terminal value).
					{
						rules[ruleNum].startTerminal = newItem;
						addNonTerminal(newItem);
					}

					newItem += in; // Adds character to the new item.

					if (in == '>') // Adds a dot after the arrow in the rule.
					{
						newItem += '.';
					}
				}
			}
			rules[ruleNum].item = newItem;
			ruleNum++;
			inputFile.get(in);
			newItem = "";
		}
	}
}

	// Adds a new terminal to the list of terminals.
	void addNonTerminal(string newNonTerminal)
	{
		if (find(nonTerminals.begin(), nonTerminals.end(), newNonTerminal) == nonTerminals.end())
		{
			nonTerminals.push_back(newNonTerminal);
		}
	}

	// Start of the creation of the state diagram. Starts by passing the first grammar rule into stateClosure.
	void createStateDiagram()
	{
		state newState{};
		states.push_back(newState);
		states[0].items.push_back(rules[0].item);
		stateClosure(states[0]);
		for (int i = 0; i < states.size(); i++)
		{
			for (int j = 0; j < states[i].items.size(); j++)
			{
				int pos = states[i].items.at(j).find('.');
				if (pos != states[i].items.at(j).size() - 1)
				{
					string c(1, states[i].items.at(j).at(pos + 1));
					findEdges(states[i].items.at(j), c, pos);
				}
			}
		}
	}

	// Adds all relevant rules to the current state that was passed in.
	void stateClosure(struct state &currentState)
	{
		int pos;
		for (int i = 0; i < currentState.items.size(); i++) // Iterates through all items in a state
		{
			pos = currentState.items.at(i).find('.'); // Find the location of the dot in the item
			if (pos != (currentState.items.at(i).size() - 1)) // If the dot is not at the end of the item
			{
				string c(1, currentState.items.at(i).at(pos + 1)); // Converts character after the dot to a string
				for (int k = 0; k < nonTerminals.size(); k++) // Iterate through the list of nonTerminals
				{
					if (isNonTerminal(c))
					{
						for (int j = 0; j < 5; j++) // Iterate through the different rules
						{
							if (rules[j].startTerminal == c && !ruleIsAlreadyInState(currentState, rules[j].item)) // If the starting nonTerminal matches the nonTerminal in the current state and the rule is not already in the state
							{
								currentState.items.push_back(rules[j].item);
							}
						}
					}
				}
			}
		}
	}

	// This will add items to a state. The last part of this function should be to call stateClosure to close out the state.
	void findEdges(string item, string symbol, int dotPos)
	{
		if (symbol == "$"){return;}
		if (item.size() - 1 == dotPos) { return; }
		state newState;
		states.push_back(newState); // Add new state to the list of states
		states.at(states.size() - 1).items.push_back(advanceDot(item));
		stateClosure(states.at(states.size() - 1));
	}

	void reduceStack() {} // This will essentially pop the states backwards. Might have to use a stack to track what state we are creating.

	// Checks if a passed character is in the nonTerminal list. Should have passed in a string, but no terminals we're passing in will be more than 1 character long.
	bool isNonTerminal(string c)
	{
		if (find(nonTerminals.begin(), nonTerminals.end(), c) != nonTerminals.end())
		{
			return true;
		}
		return false;
	}

	// Checks to see if a new rule being added is already in the state.
	bool ruleIsAlreadyInState(struct state currentState, string ruleInQuestion)
	{
		for (int i = 0; i < currentState.items.size(); i++)
		{
			if (ruleInQuestion.compare(currentState.items.at(i)) == 0) // If compare returns 0, the strings match
			{
				return true;
			}
		}
		return false;
	}

	// Swaps the position of the dot with the next symbol in the item
	string advanceDot(string s)
	{
		int pos = s.find('.');
		if (pos != s.size() - 1)
		{
			swap(s[pos], s[pos +1]);
		}
		return s;
	}