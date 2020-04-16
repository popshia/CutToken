//  System Program Homework part 1
//  CutToken ( Lexical Analysis ) ( x86 )
//  10627130 資訊三甲 林冠良
//  10/25 23:59 DEADLINE

#include <cctype>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#define NOT !

using namespace std;

//static int Command;
static ifstream Input;
static ofstream Output;

typedef struct TokenData {
    int tokenType = 0;
    int tableNum = 0;
    string token = "\0";
} TokenData;

class LexicalAnalysis {
public:
    vector<TokenData> opCodeTable;                // 1 ok
    vector<TokenData> pseudoInstructionsTable;    // 2 ok
    vector<TokenData> registerOrAccumulatorTable; // 3 ok
    vector<TokenData> delimetersTable;            // 4 ok
    vector<TokenData> symbolHashTable;            // 5 ok
    vector<TokenData> intHashTable;               // 6 ok
    vector<TokenData> stringHashTable;            // 7 ok
    vector<TokenData> commentTable;               // 8 ok
    vector<TokenData> currentLineTokens;


    void Run() { GetToken(); } // start function

    void inputTable() {
        string inputTableName = "\0";
        for ( int i = 1; i <= 4; i++ ) {
            inputTableName = "Table" + to_string( i ) + ".table";
            Input.open( inputTableName );
            while ( !Input.eof() ) {
                TokenData singleToken;
                Input >> singleToken.tableNum;
                Input >> singleToken.token;
                singleToken.tokenType = i;
                if ( i == 1 ) opCodeTable.push_back( singleToken );
                if ( i == 2 ) pseudoInstructionsTable.push_back( singleToken );
                if ( i == 3 ) registerOrAccumulatorTable.push_back( singleToken );
                if ( i == 4 ) delimetersTable.push_back( singleToken );
            } // input & push
            Input.close();
        } // read and save
    } // input tables
private:
    bool IsDelimeter( char ch, TokenData& delimeterToken ) {
        string check( 1, ch );
        for ( int i = 0; i < delimetersTable.size(); i++ ) {
            if ( check == delimetersTable[i].token ) {
                delimeterToken = delimetersTable[i];
                return true;
            } // find
        } // check the table
        return false;
    } // check if the char is delimeter

    void GetToken() {
        currentLineTokens.clear();
        string wholeLine = "\0";
        string singleToken;
        string comment;
        int commentIndex = 1;
        TokenData commentToken;
        TokenData delimeterToken;
        while ( getline( Input, wholeLine ) ) {
            Output << wholeLine << endl;
            for ( int i = 0; i < wholeLine.length(); i++ ) {
                if ( IsDelimeter( wholeLine[i], delimeterToken ) || ( wholeLine[i] == ' ' ) || ( wholeLine[i] == '\n' ) || ( wholeLine[i] == '\t' ) || ( wholeLine[i] == ';' ) || ( wholeLine[i] == '\r' ) || ( wholeLine[i] == '\0' ) ) {
                    if ( !singleToken.empty() ) {
                        Identify( singleToken, wholeLine[i] );
                        singleToken.clear();
                    } // identify
                    if ( IsDelimeter( wholeLine[i], delimeterToken ) ) {
                        currentLineTokens.push_back( delimeterToken );
                        if ( wholeLine[i] == ';' ) {
                            for ( int k = 0; k < wholeLine.length(); k++ )
                                comment.push_back( wholeLine[i] );
                            commentToken.tokenType = 8;
                            commentToken.tableNum = commentIndex;
                            commentToken.token = comment;
                            commentTable.push_back( commentToken );
                            currentLineTokens.push_back( commentToken );
                            commentIndex++;
                            break;
                        } // comment case
                    } // delimeter
                } // non-character case
                else {
                    singleToken.push_back( wholeLine[i] );
                    if ( i == wholeLine.length() - 1 && !singleToken.empty() ) {
                        singleToken.erase( remove( singleToken.begin(), singleToken.end(), '\r' ), singleToken.end() );
                        singleToken.erase( remove( singleToken.begin(), singleToken.end(), '\n' ), singleToken.end() );
                        Identify( singleToken, wholeLine[i] );
                        singleToken.clear();
                    } // end of line*/
                } // character case
            } // read single line
            for ( int i = 0; i < currentLineTokens.size(); i++ ) Output << "(" << currentLineTokens[i].tokenType << "," << currentLineTokens[i].tableNum << ")";
            Output << endl;
            currentLineTokens.clear();
        } // while not EOF
    } // cut the tokens

