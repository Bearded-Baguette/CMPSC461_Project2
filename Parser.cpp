#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <stack>

using namespace std;


struct rule
{
	string item;
	string startNonTerminal;
} rules[5];

struct state
{
	int stateNumber;
	vector<string> items;
	string startItem;
	vector<string> inputs;
};

struct edge
{
	state startState;
	state endState;
	string input;
};

struct reduce
{
	state reductionState;
	int ruleNumber;
};

void readGrammar(ifstream &inputFile);
void createStateDiagram();
void stateClosure(struct state&);
void addNonTerminal(string newTerminal);
bool isNonTerminal(string);
bool ruleIsAlreadyInState(struct state, string);
bool stateAlreadyExists(struct state&, string, string);
bool edgeAlreadyExists(struct state&, string);
void findEdges(struct state&, string, string, int);
string advanceDot(string);
void addStartItem(struct state&, string);
void addEdge(struct state&, struct state&, string);
void addItemToExistingState(struct state, string, string);
string getUsersInputAndCheckValidity();
string convertCharToString(char);
bool stringContainsCharacter(string s, char c);
void printParseTable();
void generateReductionList();
void casuallyMoveDots();
int findNextState(int, string);
bool stateIsAReductionState(int);
int numberOfCharacters(int);
string getGoTo(int);
string printCurrentStack(stack<string>);
void printParseStack(stack<string>, string);
void printParseStackHeaders();
void printGrammarInformation();

void checkUserInput(string);

int stateNumber = 0;
state acceptState;
string terminals;
vector<string> nonTerminals;
vector<edge> edges;
vector<state> states;
vector<reduce> reductions;

int main()
{
	ifstream inputFile;
	inputFile.open("input.txt");

	cout << "Welcome to our LR(0) grammar parser! \n\nThis parser was created by Devon Graves and Sam Williams!\n\n";

	// File is not found
	if (!inputFile.is_open())
	{
		cout << "ERROR! NO FILE FOUND!\n";
		cout << "GRAMMARS SHOULD BE PUT ON A FILE TITLED \"input.txt\" IN ORDER TO READ THEM!\n";
		return 1;
	}
	
	readGrammar(inputFile);
	createStateDiagram();
	generateReductionList();

	string userInput;
	bool loop = true;

	while (loop)
	{
		string menuOption = "1";
		cout << "==============================================\n";
		cout << "1. Enter string of characters\n";
		cout << "2. Show extended information on entered grammar\n";
		cout << "3. Exit\n";
		cout << "==============================================\n\n";
		cout << "Enter a menu option: ";
		cin >> menuOption;

		if (menuOption == "1")
		{
			userInput = getUsersInputAndCheckValidity();
			printParseTable();
			checkUserInput(userInput);
		}

		else if (menuOption == "2")
		{
			printGrammarInformation();
			printParseTable();
		}

		else if (menuOption == "3")
		{
			system("CLS");
			cout << "Thank you for looking at our LR(0) grammar parser!\n\nEnjoy the rest of your day!\n";
			loop = false;
		}
		else
		{
			system("CLS");
			cout << "Nice try, but we added input checking for the menu!\n\nPlease select a different option!\n\n";
		}

	}
	return 0;
}

