// The structure:
// 1. Get a code line, convert it into a byte code chunk (no "heap" needed)
// 2. Put chunks in one sequence, get this whole byte code file out
// 3. Link all mentioned files
// 4. Convert the whole byte code to assembler code
//

#include <stack>
#include <fstream>
#include "preprocess.h"
#include "exp.h"



WholeExpression NATIVE_OUT;


	//for (i = 0; i < cond.length() && ((cond[i] >= 'A' && cond[i] <= 'Z') || (cond[i] >= 'a' && cond[i] <= 'z') || (cond[i] == '_')); i++) {
	   // может ли это быть названием переменной?
	//        varname.push_back(cond[i]);
	//}
	//int j;
	//for (j = 0; j < DATA.size() && DATA.at(j).first != varname; j++) {}
	//if (DATA[j].first != varname) {
	//    dothing.first.first = "var_" + varname;
	//}
	//else {
	//    !error;
	//    return;
	//}

void init_vars (vector <pair <string, unsigned char>>& DATA, string& code, bool is_boot, vector <string>& OUT, vector <STRNS>& STRS, int MEANS, string signt) {
	int strc = 0;
	char type[] = " db ";

	for (int i = 0; i < DATA.size(); i++) {
		switch (DATA[i].second)
		{
		case DWORD: //DWORD
			code = signt + '_' + DATA[i].first + " dd 0";
			OUT.insert(OUT.begin() + is_boot * BOOT_SIZE + MEANS, code);
			break;
		case WORD:
			code = signt + '_' + DATA[i].first + " dw 0";
			OUT.insert(OUT.begin() + is_boot * BOOT_SIZE + MEANS, code);
			break;
		case BYTE:
			code = signt + '_' + DATA[i].first + " db 0";
			OUT.insert(OUT.begin() + is_boot * BOOT_SIZE + MEANS, code);
			break;
		case STR: 
			switch (STRS[strc].type)
			{
			case BYTE:
				type[2] = 'b';
				break;
			case WORD:
				type[2] = 'w';
				break;
			case DWORD:
				type[2] = 'd';
				break;
			case PTR:
				break;
			default:
				break;
			}
			char e[30];
			code.clear();
			if (STRS[strc].length > STRS[strc].values.size()) {
				_itoa_s(STRS[strc].length - (int)STRS[strc].values.size(), e, 10);
				code = "times ";
				code += e;
				code += type;
				code += "0";
			}
			
			char s[11];
			if (!STRS[strc].values.empty()) {
				code = '\n' + code;
				for (auto we = STRS[strc].values.rbegin(); we != STRS[strc].values.rend(); we++) {
					_itoa_s(*we, s, 10);
					if (we != STRS[strc].values.rbegin())
						code = ", " + code;
					code.insert(0, s);
				}

				code = type + code; // db
			}
			code.insert(0, ":\n");
			code = DATA[i].first + code; // var_a
			code = "var_" + code;
			OUT.insert(OUT.begin() + is_boot * BOOT_SIZE + MEANS, code);
			strc++;
			break;
		case PTR:
			code = signt + '_' + DATA[i].first + " dd 0";
			OUT.insert(OUT.begin() + is_boot * BOOT_SIZE + MEANS, code);
			break;
		default:
			break;
		}
	}
}


