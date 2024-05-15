#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <sstream>
using namespace std;

class Core {
public:
    bool exitEncountered=false;
    array<int, 32> registers;
    int pc;
    vector<string> program;
    bool halted;
    map<string, int> labelToAddress;

    Core() : pc(0), registers({0}), halted(false) {}

    void execute(vector<int>& memory) {
        vector<string> parts;
        string opcode;

        size_t spacePos = program[pc].find(' ');
        opcode = program[pc].substr(0, spacePos);
        parts.push_back(opcode);

        if (program[pc].back() == ':') {
            string label = program[pc].substr(0, program[pc].size() - 1);
            labelToAddress[label] = pc;
            pc++;
            return;
        }

        if (!program[pc].empty() && program[pc][0] == '#') {
            pc++;
            return;
        }

        while (spacePos != std::string::npos) {
            size_t nextSpacePos = program[pc].find(' ', spacePos + 1);
            string operand = program[pc].substr(spacePos + 1, nextSpacePos - spacePos - 1);
            parts.push_back(operand);
            spacePos = nextSpacePos;
        }

        if (opcode == "ADD") {
            int rd = stoi(parts[1].substr(1));
            int rs1 = stoi(parts[2].substr(1));
            int rs2 = stoi(parts[3].substr(1));
            registers[rd] = registers[rs1] + registers[rs2];
        } else if (opcode == "SUB") {
            int rd = stoi(parts[1].substr(1));
            int rs1 = stoi(parts[2].substr(1));
            int rs2 = stoi(parts[3].substr(1));
            registers[rd] = registers[rs1] - registers[rs2];
        } else if (opcode == "LD") {
            int rd = stoi(parts[1].substr(1));
            int location = stoi(parts[2]);
            registers[rd] = memory[location];
        } else if (opcode == "LW") {
            int rd = stoi(parts[1].substr(1));
            string offsetStr = parts[2].substr(0, parts[2].find("("));
            int offset = stoi(offsetStr);
            int p = stoi(parts[2].substr(parts[2].find("x") + 1));
            int location = registers[p] + (offset / 4);
            registers[rd] = memory[location];
        } else if (opcode == "JAL") {
            int rd = stoi(parts[1].substr(1));
            int offset = stoi(parts[2]);
            registers[rd] = pc + 1;
            pc += offset;
            return;
        } else if (opcode == "ADDI") {
            int rd = stoi(parts[1].substr(1));
            int rs1 =stoi(parts[2].substr(1));
            int imm =stoi(parts[3]);
            registers[rd] = registers[rs1] + imm;
        } else if (opcode == "LI") {
            int rd = stoi(parts[1].substr(1));
            int imm = stoi(parts[2]);
            registers[rd] = imm;
        } else if (opcode == "J") {
            std::string label = parts[1].substr(0);
            int targetAddress = labelToAddress[label];
            pc = targetAddress;
            return;
        } else if (opcode == "BNE") {
            int rs1 = stoi(parts[1].substr(1));
            int rs2 = stoi(parts[2].substr(1));
            std::string label = parts[3].substr(0);
            if (registers[rs1] != registers[rs2]) {
                int targetAddress = labelToAddress[label];
                pc = targetAddress;
                return;
            }
        } else if (opcode == "BEQ") {
            int rs1 = stoi(parts[1].substr(1));
            int rs2 = stoi(parts[2].substr(1));
            std::string label = parts[3].substr(0);
            if (registers[rs1] == registers[rs2]) {
                int targetAddress = labelToAddress[label];
                pc = targetAddress;
                return;
            }
        } else if (opcode == "BLE") {
            int rs1 = stoi(parts[1].substr(1));
            int rs2 = stoi(parts[2].substr(1));
            string label = parts[3].substr(0);
            if (registers[rs1] <= registers[rs2]) {
                int targetAddress = labelToAddress[label];
                pc = targetAddress;
                return;
            }
        } else if (opcode == "SW") {
            int rs = stoi(parts[1].substr(1));
            string offsetStr = parts[2].substr(0, parts[2].find("("));
            int offset = stoi(offsetStr);
            int p = stoi(parts[2].substr(parts[2].find("x") + 1));
            int location = registers[p] + (offset / 4);
            memory[location] = registers[rs];
        }
        pc++;
    }
};

