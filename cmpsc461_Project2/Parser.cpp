#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


void readGrammar() {}
void identifyTerminals() {}
void identifyNonTerminals() {}
void findFirsts() {}
void findFollows() {}

vector<vector<char>> terminals;
vector<vector<char>> nonTerminals;


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


}