constexpr bool isDebug = true;
/*
int maint(int argc, char* argv[])
{
	string in, out;

	if (!isDebug) {
		if (argc == 1) {
			cout << "\n    #####\n    #####\n###################\n### RamTax v0.1 ###\n###################\n    #####\n    #####\n\nUsage syntax: ramtax [infile] [outfile]\n";
			return 0;
		}

		if (argc > 1)
			in = argv[1];

		if (argc > 2)
			out = argv[2];
		else {
			cout << "Output file not specified";
			return -7;
		}
	}
	else {
		in = "file.rmtx";
		out = "out.asm";
	}
	

	string str;
	string cond;
	string name;
	char r[65];
	ifstream iffile(in);
	if (!iffile.is_open()) {
		std::cout << "infile error!";
		return -1;
	}
	ofstream offile(out, ios::app);

	if (!offile.is_open()) {
		std::cout << "outfile error!";
		return -2;
	}

	offile.close();

	while (!iffile.eof()) {
		char f = 0;
		int e = 0;
		code.clear();
		bool comment = 0;
		while (f != '\n' && !iffile.eof()) {
			f = iffile.get();
			if (f == '#' && !comment)
				comment = 1;
			if (!comment && f != '\n' && !iffile.eof())
				code.push_back(f);
		}
		getToNSpace(e, code);
		if (!code.substr(e).empty())
			IN.push_back(code);
	}

	iffile.close();

	PreProcess(IN);


	for (line = 0; line < IN.size(); line++) {
		code = IN[line];
		std::cout << code << endl;

		int row_start = 0;
		getToNSpace(row_start, code); //подобраться к не-пробелу
		CURROUT().push_back("; ####### ");
		CURROUT().back() += code.substr(row_start);
		if (!code.substr(row_start).empty()) {
			bool isConst = code.substr(row_start, 6) == "const ";
			if (code.substr(row_start, 5) == "init " || code.substr(row_start, 5) == "resv " || isConst) { // инициализация переменных
				row_start += 5;
				unsigned char type_str = 0;
				unsigned int length = 0;
				unsigned char type = defineType(code, row_start, name, length);

				//THROW_ERROR(type == 0, row_start); // bad type

				type_str = type_xtrg;

				getToNSpace(row_start, code);
				getwordUntilSpace(name, code, row_start);
				for (int i = 0; i < name.length(); i++) {
					THROW_ERROR(!(std::isalpha(name[i]) || name[i] == '_'), offset_from_space, err_BVRN);
				}

				optional<unsigned int> a = heap.find(name);
				if (!a.has_value()) { // if theres no such var
					heap.reserve(type, name, length);
				}
				else THROW_ERROR(heap.getType(name) != type, row_start, err_HALD); // if var with given name already exists, it must be the same type as it was declaired

				getToNSpaceIn(row_start, code);

				if (row_start < code.length()) {
					if (code[row_start] == '=') {
						getToNSpaceIn(row_start, code);

						THROW_ERROR(row_start >= code.length(), row_start, err_TLOP);
					}
					else THROW_ERROR(length == 0 || isConst, offset_from_space, err_TLOP); // if theres no assignment, then it must have at least size
				}


				if (type == STR) {
					vector<unsigned char> symbArray; // массив символов для определения строки

					if (row_start < code.length()) {
						THROW_ERROR(code[row_start] != '\"', row_start, err_BSTX);
						row_start++;

						for (; code[row_start] != '"'; row_start++) {
							THROW_ERROR(row_start >= code.length() - 1, row_start, err_TLOP);

							if (!symbArray.empty()) {
								if (symbArray.back() == '\\') {
									char t[5] = "0x\0\0";
									unsigned char e;
									switch (code[row_start])
									{
									case '0':
										symbArray.back() = ('\0');
										break;
									case 'x':
										for (unsigned char i = 0; i < 2 && row_start + 1 < code.length(); i++) {
											if ((code[row_start + 2] >= '0' && code[row_start + 1] <= '9') || (code[row_start + 2] >= 'a' && code[row_start + 1] <= 'f') || (code[row_start + 2] >= 'A' && code[row_start + 1] <= 'F')) {
												t[2 + i] = code[row_start + 1];
												row_start++;
											}
										}
										e = CHAR_TO_INT(t);
										symbArray.back() = (e);
										break;
									case 'a':
										symbArray.back() = ('\a');
										break;
									case 'b':
										symbArray.back() = ('\b');
										break;
									case 't':
										symbArray.back() = ('\t');
										break;
									case 'n':
										symbArray.back() = ('\n');
										break;
									case 'v':
										symbArray.back() = ('\v');
										break;
									case 'f':
										symbArray.back() = ('\f');
										break;
									case 'r':
										symbArray.back() = ('\r');
										break;
									case '"':
										symbArray.back() = ('\"');
										break;
									case '\\':
										symbArray.back() = ('\\');
										break;
									default:
										THROW_ERROR(true, row_start, err_BSTX);
										break;
									}
								}
								else
									symbArray.push_back(code[row_start]);
							}
							else
								symbArray.push_back(code[row_start]);
						}

						if (symbArray.back() != '\0') {
							symbArray.back() = ('\0');
						}
					}

					if (length == 0)
						length = (unsigned int)symbArray.size();
					else THROW_ERROR(length < symbArray.size(),
						offset_from_space, err_CSIS);

					addConst(symbArray, type_str);
				}
			}
			else if (code.substr(row_start, 3) == "if ") {
				row_start += 3;
				int row_end;
				cond.clear();
				for (row_end = row_start; code.substr(row_end, 4) != " do:"; row_end++) {
					cond.push_back(code.at(row_end));
				}
				defineExpression(cond, CURRDATA(), CURROUT(), num_cond);

				THROW_ERROR(error, error_cause);

				CURROUT().push_back("test eax, eax");
				CURROUT().push_back("jz if_");
				char r[65];
				_itoa_s(num_cond, r, 10);
				OUT.back() += r;
				cycles.push(pair(num_cond, 0));
			}
			else if (code.substr(row_start, 3) == "end") {
				switch (cycles.top().second)
				{
				case 0x00: // if
					CURROUT().push_back("if_");
					break;
				case 0x01: // inft
					CURROUT().push_back("ifn_");
					break;
				case 0x80: // cycle
					CURROUT().push_back("loop ccl_");
					break;
				case 0x81: // func
					CURROUT().push_back("ret");

					if (FNCS.back().second[0][0] == NOTH) { // не является ли функция возвращающей
						DATA_FUNC.erase(DATA_FUNC.begin());
					}

					name = "fnv_" + fnc_name;
					init_vars(DATA_FUNC, code, false, FUNC, STRS, 0, name);
					for (string i : FUNC) {
						OUT.push_back(i);
					}

					is_func = 0;
					break;
				case 0x02: // until
					CURROUT().push_back("jmp til_");
					break;
				default:
					break;
				}
				char r[65];
				_itoa_s(cycles.top().first, r, 10);
				if (cycles.top().second != 0x81)
					CURROUT().back() += r;
				if (cycles.top().second == 0x02) {
					CURROUT().push_back("tle_");
					CURROUT().back() += r;
				}
				if (cycles.top().second < 0x80)
					CURROUT().back() += ":";
				else if (cycles.top().second != 0x81)
					CURROUT().push_back("pop ecx");
				cycles.pop();
				NATIVE_OUT.push_back(ByteCode::end);
			}
			else if (code.substr(row_start, 4) == "equ " || code.substr(row_start, 4) == "inc " || code.substr(row_start, 4) == "dec ") {

				unsigned char special_cmd = 0;
				if (code.substr(row_start, 4) == "inc ") {
					NATIVE_OUT.push_back(ByteCode::inc);
					special_cmd = 1;
				}
				else if (code.substr(row_start, 4) == "dec ") {
					NATIVE_OUT.push_back(ByteCode::dec);
					special_cmd = 2;
				}
				else NATIVE_OUT.push_back(ByteCode::equ);

				bool ptr7 = false;
				getToNSpaceIn(row_start, code, 4);
				if (code[row_start] == '*') {
					ptr7 = true;
					getToNSpaceIn(row_start, code);
					NATIVE_OUT.push_back(ByteCode::ptr);
				}
				getword(name, code, row_start);
				addStringToByteCode(&name, &NATIVE_OUT);

				THROW_ERROR((DATA.empty() && !is_func), row_start, err_UNKV);

				int i = 0;
				unsigned int str_it = 0;
				unsigned int ptr_it = 0;
				for (; CURRDATA().at(i).first != name; i++) {
					THROW_ERROR(i >= (CURRDATA().size()) - 1, row_start, err_UNKV);

					if (CURRDATA().at(i).second == STR)
						str_it++;
					if (CURRDATA().at(i).second == PTR)
						ptr_it++;
				}
				bool str_eto = false;
				unsigned char type_str = 0;
				if (CURRDATA().at(i).second == STR) {
					type_str = STRS[str_it].type;
					str_eto = true;
					getToNSpace(row_start, code);

					THROW_ERROR(code[row_start] != '[', row_start, err_BSTX);

					getToNSpaceIn(row_start, code);
					cond.clear();
					int brackets = 1;
					for (; row_start < code.length() && brackets > 0; row_start++) {
						if (code[row_start] != ']' || brackets > 1)
							cond.push_back(code[row_start]);
						if (code[row_start] == ']')
							brackets--;
						else if (code[row_start] == '[')
							brackets++;
					}
					defineExpression(cond, CURRDATA(), CURROUT(), num_cond);

					THROW_ERROR(error, error_cause);

					/*CURROUT().push_back("mov esi, var_");
					CURROUT().back() += name;
					CURROUT().push_back("add esi, eax");
					CURROUT().push_back("shl esi, 12");
					CURROUT().push_back("shr si, 12");
					CURROUT().push_back("mov bx, si");
					CURROUT().push_back("shr si, 16");
					CURROUT().push_back("mov ds, si");
					CURROUT().push_back("mov si, bx");
					CURROUT().push_back("mov [es:di]");
					CURROUT().push_back("mov si, var_");
					CURROUT().back() += name;
					if (type_str == WORD) {
						CURROUT().push_back("shl eax, 1");
					}
					else if (type_str == DWORD) {
						CURROUT().push_back("shl eax, 2");
					}
					CURROUT().push_back("add esi, eax");
					name = "si";
				}

				if (!special_cmd) {
					getToNSpaceIn(row_start, code);

					THROW_ERROR(code.at(row_start) != '=', row_start, err_BSTX);

					getToNSpaceIn(row_start, code);
					int offset = row_start;
					cond.clear();
					for (; row_start < code.length(); row_start++) {
						cond.push_back(code.at(row_start));
					}
					defineExpression(cond, CURRDATA(), CURROUT(), num_cond);

					THROW_ERROR(error, error_cause + offset);

					CURROUT().push_back("mov ");
				}
				else if (special_cmd == 1) {
					CURROUT().push_back("inc ");
				}
				else if (special_cmd == 2) {
					CURROUT().push_back("dec ");
				}

				//OUT.push_back("pushad");
				if (!is_func) {
					if (ptr7) {
						CURROUT().pop_back();
						CURROUT().push_back("mov edx, ");
					}
					CURROUT().back() += '[';
					if (!str_eto)
						CURROUT().back() += "var_";
				}
				else {
					if (ptr7) {
						CURROUT().pop_back();
						CURROUT().push_back("mov edx, ");
					}
					CURROUT().back() += '[';
					if (!str_eto) {
						CURROUT().back() += "fnv_";
						CURROUT().back() += fnc_name;
						CURROUT().back() += '_';
					}
				}
				CURROUT().back() += name;
				CURROUT().back() += ']';
				if (ptr7) {
					CURROUT().push_back("mov di, dx");
					CURROUT().push_back("xor dx, dx");
					CURROUT().push_back("shr edx, 4");
					CURROUT().push_back("mov es, dx");
					if (!special_cmd)
						CURROUT().push_back("mov");
					else if (special_cmd == 1) {
						CURROUT().push_back("inc");
					}
					else if (special_cmd == 2) {
						CURROUT().push_back("dec");
					}
					CURROUT().back() += (" [es:di]");
				}
				if (special_cmd == 0) // lalala;
					if (CURRDATA().at(i).second == DWORD || CURRDATA().at(i).second == PTR || type_str == DWORD || PTRS.at(ptr_it) == DWORD) {
						CURROUT().back() += ", eax";
					}
					else if (CURRDATA().at(i).second == WORD || type_str == WORD || PTRS.at(ptr_it) == WORD) {
						CURROUT().back() += ", ax";
					}
					else if (CURRDATA().at(i).second == BYTE || type_str == BYTE || PTRS.at(ptr_it) == BYTE) {
						CURROUT().back() += ", al";
					}

				//OUT.push_back("popad");

			}

			else if (code.substr(row_start, 5) == "ifnt ") {
				CURROUT().push_back("jmp ifn_");
				_itoa_s(cycles.top().first, r, 10);
				CURROUT().back() += r;
				CURROUT().push_back("if_");
				CURROUT().back() += r;
				CURROUT().back() += ':';
				cycles.top().second = 1;
			}
			else if (code.substr(row_start, 5) == "HOME:") {
				is_home = true;
				OUT.push_back("HOME:");
				NATIVE_OUT.push_back(ByteCode::home);
			}
			else if (code.substr(row_start, 5) == "HEND.") {
				CURROUT().push_back("hlt");
				CURROUT().push_back("HEND:");
				NATIVE_OUT.push_back(ByteCode::hend);
			}

			else if (code.substr(row_start, 4) == "ASM:") {
				int row = 0;
				while (code.substr(row, 5) != "AEND.") {
					if (line + 1 < IN.size())
						code = IN[++line];
					else THROW_ERROR(true, row_start, err_BSTX);

					std::cout << code << endl;
					row = 0;
					getToNSpace(row, code);
					if (code.substr(row, 5) != "AEND.")
						CURROUT().push_back(code.substr(row));
				}

			}
			else if (code.substr(row_start, 5) == "func ") {
				unsigned char type_func = 0;
				unsigned int len = 0;

				THROW_ERROR(is_home, row_start, err_DFIH);
				THROW_ERROR(is_func, row_start, err_DFIF);

				is_func = true;
				DATA_FUNC.clear();
				FUNC.clear();

				getToNSpaceIn(row_start, code, 5);
				type_func = defineType(code, row_start, name, len);
				THROW_ERROR(type_func == 0, row_start, err_UNKT);

				getword(name, code, row_start);
				for (int i = 0; i < FNCS.size(); i++) {
					THROW_ERROR(FNCS.at(i).first == name, row_start, err_HALD);
				}
				DATA_FUNC.push_back(pair(name, type_func));
				fnc_name = name;


				getToNSpace(row_start, code);

				THROW_ERROR((code[row_start] != ':'), row_start, err_BSTX);

				row_start++;
				getToNSpace(row_start, code);

				while (code.substr(row_start, 3) != "do:") {
					THROW_ERROR(row_start >= code.length(), row_start, err_BSTX);

					unsigned int length = 0;

					unsigned char type = defineType(code, row_start, name, length); // ОПИРДЕЛЯЕМ ТИПП ПИРИМЕННОЙ
					THROW_ERROR(type == 0, row_start);

					unsigned char type_str = type_xtrg;

					getword(name, code, row_start);
					THROW_ERROR(ContainsThatVar(DATA_FUNC, name) != -1, row_start, err_HALD);

					DATA_FUNC.push_back(pair(name, type));
					if (type == PTR)
						PTRS.push_back(type_str);
					else if (type == STR) {
						STRNS strr;
						strr.length = length;
						strr.type = type_str;
						STRS.push_back(strr);
					}


					getToNSpace(row_start, code);
					if (code.substr(row_start, 3) != "do:") {

						THROW_ERROR(code[row_start] != ',', row_start, err_BSTX);

						row_start++;
						getToNSpace(row_start, code);
					}

				}


				vector<string> f;
				for (int ij = 0; ij < DATA_FUNC.size(); ij++) {
					code.clear();
					code.push_back(DATA_FUNC[ij].second);
					code += DATA_FUNC[ij].first;
					f.push_back(code);

				}
				FNCS.push_back(pair(fnc_name, f));
				//name = "fnv_" + fnc_name;
				//g(DATA_FUNC, code, false, FUNC, STRS, 0, name);
				FUNC.push_back("fnc_");
				FUNC.back() += fnc_name;
				FUNC.back() += ':';
				//       FUNC.push_back("pop ecx");
			 //          for (int ij = DATA_FUNC.size() - 1; ij > 0; ij--) {
				//           FUNC.push_back("pop [fnv_");
				  //         FUNC.back() += fnc_name;
					//       FUNC.back() += '_';
					 //      FUNC.back() += DATA_FUNC[ij].first;
					 //      FUNC.back() += ']';
					  // }
				  //     FUNC.push_back("push ecx");
				cycles.push(pair(0, 0x81));
				GivePublics();
			}
			else if (code.substr(row_start, 6) == "cycle ") { // this is definetely A cycle
				row_start += 6;
				getToNSpace(row_start, code);
				for (; code.substr(row_start, 4) != " do:"; row_start++) {
					cond.push_back(code.at(row_start));
				}
				defineExpression(cond, CURRDATA(), CURROUT(), num_cond);

				THROW_ERROR(error, error_cause);

				CURROUT().push_back("push ecx");
				CURROUT().push_back("mov ecx, eax");
				CURROUT().push_back("ccl_");
				_itoa_s(num_cond, r, 10);
				CURROUT().back() += r;
				CURROUT().back() += ':';
				cycles.push(pair(num_cond, 0x80));
			}
			else if (code.substr(row_start, 6) == "until ") {
				row_start += 6;
				getToNSpace(row_start, code);
				for (; code.substr(row_start, 4) != " do:"; row_start++) {
					cond.push_back(code.at(row_start));
				}
				size_t place = CURROUT().size();
				defineExpression(cond, CURRDATA(), CURROUT(), num_cond);

				THROW_ERROR(error, error_cause);

				_itoa_s(num_cond, r, 10);
				name = "til_";
				name += r;
				name += ':';
				CURROUT().insert(CURROUT().begin() + place, name);
				CURROUT().push_back("test eax, eax");
				CURROUT().push_back("jnz tle_");
				CURROUT().back() += r;
				cycles.push(pair(num_cond, 0x02));
			}
			else {
				cond.clear();
				for (; row_start < code.length(); row_start++)
					cond.push_back(code.at(row_start));
				defineExpression(cond, CURRDATA(), CURROUT(), num_cond);
				THROW_ERROR(error, error_cause);
			}
		}
	}



		init_vars(DATA, code, is_boot, OUT, STRS, 1, "var");

		offile.open(out);

		for (int i = 0; i < OUT.size(); i++) {
			WriteString(offile, OUT[i]);
			offile.write("\n", 1);
		}

		/*else if (code[0] == '"') {
			str.clear();
			code.erase(0, 1);
			str += code;
			char r;
			r = iffile.get();
			while (r != '"') {
				str.push_back(r);
				r = iffile.get();
			}
			STRS.push_back(str);
			DATA.push_back(pair(" ", 0x80));


		offile.close();
		std::cout << "\nFile successfully compiled!\n";
		return 0;
}*/

