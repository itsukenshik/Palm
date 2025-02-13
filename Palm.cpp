
#include <stack>
#include <forward_list>
#include "other/preprocess.h"
#include "other/native.h"
#include "other/analyze.h"
#include "other/native.h"
using namespace std;

const char SpecialSymbols[] = "=<>&|!+-*/^%(),[]:\'~";
constexpr int AdditionalEqualitySignSymbols = 11;
constexpr int DoubleSignSymbols = 8;

unsigned char FindSpecialSymbol(char* String, int& At, const char* SymbolsFound) {
	for (unsigned char i = 0; SpecialSymbols[i]; i++) {
		if (String[At] == SymbolsFound[i]) return i;
	}
	return UCHAR_MAX;
}

vector<LineElement> splitLine(char* String) {
	vector<LineElement>* TheObjectWeReturn = new vector<LineElement>;
	string Temp;
	int pos = 0;
	int i = 0;
	auto add{ [&]() {
		TheObjectWeReturn->push_back(LineElement(Temp, pos)); } };
	auto clearTemp{ [&](bool isSign = false) {if (!Temp.empty()) {
		pos = i - Temp.size() + isSign;
		add();
		Temp.clear(); }}};
	bool isString = false;
	for (; String[i] != '\0'; i++) {
		auto SignNumber = FindSpecialSymbol(String, i, SpecialSymbols);
		if (!isString) {
			if (isspace(String[i])) clearTemp();
			else
				if (SignNumber != UCHAR_MAX) {
					clearTemp();
					Temp = String[i];
					if (SignNumber == '\xf') { // '\''
						isString = true;
						clearTemp(); continue;
					}
					//searching for a long operator
					if (String[i + 1] == '=' && SignNumber < AdditionalEqualitySignSymbols
						|| String[i + 1] == Temp.back() && SignNumber < DoubleSignSymbols)
						Temp.push_back(String[++i]);
					// we do not work with double exclamation sign (!!)
					if (Temp != "!!")
						clearTemp(true);
					else
						Temp.clear();
				}
				else if (String[i] == '#') break;
				else Temp.push_back(String[i]);
		}
		else {
			bool SignNumberQuote = SignNumber == '\xf';
			if (SignNumberQuote) {
				isString = false;
				clearTemp();
			}
			Temp.push_back(String[i]);
			if (SignNumberQuote) {
				clearTemp();
			}
		}
	}
	clearTemp();
	return move(*TheObjectWeReturn);
}


constexpr bool isDebug = true;
int main(int argc, char* argv[])
{
	// adding address functions
	{
		string Type;
		Type.push_back(char(ByteCode::Int) - 1);
		Type.push_back(char(ByteCode::adr));
		string Name;
		for (int i = 6; i < 9; ++i) {
			Type[0]++;
			Name = Keywords[i];
			tempDataChunk* byteadr = new tempDataChunk(Type, Name);
			GLOBAL_FUNCTION_LIST.push(byteadr);
		}

	}


	string in, out;

	if (!isDebug) {
		if (argc == 1) {
			std::cout << "\n    #####\n    #####\n###################\n### Palm v0.1 ###\n###################\n    #####\n    #####\n\nUsage syntax: palm [infile] [outfile]\n";
			return 0;
		}

		if (argc > 1)
			in = argv[1];

		if (argc > 2)
			out = argv[2];
		else {
			std::cout << "Output file not specified";
			return -7;
		}
	}
	else {
		in = "file.plm";
		out = "out.bin";
	}

	ifstream iffile(in);
	if (!iffile.is_open()) {
		std::cout << "infile error!";
		return -1;
	}
	string out_byte = in + ".bin";
	ofstream offile_byte(out_byte, ios::app);
	if (!offile_byte.is_open()) {
		std::cout << "outfile error!";
		return -2;
	}
	offile_byte.close();

	unsigned short CurrentTabLevel = 0;
	WholeExpression BYTECODE;

	auto is_empty_line = [](string& _string)->bool {
		if (_string.empty()) return true;
		for (int i = 0; isspace(_string[i]); ++i) {
			if (i >= code.size())
				return true;
		}
		return false;
	};

	line = 1;

	while (!iffile.eof()) {
		// Here taking a line - is the first and only principle of the forthcoming Palm compiler
		const unsigned int LineSize = 0x10000;
		char TheLineWeTake[LineSize];
		iffile.getline(TheLineWeTake, LineSize);
		code = TheLineWeTake;

		//check if it is an empty line
		if (is_empty_line(code)) continue;
		// After we've extracted a line, our next coming step is to identify the primary object (first one)
		///forward_list<string> ShatteredLine = move(splitLine(TheLineWeTake));
		//since we've already written a module thats accepts whole string, our plans have changed.
		//First, we count the amount of tabs preceding the line
		unsigned short Tabs;
		for (Tabs = 0; TheLineWeTake[Tabs] == '\t'; Tabs++);
		//code.erase(code.begin(), code.begin() + Tabs);
		//nah bro dont bother do the thing


		//neat
		int Difference = CurrentTabLevel - Tabs;
		if (Tabs < CurrentTabLevel) {
			BYTECODE.insert(BYTECODE.end(), Difference, ByteCode::end);
			isAssemblerMode = false;
			if (!OBJ_STACK.empty())
				for (int i = 0; i < Difference; ++i) {
					for (auto j : OBJ_STACK.top()) {
						GLOBAL_OBJECT_LIST.remove(j); // removing local variables
					}
					OBJ_STACK.pop();
				}


			CurrentTabLevel = Tabs;
		}
		else if (isAssemblerMode) {
			int size = strlen(TheLineWeTake);
			for (int i = Tabs; i < size && TheLineWeTake[i] != ';'; i++) {
				BYTECODE.push_back(ByteCode(TheLineWeTake[i]));
			}
			BYTECODE.push_back(ByteCode('\n'));
			continue;
		}



		vector<int> SplittedElements;
		vector<LineElement> ShatteredLine = splitLine(TheLineWeTake);

		WholeExpression* BCLine = Analyze(ShatteredLine, CurrentTabLevel);
		BYTECODE.append(BCLine);
		THROW_ERROR(Difference < 0, Tabs, err_UNXPSYM);


		line++;
	}

	BYTECODE.insert(BYTECODE.end(), CurrentTabLevel, ByteCode::end);
	std::cout << "Successfully compiled!\n";
	offile_byte.open(out_byte);

	// signature
	offile_byte << SIGNATURE;
	auto append_int = [&offile_byte](unsigned _int) {
		for (int i = 0; i < 4; i++) {
			offile_byte.put(char(_int));
			_int >>= 8;
		}};
	append_int(GLOBAL_FUNCTION_LIST.last_free() - 3);
	append_int(GLOBAL_OBJECT_LIST.max_last_free());
	append_int(const_strings);


	for (int i = 0; i < BYTECODE.size(); ++i)
		offile_byte.put(BYTECODE[i]);
	offile_byte.close();

	
	ifstream check(out_byte);
	write_byte_code_pure(check);
	std::wcout << endl;
	check.clear();
	ofstream offile(out);
	check.seekg(0, std::ios::beg);
	native(check, offile);
	check.close();
}