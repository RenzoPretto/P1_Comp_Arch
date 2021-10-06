using namespace std;
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <iostream>

int loc = 260, dataLoc = 0, dataSize = 0, cycle = 1;
map<int, int> memLoc;
ofstream disassembly, simulation;

int binTwosComplementToSignedDecimal(string binary, int significantBits)
{
	int power = pow(2, significantBits - 1);
	int sum = 0;
	int i;

	for (i = 0; i < significantBits; ++i)
	{
		if (i == 0 && binary.at(i) != '0')
		{
			sum = power * -1;
		}
		else
		{
			sum += (binary.at(i) - '0') * power;//The -0 is needed
		}
		power /= 2;
	}

	return -sum;
}

class instruction;

map<int, instruction> instructLoc;

class instruction {

public:

	bool isBreak = false;
	string println = "", cat = "", opcode = "";
	int immdt_val = 0, src1 = 0, src2 = 0, offset = 0, instruct_index = 0, base = 0, dest = 0;

	instruction() {

	}

	instruction(string line) {
		cat = line.substr(0, 3);
		opcode = line.substr(3, 3);
		if (cat == "000") {
			if (opcode == "000") {
				instruct_index = 4 * stoi(line.substr(6, 32), nullptr, 2);
				//loc = instruct_index;
				println += "J #" + to_string(instruct_index);
			}
			else if (opcode == "001") {
				src1 = stoi(line.substr(6, 5), nullptr, 2);
				src2 = stoi(line.substr(11, 5), nullptr, 2);
				offset = 4 * stoi(line.substr(16, 16), nullptr, 2);
				println += "BEQ R" + to_string(src1) + ", R" + to_string(src2) + ", #" + to_string(offset);
			}
			else if (opcode == "010") {
				src1 = stoi(line.substr(6, 5), nullptr, 2);
				src2 = stoi(line.substr(11, 5), nullptr, 2);
				offset = 4 * stoi(line.substr(16, 16), nullptr, 2);
				println += "BNE R" + to_string(src1) + ", R" + to_string(src2) + ", #" + to_string(offset);
			}
			else if (opcode == "011") {
				src1 = stoi(line.substr(6, 5), nullptr, 2);
				offset = 4 * stoi(line.substr(16, 16), nullptr, 2);
				println += "BGTZ R" + to_string(src1) + ", #" + to_string(offset);
			}
			else if (opcode == "100") {
				base = stoi(line.substr(6, 5), nullptr, 2);
				src1 = stoi(line.substr(11, 5), nullptr, 2);
				offset = stoi(line.substr(16, 16), nullptr, 2);
				println += "SW R" + to_string(src1) + ", " + to_string(offset) + "(R" + to_string(base) + ")";
			}
			else if (opcode == "101") {
				base = stoi(line.substr(6, 5), nullptr, 2);
				src1 = stoi(line.substr(11, 5), nullptr, 2);
				offset = stoi(line.substr(16, 16), nullptr, 2);
				println += "LW R" + to_string(src1) + ", " + to_string(offset) + "(R" + to_string(base) + ")";
			}
			else if (opcode == "110") {
				println += "BREAK";
				isBreak = true;
			}
		}
		else if (cat == "001") {
			if (opcode == "000") {
				dest = stoi(line.substr(6, 5), nullptr, 2);
				src1 = stoi(line.substr(11, 5), nullptr, 2);
				src2 = stoi(line.substr(16, 5), nullptr, 2);
				println += "ADD R" + to_string(dest) + ", R" + to_string(src1) + ", R" + to_string(src2);
			}
			else if (opcode == "001") {
				dest = stoi(line.substr(6, 5), nullptr, 2);
				src1 = stoi(line.substr(11, 5), nullptr, 2);
				src2 = stoi(line.substr(16, 5), nullptr, 2);
				println += "SUB R" + to_string(dest) + ", R" + to_string(src1) + ", R" + to_string(src2);
			}
			else if (opcode == "010") {
				dest = stoi(line.substr(6, 5), nullptr, 2);
				src1 = stoi(line.substr(11, 5), nullptr, 2);
				src2 = stoi(line.substr(16, 5), nullptr, 2);
				println += "AND R" + to_string(dest) + ", R" + to_string(src1) + ", R" + to_string(src2);
			}
			else if (opcode == "011") {
				dest = stoi(line.substr(6, 5), nullptr, 2);
				src1 = stoi(line.substr(11, 5), nullptr, 2);
				src2 = stoi(line.substr(16, 5), nullptr, 2);
				println += "OR R" + to_string(dest) + ", R" + to_string(src1) + ", R" + to_string(src2);
			}
			else if (opcode == "100") {
				dest = stoi(line.substr(6, 5), nullptr, 2);
				src1 = stoi(line.substr(11, 5), nullptr, 2);
				src2 = stoi(line.substr(16, 5), nullptr, 2);
				println += "SRL R" + to_string(dest) + ", R" + to_string(src1) + ", R" + to_string(src2);
			}
			else if (opcode == "101") {
				dest = stoi(line.substr(6, 5), nullptr, 2);
				src1 = stoi(line.substr(11, 5), nullptr, 2);
				src2 = stoi(line.substr(16, 5), nullptr, 2);
				println += "SRA R" + to_string(dest) + ", R" + to_string(src1) + ", R" + to_string(src2);
			}
			else if (opcode == "110") {
				dest = stoi(line.substr(6, 5), nullptr, 2);
				src1 = stoi(line.substr(11, 5), nullptr, 2);
				src2 = stoi(line.substr(16, 5), nullptr, 2);
				println += "MUL R" + to_string(dest) + ", R" + to_string(src1) + ", R" + to_string(src2);
			}
		}
		else if (cat == "010") {
			dest = stoi(line.substr(6, 5), nullptr, 2);
			src1 = stoi(line.substr(11, 5), nullptr, 2);
			immdt_val = stoi(line.substr(16, 16), nullptr, 2);
			if (opcode == "000") {
				println += "ADDI R" + to_string(dest) + ", R" + to_string(src1) + ", #" + to_string(immdt_val);
			}
			else if (opcode == "001") {
				println += "ANDI R" + to_string(dest) + ", R" + to_string(src1) + ", #" + to_string(immdt_val);
			}
			else if (opcode == "010") {
				println += "ORI R" + to_string(dest) + ", R" + to_string(src1) + ", #" + to_string(immdt_val);
			}
		}
	};

