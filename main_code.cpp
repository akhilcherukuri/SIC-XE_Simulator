#include <iostream>
#include <stdbool.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <cstring>

using namespace std;

#define MAX_FILENAME_SIZE 128
#define ENABLE_CLI 0

// Error Codes

int ERR_INVALID_ARGS = -1;
int ERR_INVALID_INSTRUCTIONS_FILE = -2;
int ERR_INVALID_REGISTERS_FILE = -3;
int ERR_INVALID_INPUT_FILE = -4;
int ERR_INVALID_ASSEMBLER_DIRECTIVE_FILE = -5;
int SUCCESS = 0;

// Function Declarations

int cli__run_program();
void cli__main_menu();
int cli__show_file_contents(char *);
char *getCmdOption(char **begin, char **end, const string &option);
bool cmdOptionExists(char **begin, char **end, const string &option);

// Intialization

void fill_optab();
void fill_regtab();
void fill_adtab();

void load__registers_from_file();
void load__instructions_from_file();

// Pass 1 Assembly Function

void parse_sample_program_into_data_structure();
void pass_1_assembly();

bool check_validity();
int validate_arguments(char *);
int validate_opcode(char *);
int determine_format_type();
void assign_address();
void insert_in_symtab();
void increment_locctr();
int get_length_of_constant();

// Pass 2 Assembly Function

void pass_2_assembly();

// Global Data Structures

struct cli_data_s
{
    char assembler_directives_filename[MAX_FILENAME_SIZE];
    char input_filename[MAX_FILENAME_SIZE];
    char instructions_filename[MAX_FILENAME_SIZE];
    char registers_filename[MAX_FILENAME_SIZE];
    char machine_code_filename[MAX_FILENAME_SIZE];
    char object_code_filename[MAX_FILENAME_SIZE];
    int encryption_key;
};

union flags_u
{
    uint8_t flag;
    struct flags_s
    {
        uint8_t e : 1;
        uint8_t p : 1;
        uint8_t b : 1;
        uint8_t x : 1;
        uint8_t i : 1;
        uint8_t n : 1;
    };
};

struct instruction_data_s
{
    string label;
    string opcode;
    string operand;
    uint32_t instr_address;
    flags_u addressing_flags;
    bool is_machine_code_required;
};

// Global Variables

int LOCCTR;
instruction_data_s temp_instruction_data;
cli_data_s cli_data;
vector<instruction_data_s> inst_v;

// OPTAB
/*
* string - Opcode
* int - Instruction Format Number
* string - Machine Code
*/
unordered_map<string, pair<int, string>> OPTAB;

// REGTAB
/*
* string - Register Name
* int - Register Number
*/
unordered_map<string, int> REGTAB;

// ADTAB
/*
* string - Assembler Directive Name
* int - Assembler Directive Number
*/
unordered_map<string, int> ADTAB;

// SYMTAB
/*
* string - Label Name
* int - Address
*/
unordered_map<string, int> SYMTAB;

// Initialization Functions

void fill_optab()
{
    FILE *fp = fopen(cli_data.instructions_filename, "r");
    char opcode_line[100];
    while (fgets(opcode_line, sizeof(opcode_line), fp))
    {
        char opcode_name[10];
        int opcode_format;
        char opcode_num[10];
        string s1;
        string s2;
        pair<int, string> temp;
        stringstream ss1, ss2;

        sscanf(opcode_line, "%s %d %s", opcode_name, &opcode_format, opcode_num);

        ss1 << opcode_name;
        s1 = ss1.str();
        ss2 << opcode_num;
        s2 = ss2.str();
        temp.first = opcode_format;
        temp.second = s2;
        OPTAB[s1] = temp;
        cout << s1 << " " << temp.first << " " << temp.second << endl;
    }
    fclose(fp);
}

void fill_regtab()
{
    FILE *fp = fopen(cli_data.registers_filename, "r");
    char opcode_line[100];
    while (fgets(opcode_line, sizeof(opcode_line), fp))
    {
        char reg_name[10];
        int reg_num;
        string s1;
        stringstream ss1;

        sscanf(opcode_line, "%s %d", reg_name, &reg_num);

        ss1 << reg_name;
        s1 = ss1.str();

        REGTAB[s1] = reg_num;
        cout << s1 << " " << reg_num << endl;
    }
    fclose(fp);
}

void fill_adtab()
{
    FILE *fp = fopen(cli_data.assembler_directives_filename, "r");
    char ad_line[100];
    while (fgets(ad_line, sizeof(ad_line), fp))
    {
        char ad_name[10];
        int ad_num;
        string s1;
        stringstream ss1;

        sscanf(ad_line, "%s %d", ad_name, &ad_num);

        ss1 << ad_name;
        s1 = ss1.str();

        ADTAB[s1] = ad_num;
        cout << s1 << " " << ad_num << endl;
    }
    fclose(fp);
}

