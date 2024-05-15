#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

class Core {
public:
    bool exitEncountered = false;
    array<int, 32> registers;
    int pc;
    vector<string> program;
    bool halted;
    map<string, int> labelToAddress;

    struct PipelineRegister {
        string instruction;
        int rd;
        int rs1;
        int rs2;
        int imm;
    };
    array<PipelineRegister, 5> pipeline;

    bool dataHazard = false;
    int stalls = 0;

    map<string, int> instructionLatency = {
        {"ADD", 1},
        {"SUB", 1},
        {"MUL", 3},
        {"DIV", 4},
        {"LD", 2},
        {"ADDI", 1},
        {"JAL", 1},
        {"JALR", 1},
    };

    Core() : pc(0), registers({0}), halted(false) {}

    void execute(vector<int>& memory, bool dataForwarding) {
        for (int i = 4; i > 0; --i) {
            pipeline[i] = pipeline[i - 1];
        }

        if (pc < program.size()) {
            pipeline[0].instruction = program[pc++];
        } else {
            pipeline[0].instruction = "EXIT";
            exitEncountered = true;
        }

        decode(pipeline[1]);
        executeInstruction(pipeline[2], memory, dataForwarding);
        writeBack(pipeline[4]);
    }

    void decode(PipelineRegister& reg) {
        string instruction = reg.instruction;
        vector<string> parts;
        size_t spacePos = instruction.find(' ');
        string opcode = instruction.substr(0, spacePos);
        parts.push_back(opcode);

        while (spacePos != std::string::npos) {
            size_t nextSpacePos = instruction.find(' ', spacePos + 1);
            string operand = instruction.substr(spacePos + 1, nextSpacePos - spacePos - 1);
            parts.push_back(operand);
            spacePos = nextSpacePos;
        }

        if (opcode == "ADD" || opcode == "SUB" || opcode == "LD" || opcode == "JALR") {
            reg.rd = stoi(parts[1].substr(1));
            reg.rs1 = stoi(parts[2].substr(1));
            if (parts.size() > 3)
                reg.rs2 = stoi(parts[3].substr(1));
        } else if (opcode == "ADDI") {
            reg.rd = stoi(parts[1].substr(1));
            reg.rs1 = stoi(parts[2].substr(1));
            reg.imm = stoi(parts[3]);
        }
    }

    void executeInstruction(PipelineRegister& reg, vector<int>& memory, bool dataForwarding) {
        string instruction = reg.instruction;
        vector<string> parts;
        size_t spacePos = instruction.find(' ');
        string opcode = instruction.substr(0, spacePos);
        parts.push_back(opcode);

        while (spacePos != std::string::npos) {
            size_t nextSpacePos = instruction.find(' ', spacePos + 1);
            string operand = instruction.substr(spacePos + 1, nextSpacePos - spacePos - 1);
            parts.push_back(operand);
            spacePos = nextSpacePos;
        }

        if (instructionLatency.find(opcode) != instructionLatency.end()) {
            int latency = instructionLatency[opcode];

            if (!dataForwarding && dataHazard) {
                ++stalls;
                dataHazard = false;
            }

            if (opcode == "ADD" || opcode == "SUB" || opcode == "LD" || opcode == "JALR") {
                int rd = stoi(parts[1].substr(1));
                int rs1 = stoi(parts[2].substr(1));
                if (parts.size() > 3)
                    pipeline[2].rs2 = stoi(parts[3].substr(1));
                pipeline[2].rd = rd;
                pipeline[2].rs1 = rs1;
            } else if (opcode == "ADDI") {
                int rd = stoi(parts[1].substr(1));
                int rs1 = stoi(parts[2].substr(1));
                pipeline[2].rd = rd;
                pipeline[2].rs1 = rs1;
                pipeline[2].imm = stoi(parts[3].substr(0));
            }

            for (int i = 0; i < latency; ++i) {
                if (i != 0) {
                    pipeline[0].instruction = opcode;
                    executeInstruction(pipeline[2], memory, dataForwarding);
                }
            }
        }
    }

    void writeBack(PipelineRegister& reg) {
        if (reg.instruction.substr(0, 4) == "EXIT")
            return;

        string instruction = reg.instruction;
        vector<string> parts;
        size_t spacePos = instruction.find(' ');
        string opcode = instruction.substr(0, spacePos);
        parts.push_back(opcode);

        while (spacePos != std::string::npos) {
            size_t nextSpacePos = instruction.find(' ', spacePos + 1);
            string operand = instruction.substr(spacePos + 1, nextSpacePos - spacePos - 1);
            parts.push_back(operand);
            spacePos = nextSpacePos;
        }

        if (opcode == "ADD" || opcode == "SUB" || opcode == "LD" || opcode == "ADDI" || opcode == "JALR") {
            int rd = stoi(parts[1].substr(1));
            registers[rd] = reg.rd;
        }
    }
};

class Processor {
public:
    vector<int> memory;
    int clock;
    array<Core, 2> cores;
    bool dataForwarding = false;

    Processor() : clock(0), memory(4096, 0), cores({Core(), Core()}) {}

    void run() {
        while (!(cores[0].exitEncountered && cores[1].exitEncountered)) {
            for (int i = 0; i < 2; ++i) {
                cores[i].execute(memory, dataForwarding);
            }
            detectStalls();
            ++clock;
        }

        cout << "+============ After Run ============:" << endl;
        /*for (int i = 0; i < 2; ++i) {
            for (int reg : cores[i].registers) {
                cout << reg << ' ';
            }
            cout << endl;
        }*/
        cout << "Number of Stalls: " << cores[0].stalls + cores[1].stalls << endl;
        double totalInstructions = cores[0].pc + cores[1].pc;
        cout << "Instructions Per Cycle (IPC): " << fixed << setprecision(2) << (totalInstructions != 0 ? (totalInstructions / clock) : 0) << endl;
    }

    void detectStalls() {
        for (int i = 0; i < 2; ++i) {
            Core& core = cores[i];
            if (core.pipeline[1].instruction != "" && core.pipeline[2].instruction != "") {
                string rd = core.pipeline[2].instruction.substr(0, 3);
                string rs = core.pipeline[1].instruction.substr(0, 3); 
                if (rd == "ADD" || rd == "SUB" || rd == "LD" || rd == "ADDI" || rd == "JALR") {
                    if (rs == "ADD" || rs == "SUB" || rs == "LD" || rs == "ADDI" || rs == "JALR") {
                        core.dataHazard = true;
                    }
                }
            }
            if (core.dataHazard)
                ++core.stalls;
        }
    }
};

int main() {
    Processor sim;

    cout << "Do you want to enable data forwarding? (1/0): ";
    char choice;
    cin >> choice;
    if (toupper(choice) == '0') {
        sim.dataForwarding = true;
    }

    cout << "Enter the file name to be opened: ";
    string fileName;
    cin >> fileName;

    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "ERROR opening file" << endl;
        return -1;
    }

    string line;
    while (getline(file, line)) {
        sim.cores[0].program.push_back(line);
    }
    file.close();

    sim.run();

    return 0;
    }