    void Identify( string singleToken, char currentChar ) {
        string save = singleToken;
        int ASCII = 0;
        int indexPlus = 0;
        if ( currentChar == '\'' ) {
            stringHashTable.resize( 100 );
            indexPlus = 0;
            TokenData stringToken;
            for ( int i = 0; i < singleToken.size(); i++ )
                ASCII += (int)singleToken[i];
            while ( stringHashTable[ASCII % 100 + indexPlus].token != "\0" && stringHashTable[ASCII % 100 + indexPlus].token != singleToken ) {
                indexPlus += 1;
                if ( ( ASCII % 100 + indexPlus ) > 99 ) indexPlus = -( ASCII % 100 );
            } // hashing
            stringToken.tokenType = 7;
            stringToken.tableNum = ASCII % 100 + indexPlus;
            stringToken.token = singleToken;
            stringHashTable[ASCII % 100 + indexPlus] = stringToken;
            currentLineTokens.push_back( stringToken );
            return;
        } // string case
        else if ( singleToken[singleToken.size() - 1] == 'H' && singleToken != "AH" && singleToken != "BH" && singleToken != "CH" && singleToken != "DH" ) {
            intHashTable.resize( 100 );
            int i = 0;
            for ( i = 0; isxdigit( singleToken[i] ); i++ )
                ASCII += (int)singleToken[i];
            ASCII += 72;
            if ( i == singleToken.size() - 1 ) {
                TokenData intToken;
                while ( intHashTable[ASCII % 100 + indexPlus].token != "\0" && intHashTable[ASCII % 100 + indexPlus].token != singleToken ) {
                    indexPlus += 1;
                    if ( ( ASCII % 100 + indexPlus ) > 99 ) indexPlus = -( ASCII % 100 );
                } // hashing
                intToken.tokenType = 6;
                intToken.tableNum = ASCII % 100 + indexPlus;
                intToken.token = singleToken;
                intHashTable[ASCII % 100 + indexPlus] = intToken;
                currentLineTokens.push_back( intToken );
                return;
            } // push to current
        } // check if the token is an integer
        for ( int i = 0; i < singleToken.size(); i++ ) singleToken[i] = toupper( singleToken[i] );
        for ( int i = 0; i < opCodeTable.size(); i++ ) {
            if ( singleToken == opCodeTable[i].token ) {
                currentLineTokens.push_back( opCodeTable[i] );
                return;
            } // found
        } // opCode
        for ( int i = 0; i < pseudoInstructionsTable.size(); i++ ) {
            if ( singleToken == pseudoInstructionsTable[i].token ) {
                currentLineTokens.push_back( pseudoInstructionsTable[i] );
                return;
            } // found
        } // pseudoInstructions
        for ( int i = 0; i < registerOrAccumulatorTable.size(); i++ ) {
            if ( singleToken == registerOrAccumulatorTable[i].token ) {
                currentLineTokens.push_back( registerOrAccumulatorTable[i] );
                return;
            } // found
        } // registerOrAccumulator
        ASCII = 0;
        singleToken = save;
        symbolHashTable.resize( 100 );
        indexPlus = 0;
        TokenData symbolToken;
        for ( int i = 0; i < singleToken.size(); i++ )
            ASCII += (int)singleToken[i];
        while ( symbolHashTable[ASCII % 100 + indexPlus].token != "\0" && symbolHashTable[ASCII % 100 + indexPlus].token != singleToken ) {
            indexPlus += 1;
            if ( ( ASCII % 100 + indexPlus ) > 99 ) indexPlus = -( ASCII % 100 );
        } // hashing
        symbolToken.tokenType = 5;
        symbolToken.tableNum = ASCII % 100 + indexPlus;
        symbolToken.token = singleToken;
        symbolHashTable[ASCII % 100 + indexPlus] = symbolToken;
        currentLineTokens.push_back( symbolToken );
        return;
    } // Identify()
};

int main( int argc, const char* argv[] ) {
    int command = 0;
    bool continueOrNot = false;
    LexicalAnalysis Start;
    Start.inputTable();
    string inputFile;
    do {
        cout << "*****************************************" << endl; // welcome message
        cout << "***** Lexical Analysis              *****" << endl;
        cout << "***** 0 : Quit                      *****" << endl;
        cout << "***** 1 : Cut Token                 *****" << endl;
        cout << "*****************************************" << endl;
        cout << endl << "Please enter your choice :" << endl;
        cin >> command;
        cout << endl;
        if ( command == 0 ) { // bye :(((
            cout << "Bye :(((" << endl;
            return 0;
        } // quit
        else if ( command > 1 || command < 0 ) { // wrong command
            cout << "Error command! please enter an acceptable command :" << endl << endl;
            continueOrNot = true;
        } // wrong command
        else if ( command == 1 ) { // cut token
            bool function1Confirm = false;
            do {
                cout << "Please enter the file you want to process ( without \".txt\" ) or [0] to quit:" << endl;
                cin >> inputFile;
                cout << endl;
                if ( inputFile == "0" ) {
                    function1Confirm = true;
                    continueOrNot = true;
                } // quit
                else {
                    string inputFileName = inputFile + ".txt";
                    Input.open( inputFileName );
                    if ( Input.is_open() ) {
                        cout << "Processing " << inputFileName << "..." << endl << endl;
                        string outputFileName = inputFile + "_output.txt";
                        Output.open( outputFileName );
                        cout << "Output in " << outputFileName << "..." << endl << endl;
                        Start.Run();
                        cout << "Process complete :) Please check the output file..." << endl << endl;
                        function1Confirm = true;
                        continueOrNot = true;
                    } // opened
                    else cout << "*****  " << inputFileName << " does not exist!  *****" << endl; // no file
                } // open and sort
            } while ( !function1Confirm );
            inputFile = "0";
        } // mission 1: cut token
    } while ( continueOrNot ); // gogogo
    Input.close();
    Output.close();
} // main function