// Pass 1 Assembly Functions

// Validation Function Definitions

int get_length_of_constant()
{
    if (temp_instruction_data.operand[0] == 'C' && temp_instruction_data.operand[1] == '\'')
    {
        return (temp_instruction_data.operand.length() - 3);
    }
    else if (temp_instruction_data.operand[0] == 'X' && temp_instruction_data.operand[1] == '\'')
    {
        return ((temp_instruction_data.operand.length() - 3) / 2);
    }
    else
    {
        cout << "Error: Invalid Operand found in assembler directive" << endl;
        return -1;
    }
}

int validate_arguments(char *opcode_line)
{
    char label[20], opcode[20], operand[20];

    string strFromChar;
    strFromChar.append(opcode_line);
    istringstream ss(strFromChar);

    string word;

    int number_arg = 0;
    bool no_label = false;
    if (opcode_line[0] == ' ')
    {
        no_label = true;
        number_arg++;
    }

    int i = 0;
    string arr_instruction[3];
    // TODO: Handle case for assembler directives.
    while (ss >> word)
    {
        if (no_label && i == 0)
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

    if (number_arg >= 4)
    {
        return ERR_INVALID_ARGS;
    }

    temp_instruction_data.label = arr_instruction[0];
    temp_instruction_data.opcode = arr_instruction[1];
    temp_instruction_data.operand = arr_instruction[2];

    // cout << "Label " << temp_instruction_data.label << endl;
    // cout << "Opcode " << temp_instruction_data.opcode << endl;
    // cout << "Operand " << temp_instruction_data.operand << endl;

    return SUCCESS;
}

int validate_opcode(char *opcode_line)
{
    string temp_opcode;
    temp_opcode = temp_instruction_data.opcode;
    if (temp_instruction_data.opcode[0] == '+')
    {
        temp_opcode = &temp_instruction_data.opcode[1];
    }
    if (ADTAB.find(temp_opcode) != ADTAB.end())
    {
        temp_instruction_data.is_machine_code_required = false;
        if (temp_instruction_data.opcode == "BYTE")
        {
            temp_instruction_data.is_machine_code_required = true;
        }
        return 0;
    }
    else if (OPTAB.find(temp_opcode) != OPTAB.end())
    {
        temp_instruction_data.is_machine_code_required = true;
        return 0;
    }
    else
    {
        cout << temp_instruction_data.opcode << " "
             << "Invalid Opcode" << endl;
        return 1;
    }
}

bool check_validity(char *opcode_line)
{
    int err_code = validate_arguments(opcode_line);
    if (err_code == 0)
    {
        err_code = validate_opcode(opcode_line); // TODO: Implement validate_opcode()
        if (err_code == 0)
        {
            // valid instruction line
            return true;
        }
        else
        {
            cout << "Invalid instruction ocurred. Code: " << err_code << endl;
            return false;
        }
    }
    else
    {
        // TODO: Error code mapping and error log file.
        cout << "Error ocurred. Code: " << err_code << endl;
        return false;
    }
}

// Addressing Function Definitions

int determine_format_type()
{
    string temp_opcode;
    int number_of_bytes = 0;
    temp_opcode = temp_instruction_data.opcode;
    if (temp_instruction_data.is_machine_code_required == true)
    {
        if (temp_instruction_data.opcode[0] == '+')
        {
            temp_opcode = &temp_instruction_data.opcode[1];
        }
        pair<int, string> temp = OPTAB[temp_opcode];
        number_of_bytes = temp.first;
        // cout << "DEBUG 2 " << temp_instruction_data.opcode << " " << temp.first << " " << temp.second << endl;
        if ((temp_instruction_data.opcode[0] == '+') && (number_of_bytes == 3))
        {
            // cout << " DEBUG 1" << temp_instruction_data.opcode << endl;
            number_of_bytes = 4;
        }
        if (temp_instruction_data.opcode == "BYTE")
        {
            number_of_bytes = get_length_of_constant();
            if (number_of_bytes == -1)
            {
                return -1;
            }
        }
    }
    else
    {
        if (temp_instruction_data.opcode == "RESW")
        {
            number_of_bytes = 3 * stoi(temp_instruction_data.operand);
        }
        else if (temp_instruction_data.opcode == "RESB")
        {
            number_of_bytes = 1 * stoi(temp_instruction_data.operand);
        }
        else if (temp_instruction_data.opcode == "START" || temp_instruction_data.opcode == "END" || temp_instruction_data.opcode == "BASE")
        {
            number_of_bytes = 0;
        }
        else if (temp_instruction_data.opcode == "WORD")
        {
            number_of_bytes = 3;
        }

        else
        {
            cout << "Could not determine assembler directive" << endl;
        }
    }

    return number_of_bytes;
}

void increment_locctr()
{
    // cout << hex << LOCCTR << " " << temp_instruction_data.label << " " << temp_instruction_data.opcode << " " << temp_instruction_data.operand << endl;
    LOCCTR += determine_format_type();
}

void assign_address()
{
    // If START is not there initialize to 0
    temp_instruction_data.instr_address = 0;
    if (temp_instruction_data.opcode == "START")
    {
        LOCCTR = stoi(temp_instruction_data.operand);
        temp_instruction_data.instr_address = LOCCTR;
    }
}

void insert_in_symtab()
{
    if (temp_instruction_data.label != "NA" && temp_instruction_data.label != " " && temp_instruction_data.label != "")
    {
        SYMTAB[temp_instruction_data.label] = LOCCTR;
        cout << temp_instruction_data.label << " " << hex << SYMTAB[temp_instruction_data.label] << endl;
    }
}

void parse_sample_program_into_data_structure()
{
    FILE *fp = fopen("input_assembly_file.txt", "r");
    char opcode_line[100];
    if (fp != NULL)
    {
        memset(&temp_instruction_data, 0, sizeof(temp_instruction_data));
        while (fgets(opcode_line, sizeof(opcode_line), fp))
        {
            // temp_instruction_data = {0};
            if (check_validity(opcode_line))
            {
                assign_address();
                insert_in_symtab();
                increment_locctr();
                inst_v.push_back(temp_instruction_data);
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

// CLI Function Definitions

char *getCmdOption(char **begin, char **end, const string &option)
{
    char **itr = find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char **begin, char **end, const string &option)
{
    return find(begin, end, option) != end;
}

void display_title()
{
    cout << "  _____ _____ _______   ________                                 _     _           _____ _                 _       _              " << endl;
    cout << " / ____|_   _/ ____\" \" / /  ____|   /\"                          | |   | |         / ____(_)               | |     | |             " << endl;
    cout << "| (___   | || |     \" V /| |__     /  \"   ___ ___  ___ _ __ ___ | |__ | | ___ _ _| (___  _ _ __ ___  _   _| | __ _| |_ ___  _ __  " << endl;
    cout << " \"___ \"  | || |      > < |  __|   / /\" \" / __/ __|/ _ \" '_ ` _ \"| '_ \"| |/ _ \" '__\"___ \"| | '_ ` _ \"| | | | |/ _` | __/ _ \"| '__| " << endl;
    cout << " ____) |_| || |____ / . \"| |____ / ____ \\__ \"__ \"  __/ | | | | | |_) | |  __/ |  ____) | | | | | | | |_| | | (_| | || (_) | |    " << endl;
    cout << "|_____/|_____\"_____/_/ \"_\"______/_/    \"_\"___/___/\"___|_| |_| |_|_.__/|_|___|_| |_____/|_|_| |_| |_|\"__,_|_|\"__,_|\"__\"___/|_|    " << endl;
}

void display_help()
{
    cout << R"( - Input filename -> Input only .asm file)" << endl;
    cout << R"( - Machine code filename -> User Input of output filename for Machine code)" << endl;
    cout << R"( - Object code filename -> User Input of output filename for Object code)" << endl;
    cout << R"( - Do you want to encrypt the Output files? [y/n]: )" << endl;
    cout << R"( -- 'n' -> default option when user doesn’t give anything)" << endl;
    cout << R"( -- 'y' -> prompted to enter 6 digit key)" << endl;
}

void cli__main_menu()
{
#if ENABLE_CLI == 1
    int cli_user_choice = 0, ret_status = SUCCESS;
    char filename[MAX_FILENAME_SIZE];

    display_title();
    while (cli_user_choice <= 3)
    {
        cout << endl;
        cout << "---------MENU---------\n";
        cout << "1. Begin Assembler Processing\n";
        cout << "2. Show Instructions\n";
        cout << "3. Show Registers\n";
        cout << "4. Quit\n"
             << flush;
        cout << "\nEnter your choice: ";

        cin >> cli_user_choice;
        if (cli_user_choice >= 4 || cli_user_choice <= 0)
        {
            cout << endl
                 << "Exiting the program.";
            exit(0);
        }
        switch (cli_user_choice)
        {
        case 1:
            ret_status = cli__run_program();
            if (ret_status != SUCCESS)
                continue;
            break;
        case 2:
            cout << "\nEnter Instruction File Name: ";
            cin >> filename;
            ret_status = cli__show_file_contents(filename);
            if (ret_status != SUCCESS)
                continue;
            break;
        case 3:
            cout << "\nEnter Register File Name: ";
            cin >> filename;
            ret_status = cli__show_file_contents(filename);
            if (ret_status != SUCCESS)
                continue;
            break;
        default:
            cout << endl
                 << "Exiting the program.";
            exit(0);
        }
    }
#elif ENABLE_CLI == 0
    strcpy(cli_data.input_filename, "input_assembly_file.txt");
    strcpy(cli_data.instructions_filename, "instructions.txt");
    strcpy(cli_data.registers_filename, "registers.txt");
    strcpy(cli_data.assembler_directives_filename, "assembler_directives.txt");
    strcpy(cli_data.machine_code_filename, "mcode.txt");
    strcpy(cli_data.object_code_filename, "ocode.txt");
    cli_data.encryption_key = 0;

    cout << endl;
    cout << "------------OPTAB-------------" << endl;
    fill_optab();
    cout << "------------REGTAB-------------" << endl;
    fill_regtab();
    cout << "------------ADTAB-------------" << endl;
    fill_adtab();
    cout << "------------SYMTAB-------------" << endl;

    pass_1_assembly();
#endif
}

int get_input_files()
{
    return SUCCESS;
}

int cli__run_program()
{
    char encrypt_output_file_option = 'n';
    // get_input_files();

    ifstream input_fp, instruction_fp, register_fp, assembler_directive_fp;

    // Input Assembly File
    cout << "\nEnter Input File Name: ";
    cin >> cli_data.input_filename;
    input_fp.open((cli_data.input_filename));

    if (input_fp.fail())
    {
        cout << "\nError: Input File Not Found." << endl;
        return ERR_INVALID_INPUT_FILE;
    }

    // Instruction File
    cout << "\nEnter Instruction File Name: ";
    cin >> cli_data.instructions_filename;
    instruction_fp.open((cli_data.instructions_filename));

    if (instruction_fp.fail())
    {
        cout << "\nError: Instruction File Not Found." << endl;
        return ERR_INVALID_INSTRUCTIONS_FILE;
    }

    // Register File
    cout << "\nRegister File Name: ";
    cin >> cli_data.registers_filename;
    register_fp.open((cli_data.registers_filename));

    if (register_fp.fail())
    {
        cout << "\nError: Register File Not Found." << endl;
        return ERR_INVALID_REGISTERS_FILE;
    }

    // Assembler Directive File
    cout << "\nAssembler Directive File Name: ";
    cin >> cli_data.assembler_directives_filename;
    assembler_directive_fp.open((cli_data.assembler_directives_filename));

    if (assembler_directive_fp.fail())
    {
        cout << "\nError: Assembler Directive File Not Found." << endl;
        return ERR_INVALID_ASSEMBLER_DIRECTIVE_FILE;
    }

    cout << "\nEnter Machine Code Filename: ";
    cin >> cli_data.machine_code_filename;

    cout << "\nEnter Object Code Filename: ";
    cin >> cli_data.object_code_filename;

    cout << "\nDo you want to encrypt the Output File? [y/n]: ";
    cin >> encrypt_output_file_option;

    if (encrypt_output_file_option == 'y')
    {
        cout << "\nEnter 6-digit Encryption Key: ";
        cin >> cli_data.encryption_key;
    }
    else
    {
        cout << "\nEncryption disabled.\n";
        cli_data.encryption_key = 0;
    }

    // TODO: Check if assembler program starts with START and ends with END
    cout << "------------OPTAB-------------" << endl;
    fill_optab();
    cout << "------------REGTAB-------------" << endl;
    fill_regtab();
    cout << "------------ADTAB-------------" << endl;
    fill_adtab();
    // cout << ADTAB["END"] << endl;

    pass_1_assembly();

    return SUCCESS;
}

int cli__show_file_contents(char *filename)
{
    string instructions_line;
    ifstream input_instructions_file;
    input_instructions_file.open(filename);
    if (input_instructions_file.is_open())
    {
        while (getline(input_instructions_file, instructions_line))
        {
            cout << instructions_line << endl;
        }
        input_instructions_file.close();
    }
    else
    {
        cout << "Error: Unable to open " << filename << " file." << endl;
        return ERR_INVALID_INPUT_FILE;
    }
    input_instructions_file.close();
    return SUCCESS;
}

int main(int argc, char *argv[])
{
    if (cmdOptionExists(argv, argv + argc, "-h"))
    {
        display_help();
    }
    else
    {
        cli__main_menu();
        return SUCCESS;
    }
}