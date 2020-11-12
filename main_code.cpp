#include <bits/stdc++.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// Error Codes
int ERR_INVALID_ARGS = -1;
int SUCCESS = 0;

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
int validate_arguments(char *);
int validate_opcode(char *);

// Pass 2 Assembly Function Declarations

void pass_2_assembly();

// Data Structures
struct temp_line_s
{
    string label;
    string opcode;
    string operand;
};

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

temp_line_s temp_line;

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

// Pass 1 Assembly

// Pass 1 Assembly Function Definitions
int validate_arguments(char * opcode_line)
{
    char label[20], opcode[20], operand[20];

    string strFromChar;
    strFromChar.append(opcode_line);
    istringstream ss(strFromChar);

	string word;

	int number_arg = 0;
    bool no_label = false;
    if(opcode_line[0] == ' ')
    {
        no_label = true;
        number_arg++;
    }
    
    int i = 0;
    string arr_instruction[3];
    // TODO: Handle case for assembler directives. 
    while (ss >> word) 
	{
        if(no_label && i == 0)
        {
            arr_instruction[0] = "NA"; // Label
            arr_instruction[1] = word; // Opcode
            i += 2;
            continue;
        }
        arr_instruction[i] = word;
        i++;
        number_arg++;
	}
    // TODO: Add debug macros
    // cout << "Number of ARGS: " << number_arg << endl;

    if (number_arg >= 4){
        return ERR_INVALID_ARGS;
    }

    temp_line.label = arr_instruction[0];
    temp_line.opcode = arr_instruction[1];
    temp_line.operand = arr_instruction[2];

    // cout << "Label " << temp_line.label << endl;
    // cout << "Opcode " << temp_line.opcode << endl;
    // cout << "Operand " << temp_line.operand << endl;

    return SUCCESS;
}

bool check_validity(char *opcode_line)
{
    int err_code = validate_arguments(opcode_line);
    if (err_code == 0)
    {
        // err_code = validate_opcode(opcode_line);  // TODO: Implement validate_opcode()
        if (err_code == 0)
        {
            // valid instruction line
        }
        else
        {
            cout << "Invalid instruction ocurred. Code: " << err_code << endl;
        }
    }
    else
    {
        // TODO: Error code mapping and error log file.
        cout << "Error ocurred. Code: " << err_code << endl;
    }
}

void parse_sample_program_into_data_structure() // Include in pass 1
{
    FILE *fp = fopen("input_assembly_file.txt", "r");
    char opcode_line[100];
    if(fp != NULL)
    {
        while (fgets(opcode_line, sizeof(opcode_line), fp))
        {
            if (check_validity(opcode_line))
            {
                //assign_address();
            } // TODO: Handle error validity case
        }
        fclose(fp);
    }
    else
    {
        cout << "Error opening the file." << endl;
    }
}

void pass_1_assembly()
{
    parse_sample_program_into_data_structure();
}

int main()
{
    // TODO: Check if assembler program starts with START and ends with END
    fill_optab();
    fill_regtab();
    cout << REGTAB["X"] << endl;
    cout << REGTAB["A"] << endl;
    cout << REGTAB["F"] << endl;

    pass_1_assembly();

    return 0;
}
