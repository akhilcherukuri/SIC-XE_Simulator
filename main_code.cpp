#include <bits/stdc++.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void cli__init();
// Intialization

void fill_optab();
void fill_regtab();

void load__registers_from_file();
void load__instructions_from_file();

// Pass 1 Assembly Function Declarations

void parse_sample_program_into_data_structure();
void pass_1_assembly();

bool check_validity();
bool validate_three_arguments();
bool validate_opcode();

// Pass 2 Assembly Function Declarations

void pass_2_assembly();

// Data Structures

struct instruction_data_s
{
    string label;
    string opcode;
    string operands;
    uint32_t instr_address;
    union flags_s
    {
        uint8_t e : 1;
        uint8_t p : 1;
        uint8_t b : 1;
        uint8_t x : 1;
        uint8_t i : 1;
        uint8_t n : 1;
    };
};

vector<instruction_data_s> inst_v;

// OPTAB
/* 
* string - opcode
* int - format number
* string - machine code  
*/
unordered_map<string, pair<int, string>> OPTAB;
// OPTAB
/* 
* string - REGISTER NAME
* int - REGISTER NUMBER
*/
unordered_map<string, int> REGTAB;

// Initialization

void fill_optab()
{
    FILE *fp = fopen("instructions.txt", "r");
    char opcode_line[100];
    while (fgets(opcode_line, sizeof(opcode_line), fp))
    {
        char a[10];
        int b;
        char c[10];
        string s1;
        string s2;
        pair<int, string> temp;
        stringstream ss1, ss2;

        sscanf(opcode_line, "%s %d %s", a, &b, c);

        ss1 << a;
        s1 = ss1.str();
        ss2 << c;
        s2 = ss2.str();
        temp.first = b;
        temp.second = s2;
        OPTAB[s1] = temp;
        cout << s1 << " " << temp.first << " " << temp.second << endl;
    }
    fclose(fp);
}

void fill_regtab()
{
    FILE *fp = fopen("registers.txt", "r");
    char opcode_line[100];
    while (fgets(opcode_line, sizeof(opcode_line), fp))
    {
        char a[10];
        int b;
        string s1;
        stringstream ss1;

        sscanf(opcode_line, "%s %d", a, &b);

        ss1 << a;
        s1 = ss1.str();

        REGTAB[s1] = b;
        cout << s1 << " " << b << endl;
    }
    fclose(fp);
}

// // Pass 1 Assembly

// // Pass 1 Assembly Function Definitions
// bool check_validity()
// {
//     if (validate_three_arguments())
//     {
//         if (validate_opcode())
//         {
//         }
//     }
// }

// void parse_sample_program_into_data_structure() // Include in pass 1
// {
//     FILE *fp = fopen("input_assembly_file.txt", "r");
//     char opcode_line[100];
//     while (fgets(opcode_line, sizeof(opcode_line), fp))
//     {
//         if (check_validity())
//         {
//             assign_address();
//         }
//     }
// }

// void pass_1_assembly()
// {
//     parse_sample_program_into_data_structure();
// }

int main()
{
    fill_optab();
    fill_regtab();
    cout << REGTAB["X"] << endl;
    cout << REGTAB["A"] << endl;
    cout << REGTAB["F"] << endl;

    // parse_sample_program_into_data_structure();
}