int main() {
	string r = "A";
	int e;
	WholeExpression t = defineWholeExpression(r, e);
	string fg = "ff\0edf";
	fg.push_back('\0');
	t;
	fg.push_back('d');
	cout << endl << fg;
}

//int main() {
//	DATA_HEAP heap;
//	string t = "Apple";
//	heap.reserve(BYTE, t);
//	t = "Bubble";
//	heap.reserve(DWORD, t);
//	t = "Con";
//	heap.reserve(BYTE, t, 10); // str 10 byte con
//	t = "Dome";
//	heap.reserve(WORD, t);
//
//	t = "Bubble";
//	heap.free(t);
//	t = "Dome";
//	heap.free(t);
//	t = "Con";
//	heap.free(t);
//	t = "Apple";
//	heap.free(t);
//
//	t = "Engineer";
//	heap.reserve(WORD, t);
//	t = "Fan";
//	heap.reserve(WORD, t, 3);
//	t = "Gelatin";
//	heap.reserve(DWORD, t, 6);
//	t = "Hexaeder";
//	heap.reserve(WORD, t);
//
//	t = "Fan";
//	heap.free(t);
//	t = "Hexaeder";
//	heap.free(t);
//	t = "Gelatin";
//	heap.free(t);
//	t = "Engineer";
//	heap.free(t);
//
//	return 20;
//}