class Processor {
public:
    vector<int> memory;
    int clock;
    array<Core, 2> cores;

    Processor() : clock(0), memory(4096, 0), cores({Core(), Core()}) {}

    void run() {
        bool exitEncountered = false; 

        while ((cores[0].exitEncountered == false) || (cores[1].exitEncountered == false)) {
            for (int i = 0; i < 2; ++i) {
                if (!cores[i].exitEncountered)
                    cores[i].execute(memory);
                cout << "Core: " << i << " " << cores[i].program[cores[i].pc] << " PC: " << cores[i].pc << endl;
                if (cores[i].program[cores[i].pc] == "EXIT") {
                    cores[i].exitEncountered = true;
                }
            }
        }
    }
};

int main() {
    Processor sim;

    int wordmemory1 = 500;
    int stringmemory1 = 1000;
    int wordmemory2 = 1500;
    int stringmemory2 = 2000;

    ifstream file1("test1.txt");
    if (!file1.is_open()) {
        cerr << "ERROR opening in File" << std::endl;
        return -1;
    }
    string line1;
    while (getline(file1, line1)) {
        if (!line1.empty() && line1[0] == '#') {
            continue;
        }

        if (line1.find(".word") != std::string::npos) {
            size_t pos = line1.find(".word");
            string numbersString = line1.substr(pos + 6);
            stringstream ss(numbersString);
            int num;
            while (ss >> num) {
                sim.memory[wordmemory1++] = num;
            }
        } else if (line1.find(".string") != std::string::npos) {
            size_t pos_start = line1.find("\""); 
            size_t pos_end = line1.find_last_of("\""); 
            if (pos_start != string::npos && pos_end != string::npos) {
                string stringContent = line1.substr(pos_start + 1, pos_end - pos_start - 1); 
                for (char c : stringContent) {
                    sim.memory[stringmemory1++] = c; 
                }
                sim.memory[stringmemory1++] = '\0'; 
            }
        } else if (line1.back() == ':') {
            sim.cores[0].labelToAddress[line1.substr(0, line1.size() - 1)] = sim.cores[0].program.size();
        } else {
            sim.cores[0].program.push_back(line1);
        }
    } 
    file1.close();

    ifstream file2("test2.txt");
    if (!file2.is_open()) {
        cerr << "ERROR opening in File" << std::endl;
        return -1;
    }
    string line2;
    while (getline(file2, line2)) {
        if (!line2.empty() && line2[0] == '#') {
            continue;
        }

        if (line2.find(".word") != string::npos) { 
            size_t pos = line2.find(".word");
            string numbersString = line2.substr(pos + 6); 
            stringstream ss(numbersString);
            int num;
            while (ss >> num) {
                sim.memory[wordmemory2++] = num; 
            }
        } else if (line2.find(".string") != string::npos) { 
            size_t pos_start = line2.find("\""); 
            size_t pos_end = line2.find_last_of("\""); 
            if (pos_start != string::npos && pos_end != string::npos) {
                string stringContent = line2.substr(pos_start + 1, pos_end - pos_start - 1); 
                for (char c : stringContent) {
                    sim.memory[stringmemory2++] = c; 
                }
                sim.memory[stringmemory2++] = '\0'; 
            }
        } else if (line2.back() == ':') { 
            sim.cores[1].labelToAddress[line2.substr(0, line2.size() - 1)] = sim.cores[1].program.size(); 
        } else {
            sim.cores[1].program.push_back(line2);
        }
    }
    file2.close();

    sim.run(); 

    cout << "+============ After Run ============:" << endl;
    for (int i = 0; i < 2; ++i) {
        for (int reg : sim.cores[i].registers) {
            cout << reg << ' ';
        }
        cout << endl;
    }

    return 0;
}