// Reads in the grammar rules specfified in input.txt
void readGrammar(ifstream &inputFile)
{
	char in;
	int ruleNum = 0;
	string newItem;
	while (!inputFile.eof())
	{
		while (inputFile.peek() != '\n' && !inputFile.eof())
		{
			inputFile.get(in);
			if (!isspace(in))
			{
				if (in == '-') // Sets the startTerminal equal to the value of the current rule (only the terminal value).
				{
					rules[ruleNum].startNonTerminal = newItem;
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



	//Gets a list of terminals and assigns them to global variable

	string nonterminalsString;
	for (int i = 0; i < nonTerminals.size(); i++)
	{
		nonterminalsString.append(nonTerminals.at(i));
	}

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < rules[i].item.length(); j++)
		{
			if (rules[i].item[j] == '.')
			{
				while (rules[i].item[j])
				{
					j++;
					//if global string terminals doesn't already have the character inside of it and the character is not a space and the character is not found in the list of non terminals...
					if (stringContainsCharacter(terminals, rules[i].item[j]) == false && rules[i].item[j] != '\0'
						&& rules[i].item[j] != ' ' && !stringContainsCharacter(nonterminalsString, rules[i].item[j]))
					{
						//append the character to a list of terminal characters.
						terminals.append(convertCharToString(rules[i].item[j]));
					}
				}
			}
		}
	}
	//end gets list of terminals.
}

//converts a character to a string
string convertCharToString(char c)
{
	stringstream ss;
	string s;
	ss << c;
	ss >> s;
	return s;
}

// Checks if a string contains a specific character
bool stringContainsCharacter(string s, char c)
{
	for (int i = 0; i < s.length(); i++)
	{
		if (s[i] == c)
		{
			return true;
		}
	}
	return false;
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
	states[0].stateNumber = stateNumber;
	stateNumber++;
	stateClosure(states[0]);
	for (int i = 0; i < states.size(); i++) // Iterate through all states in the states vector
	{
		for (int j = 0; j < states[i].items.size(); j++) // Iterate through all items in each state vector
		{
			int pos = states[i].items.at(j).find('.');
			if (pos != states[i].items.at(j).size() - 1) // If the dot in an item is at the end of the item, do not find an edge fo
			{
				string c(1, states[i].items.at(j).at(pos + 1)); // c = the symbol immediately after the dot
				findEdges(states[i], states[i].items.at(j), c, pos);
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
						// If the starting nonTerminal matches the nonTerminal in the current state and the rule is not already in the state
						if (rules[j].startNonTerminal == c && !ruleIsAlreadyInState(currentState, rules[j].item))
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
void findEdges(struct state &currentState, string item, string symbol, int dotPos)
{
	if (symbol == "$")
	{
		acceptState = currentState;
		string acceptItem = advanceDot(item);
		acceptState.items.at(0) = acceptItem;
		acceptState.startItem = acceptItem;
		return;
	}
	if (item.size() - 1 == dotPos) { return; } // Double checks that the dot is not at the end of the item. Probably redundant, but it works.
	string newItem = advanceDot(item);
	if (stateAlreadyExists(currentState, newItem, symbol)) { return; } // Checks to see if the state being made already exists.
	if (edgeAlreadyExists(currentState, symbol)) // Checks to see if the edge being created already exists
	{
		addItemToExistingState(currentState, newItem, symbol);
		return;
	}
	state newState;
	newState.items.push_back(newItem); // Advances the dot on the item and pushes it onto the state that was just put on the vector.
	addStartItem(newState, newItem); // Sets the new item as the starting item of the state
	newState.stateNumber = stateNumber; // Sets the state number of the new state
	stateNumber++;
	addEdge(currentState, newState, symbol); // Adds an edge between states to the list of edges
	states.push_back(newState); // Add new state to the list of states
	stateClosure(states.at(states.size() - 1)); // Closes out the new state.
}

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

// Checks the new item with exisiting start items. If true, there is already a state that exists with that item
bool stateAlreadyExists(struct state &currentState, string newItem, string input)
{
	for (int i = 0; i < states.size(); i++)
	{
		if (states.at(i).startItem == newItem)
		{
			addEdge(currentState, states.at(i), input);
			return true;
		}
	}
	return false;
}

// Checks the new edge being created to see if it already exists
bool edgeAlreadyExists(struct state &startState, string input)
{
	for (int i = 0; i < startState.inputs.size(); i++)
	{
		if (startState.inputs.at(i) == input)
		{
			return true;
		}
	}
	startState.inputs.push_back(input); // Adds an input to the list of inputs for a state
	return false;
}

// Swaps the position of the dot with the next symbol in the item
string advanceDot(string s)
{
	int pos = s.find('.');
	if (pos != s.size() - 1)
	{
		swap(s[pos], s[pos + 1]);
	}
	return s;
}

// Sets the starting item of the state
void addStartItem(struct state &currentState, string newItem)
{
	currentState.startItem = newItem;
}

// Creates a new edge between states
void addEdge(struct state &start, struct state &end, string input)
{
	edge newEdge;
	newEdge.startState = start;
	newEdge.endState = end;
	newEdge.input = input;
	edges.push_back(newEdge);
}

// Adds an item to a state that already exists. This is not the same as closing out the state.
void addItemToExistingState(struct state start, string newItem, string input)
{
	for (int i = 0; i < edges.size(); i++)
	{
		if (edges.at(i).input == input && edges.at(i).startState.stateNumber == start.stateNumber)
		{
			edges.at(i).endState.items.push_back(newItem);
			states.at(edges.at(i).endState.stateNumber).items.push_back(newItem);
			stateClosure(states.at(edges.at(i).endState.stateNumber));
		}
	}
}

//Returns the user's input as a string. Checks if the input itself consists of legal characters and that the parenthesis are in order. needs to be called after readgrammar()
string getUsersInputAndCheckValidity()
{
	bool stringIsValid = false;
	bool allCharactersLegal = true;
	string usersInputString;

	while (stringIsValid == false)
	{
		cout << "\nPlease enter a string of characters: ";
		cin >> usersInputString;

		int numOfLeftParenthesis = 0;
		int numOfRightParenthesis = 0;
		for (int i = 0; i < usersInputString.length(); i++)
		{
			//Check if character is legal and found in the rules in input.txt
			for (int j = 0; j < terminals.length(); j++)
			{
				//character is found in input.txt
				if (usersInputString[i] == terminals[j])
				{
					break;
				}

				//character is not found in input.txt
				if (j == terminals.length() - 1)
				{
					allCharactersLegal = false;
				}
			}


			//Parenthesis checking.
			//if right parenthesis outnumber left parenthesis at any point of the iteration, the string is invalid.
			if (numOfRightParenthesis > numOfLeftParenthesis)
			{
				numOfRightParenthesis = -1;
				break;
			}
			else if (usersInputString[i] == '(')
			{
				numOfLeftParenthesis = numOfLeftParenthesis + 1;
			}
			else if (usersInputString[i] == ')')
			{
				numOfRightParenthesis = numOfRightParenthesis + 1;
			}

		}

		if (numOfLeftParenthesis == numOfRightParenthesis && allCharactersLegal == true)
		{
			stringIsValid = true;
		}
		else
		{
			allCharactersLegal = true;
			stringIsValid = false;
			cout << "\nInvalid string. Try again.";
		}
	}
	return usersInputString;
}

// Generates the list of reduce states
void generateReductionList()
{
	casuallyMoveDots();
	for (int i = 0; i < states.size(); i++) // Iterate through all states
	{
		for (int j = 0; j < states.at(i).items.size(); j++) // Iterate through all items in the state
		{
			int pos = states.at(i).items.at(j).find('.');
			if (pos == states.at(i).items.at(j).size() - 1)
			{
				string rule = states.at(i).items.at(j);
				for (int k = 0; k < 5; k++)
				{
					if (rules[k].item == rule)
					{
						reduce newReduction{ states.at(i), k };
						reductions.push_back(newReduction);
					}
				}
			}
		}
	}
}

// Nothing to see here. Move along.
void casuallyMoveDots()
{
	for (int i = 0; i < 5; i++)
	{
		int pos = rules[i].item.find('.');
		rules[i].item.erase(pos, 1);
		rules[i].item += '.';
	}
}

// Prints the parse table
void printParseTable()
{
	int rows = states.size();
	int columns = terminals.length() + nonTerminals.size();


	vector< vector<string> > parseTable(rows + 1, vector<string>(columns + 1));

	parseTable[0][0] = " ";

	//numbering states on left side of parse table
	for (int i = 0; i < rows; i++)
	{
		parseTable[i + 1][0] = to_string(i);
	}


	int temp;

	//adding terminals to top of parse table
	for (int i = 0; i < terminals.length(); i++)
	{

		parseTable[0][i + 1] = terminals[i];
		temp = i + 1;
	}

	//adding nonterminals to top of parse table
	for (int i = 0; i < nonTerminals.size(); i++)
	{
		parseTable[0][i + temp + 1] = nonTerminals.at(i);
	}


// sets shifts and gotos to table
for (int stateNumber = 0; stateNumber < edges.size(); stateNumber++)
{
	for (int rowNumber = 0; rowNumber < rows + 1; rowNumber++)
	{
		if (rowNumber == edges.at(stateNumber).startState.stateNumber)
		{

			//if the symbol is found within the list of terminals...
			if (terminals.find(edges.at(stateNumber).input) != std::string::npos)
			{
				for (int columnNumber = 0; columnNumber < columns + 1; columnNumber++)
				{
					if (parseTable[0][columnNumber] == edges.at(stateNumber).input)
					{
						string shiftNumber = "S";
						shiftNumber.append(to_string(edges.at(stateNumber).endState.stateNumber));

						parseTable[rowNumber + 1][columnNumber] = shiftNumber;
					}
				}
			}
			//if the symbol is not found within the list of terminals.
			else
			{
				for (int columnNumber = 0; columnNumber < columns + 1; columnNumber++)
				{
					if (parseTable[0][columnNumber] == edges.at(stateNumber).input)
					{
						string shiftNumber = "G";
						shiftNumber.append(to_string(edges.at(stateNumber).endState.stateNumber));
						parseTable[rowNumber + 1][columnNumber] = shiftNumber;
					}
				}
			}
		}
	}
}

// sets reduces to table
for (int rowNumber = 0; rowNumber < rows + 1; rowNumber++)
{
	for (int reductionNumber = 0; reductionNumber < reductions.size(); reductionNumber++)
	{
		if (rowNumber == reductions.at(reductionNumber).reductionState.stateNumber)
		{
			//filler up with r
			for (int columnNumber = 0; columnNumber < columns + 1; columnNumber++)
			{
				// if it's in a column under a terminal character
				if (terminals.find(parseTable[0][columnNumber]) != std::string::npos)
				{
					string reductionRuleNumber = "R";
					reductionRuleNumber += to_string(reductions.at(reductionNumber).ruleNumber);
					parseTable[rowNumber + 1][columnNumber] = reductionRuleNumber;
				}
			}
		}
	}
}

// sets accept state
for (int columnNumber = 0; columnNumber < columns + 1; columnNumber++)
{
	if (parseTable[0][columnNumber] == "$")
	{
		parseTable[acceptState.stateNumber + 1][columnNumber] = "a";
	}
}


//prints parse table
cout << endl << endl << "PARSE TABLE\n---------------------------------------------\n\n";
for (int i = 0; i < rows + 1; i++)
{
	for (int j = 0; j < columns + 1; j++)
	{
		cout << setw(5) << parseTable[i][j];
	}
	cout << endl << endl;
}

return;
}

// Checks the user input against the parse table to see if the input is valid.
void checkUserInput(string input)
{
	stack<string> inputStack;
	int currentState = 0;
	string nextIn, goToSymbol;
	inputStack.push(to_string(currentState));
	printParseStackHeaders();
	printParseStack(inputStack, input);
	for (int i = 0; i < input.size(); i++) // Iterates through the user input from left to right
	{
		nextIn = input[i];
		input[i] = ' ';
		inputStack.push(nextIn);
		currentState = findNextState(currentState, nextIn); // Sets the current state based on the start state and the input
		inputStack.push(to_string(currentState));
		printParseStack(inputStack, input);
		if (nextIn == "$")
		{
			cout << "\n\nThe string you have entered is valid!\n\n\n\n"; // IT TOOK OVER 24 HOURS OF CODING TO GET TO THIS ONE LINE!!!!! WOOOOOOOOOOOOOOOOOOOOOOOOO!!!!!!
			return;
		}
	checkForReduce:
		if (stateIsAReductionState(currentState))
		{
			int num = numberOfCharacters(currentState);
			goToSymbol = getGoTo(currentState);
			for (int j = 0; j < num * 2; j++) // Pop the input stack by the number of terminals and nonTerminals in an item times 2.
			{
				inputStack.pop();
			}
			currentState = stoi(inputStack.top()); // The new current state is now the number at the top of the stack
			inputStack.push(goToSymbol);
			for (int k = 0; k < edges.size(); k++) // Iterates through the list of edges to find one with a matching start state and input. This allows us to see the next state
			{
				if (edges.at(k).startState.stateNumber == currentState && edges.at(k).input == goToSymbol)
				{
					currentState = edges.at(k).endState.stateNumber;
					inputStack.push(to_string(currentState));
					printParseStack(inputStack, input);
					goto checkForReduce;
				}
			}
		}
	}
	cout << "\n\nThe string you have entered is invalid! Please try again!\n\n\n\n";
}

// Find the next state the stack will go to
int findNextState(int currentStateNumber, string input)
{
	for (int i = 0; i < edges.size(); i++)
	{
		if (edges.at(i).startState.stateNumber == currentStateNumber && edges.at(i).input == input)
		{
			return edges.at(i).endState.stateNumber;
		}
	}
}

// Check if a state is a reduction state
bool stateIsAReductionState(int currentStateNumber)
{
	for (int i = 0; i < reductions.size(); i++)
	{
		if (reductions.at(i).reductionState.stateNumber == currentStateNumber)
		{
			return true;
		}
	}
	return false;
}

// Return the number of characters in the rule after the arrowhead
int numberOfCharacters(int currentStateNumber)
{
	int num = 0; // Number of characters after the arrowhead. Does not include the dot.
	int pos = states.at(currentStateNumber).startItem.find('>') + 1; // This assumes the start item is the only item in thes state. Please don't break it.
	for (pos; states.at(currentStateNumber).startItem.at(pos) != '.'; pos++) // Likewise
	{
		num++;
	}
	return num;
}

// Returns the nonTerminal string from the reduce state
string getGoTo(int currentStateNumber)
{
	string c(1, states.at(currentStateNumber).startItem.at(0));
	return c;
}

// Prints the stack from the parse table
string printCurrentStack(stack<string> inputStack)
{
	stack<string> tempStack;
	string stack;
	int stackSize = inputStack.size();
	for (int i = 0; i < stackSize; i++)
	{
		tempStack.push(inputStack.top());
		inputStack.pop();
	}
	for (int i = 0; i < stackSize; i++)
	{
		stack += tempStack.top();
		if (tempStack.top() == "$")
		{
			return stack;
		}
		tempStack.pop();
	}
	return stack;
}

// Prints the stack and input of the parse table
void printParseStack(stack<string> inputStack, string inputString)
{
	cout << setw(20) << left << printCurrentStack(inputStack);
	cout << setw(10) << inputString << endl;
}

// Prints headers for the parse table
void printParseStackHeaders()
{
	cout << setw(20) << left << "STACK";
	cout << setw(10) << "INPUT" << endl;
	cout << "______________________________\n";
}

// Prints information about the states, edges, and reduction states. Also prints the list of terminals and nonterminals.
void printGrammarInformation()
{
	string nonterminalsString;
	for (int i = 0; i < nonTerminals.size(); i++)
	{
		nonterminalsString.append(nonTerminals.at(i));
	}

	cout << endl << "Here are the entered rules:\n";
	for (int i = 0; i < 5; i++)
	{
		cout << rules[i].item << endl;
		for (int j = 0; j < rules[i].item.length(); j++)
		{
			if (rules[i].item[j] == '.')
			{
				while (rules[i].item[j])
				{
					j++;
					//if global string terminals doesn't already have the character inside of it and the character is not a space and the character is not found in the list of non terminals...
					if (stringContainsCharacter(terminals, rules[i].item[j]) == false && rules[i].item[j] != '\0'
						&& rules[i].item[j] != ' ' && !stringContainsCharacter(nonterminalsString, rules[i].item[j]))
					{
						//append the character to a list of terminal characters.
						terminals.append(convertCharToString(rules[i].item[j]));
					}
				}
			}
		}
	}

	cout << endl << "Nonterminals: " << nonterminalsString << endl;
	cout << "\nTerminals: " << terminals << endl;
	cout << endl << endl << endl;
	//end gets list of terminals.
	for (int k = 0; k < states.size(); k++)
	{
		cout << "State " << states.at(k).stateNumber << endl;
		for (int i = 0; i < states.at(k).items.size(); i++)
		{
			cout << states.at(k).items.at(i) << endl;
		}
		cout << "-----------------\n";
		cout << endl;
	}
	for (int i = 0; i < edges.size(); i++)
	{
		cout << "Edge " << i << " connects " << edges.at(i).startState.stateNumber << " with " << edges.at(i).endState.stateNumber << endl;
		cout << "The input was " << edges.at(i).input << endl;
		cout << "------------------------------------\n";
	}
	for (int i = 0; i < reductions.size(); i++)
	{
		cout << "Reduction at state " << reductions.at(i).reductionState.stateNumber << endl;
		cout << "The reduction item is " << reductions.at(i).ruleNumber << endl;
		cout << "---------------------------\n";
	}
}