	void execute() {
		if (cat == "000") {
			if (opcode == "000") {
				instructLoc[loc].getData();
				loc = instruct_index;
				instructLoc[loc].execute();
			}
			else if (opcode == "001") {
				if (memLoc[src1] == memLoc[src2]) {
					instructLoc[loc].getData();
					loc = loc + offset + 4;
					instructLoc[loc].execute();
				}
			}
			else if (opcode == "010") {
				if (memLoc[src1] != memLoc[src2]) {
					instructLoc[loc].getData();
					loc = loc + offset + 4;
					instructLoc[loc].execute();
				}
			}
			else if (opcode == "011") {
				if (memLoc[src1] > 0) {
					instructLoc[loc].getData();
					loc = loc + offset + 4;
					instructLoc[loc].execute();
				}
			}
			else if (opcode == "100") {
				memLoc[memLoc[base] + offset] = memLoc[src1];
			}
			else if (opcode == "101") {
				memLoc[src1] = memLoc[memLoc[base] + offset];
			}
			else if (opcode == "110") {
				isBreak = true;
			}
		}
		else if (cat == "001") {
			if (opcode == "000") {
				memLoc[dest] = memLoc[src1] + memLoc[src2];
			}
			else if (opcode == "001") {
				memLoc[dest] = memLoc[src1] - memLoc[src2];
			}
			else if (opcode == "010") {
				memLoc[dest] = memLoc[src1] & memLoc[src2];
			}
			else if (opcode == "011") {
				memLoc[dest] = memLoc[src1] | memLoc[src2];
			}
			else if (opcode == "100") {
				memLoc[dest] = (unsigned int)memLoc[src1] >> memLoc[src2];
			}
			else if (opcode == "101") {
				memLoc[dest] = memLoc[src1] >> memLoc[src2];
			}
			else if (opcode == "110") {
				memLoc[dest] = memLoc[src1] * memLoc[src2];
			}
		}
		else if (cat == "010") {
			if (opcode == "000") {
				memLoc[dest] = memLoc[src1] + immdt_val;
			}
			else if (opcode == "001") {
				memLoc[dest] = memLoc[src1] & immdt_val;
			}
			else if (opcode == "010") {
				memLoc[dest] = memLoc[src1] | immdt_val;
			}
		}
	}

	void getData() {
		simulation << "--------------------" << endl;
		simulation << "Cycle " << cycle << ":\t" << loc << "\t" << println << endl;
		simulation << "\nRegisters" << endl;
		for (int i = 0; i < 4; i++) {
			if (i * 8 < 10) {
				simulation << "R0" << i * 8 << ":";
			}
			else {
				simulation << "R" << i * 8 << ":";
			}
			for (int j = 0; j < 8; j++) {
				simulation << "\t" << memLoc[(8 * i) + j];
			}
			simulation << "\n";
		}
		simulation << "\nData" << endl;
		int n = (dataSize / 8);
		for (int i = 0; i < n; i++) {
			simulation << dataLoc + 32 * i << ":";
			for (int j = 0; j < 8; j++) {
				simulation << "\t" << memLoc[dataLoc + (i * 32) + (j * 4)];
			}
			simulation << endl;
		}
		cycle++;
		simulation << "\n";
	}

	string getLine() {
		return println;
	}
};

int main(int argc, char** argv) {
	std::ifstream infile(argv[1]);
	disassembly.open("disassembly.txt");
	simulation.open("simulation.txt");
	string line;
	vector<instruction> instrucs;

	while (getline(infile, line)) { //Load instructions
		instruction new_instruc = instruction(line);
		instructLoc[loc] = new_instruc;
		disassembly << line << "\t" << loc << "\t" << instructLoc[loc].println << endl;
		loc += 4;
		if (new_instruc.isBreak) {
			break;
		}
	}
	dataLoc = loc;
	while (getline(infile, line)) { //Load into memory
		memLoc[loc] = binTwosComplementToSignedDecimal(line, 32);
		disassembly << line << "\t" << loc << "\t" << memLoc[loc] << endl;
		loc += 4;
		dataSize++;
	}
	loc = 256;
	while (!instructLoc[loc].isBreak) {
		loc += 4;
		instructLoc[loc].execute();
		instructLoc[loc].getData();
	}

}