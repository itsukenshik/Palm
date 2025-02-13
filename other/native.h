#pragma once
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "bctable.h"

#define LENGTH(_string) sizeof(_string) / sizeof(_string[0])

const char SIGNATURE[] = "Palm Interpret Ramonter$";

void write_byte_code_pure(ifstream& _file) {

	int i = 1;
	while (true) {
		auto let = _file.get();
		if (_file.eof()) break; // end of file

		std::cout << endl << '#'<< i << "\t|\t(" << let << ")\t";
		if (let < size_of_bctable && let >= 0)
			std::cout << bctable[let];
		i++;
	}
 }

inline int get_int(char* INTBUFFER) {
	int INTbuf = 0;
	for (int i = 0; i < 4; ++i) {
		INTbuf += INTBUFFER[i] * pow(256, i);
	}
	return INTbuf;
}


void write_compact(ifstream& _file) {
	// SIGNATURE
	// AMOUNT OF FUNCTIONS
	// MAX LAST FREE TEMP DATA
	int tab_lv = 0;
	int let = 0;

	{
		char CHECK_SIGNATURE[LENGTH(SIGNATURE)];
		_file.read(CHECK_SIGNATURE, LENGTH(SIGNATURE) - 1);
		CHECK_SIGNATURE[LENGTH(SIGNATURE) - 1] = '\0';
		if (strcmp(CHECK_SIGNATURE, SIGNATURE)) {
			std::cout << "\nByte code file corrupted";
			return;
		}
	}
	auto read_byte_code = [&_file, &let]() {
		let = _file.get();
		if (_file.eof()) {
			std::cout << "\nUNEXPECTED END OF FILE";
			exit(EXIT_FAILURE);
		}
		std::cout << ' ' << bctable[let];
	};

	

	auto read_int = [&_file]() {
		char INTBUFFER[5];
		_file.read(INTBUFFER, 4);
		unsigned int INTbuf = get_int(INTBUFFER);
		std::cout << ' ' << INTbuf;
	};

	auto address_we_change = [&]() {
		read_byte_code();
		if (let == to_obj) {
			read_int();
		}
		else if (let == to_adr) {
			read_byte_code();
			read_byte_code();
			if (let != _reg)
				read_int();
		}
	};

	char temp[8];
	_file.read(temp, 8);

	const unsigned functions_amount = get_int(temp);
	const unsigned max_last_free = get_int(temp + 4);
	std::cout << functions_amount << ' ' << max_last_free;
	const char** FUNCTION_ARGS = new const char*[functions_amount];

	int i = 1;
	while (true) {
		let = _file.get();
		if (let < 0) break; // end of file

		std::cout << endl << '#' << i << "\t";
		for (int i = 0; i < tab_lv; ++i)
			std::cout << '\t';
		std::cout << bctable[let];
		i++;

		// tabs


		switch (let) {
		case home:
			tab_lv++;
			break;
		case ByteCode::end:
			tab_lv--;
			break;
		case allocate:
		case initialize_reg:
			read_byte_code();
			break;
		case declfnc:
			std::cout << '(';
			std::cout << bctable[_file.get()];
			while ((let = _file.get()) != null) {
				std::cout << ", " << bctable[let];
			}
			std::cout << ')';
			tab_lv++;
			break;
		case call:
		case callret:
		case retval:
		case retobj:
		case argconst:
		case argobj:
		case dwordadr_obj:
		case wordadr_obj:
		case byteadr_obj:
			read_int();
			break;


		case initialize_val:
			read_int();
			read_byte_code();
			break;
		case _2obj:
		case _byobj:
		case _obj:
			read_byte_code();
			read_int();
			std::cout << ',';
			read_int();
			break;
		case write:
			address_we_change();
			read_byte_code();
			if (let != _reg)
				read_int();
			break;
		case increment:
		case decrement:
			address_we_change();
			break;
		case If:
		case cyc:
		case whl:
		case til:
			tab_lv++;
			read_byte_code();
			if (let != _reg)
				read_int();
			break;
		default:
			break;
		}
	}
	std::cout << "\nend of file";
	return;
}

/*struct STRNS {
	unsigned char type;
	vector<unsigned int> values;
	int length = 0;
};

typedef unsigned int Ptr;

struct StrConstConnection {
	string name;
	Ptr ptr;
};

vector <STRNS> STRS;

vector <StrConstConnection> STR_CONST;
vector <unsigned char> CONST;

enum class data_vectors {
	TEMP, DATA, DATA_FUNC, DATA_PUBL
};

enum class _TYPE : unsigned char {
	_BYTE = 0x01, _WORD, _DWORD
};

template <class T>

inline unsigned char getTypeSize(T Type, unsigned int size = 1) {
	return (unsigned char)pow(2, (unsigned char)Type - 1) * size;
}

struct data_chunk {
	string depName;
	_TYPE type;
	Ptr ptr;
	unsigned int length;
};

struct free_chunk {
	Ptr ptr_Beg;
	Ptr ptr_Fin;
};*/
// для объектов нам категорически не нужны названия в виде строк, нужны только int
// 
/*class DATA_HEAP {
private:
	Ptr pos;
	vector<data_chunk> DATA;
	vector<free_chunk> FREE;
public:
	DATA_HEAP() {
		pos = 0;
		DATA.clear();
		STRS.clear();
	}

	~DATA_HEAP() {
		if (!DATA.empty())
			DATA.clear();

		if (!STRS.empty())
			STRS.clear();
	}

	optional<string> getRealAddress(string& name) {
		int i = 0;
		for (; i < DATA.size(); i++) {
			if (name == DATA[i].depName)
				return ("0x" + TO_STRHEX(DATA[i].ptr) + " + HEND");
		}
		return nullopt;
	}

	data_chunk& getData(int pos) {
		if (pos < DATA.size())
			return DATA[pos];
	}

	int next(int ptr) {
		int i = 0;
		do {

		} while (DATA.at(i).ptr <= ptr);

	}

	void reserve(unsigned char Type, string& Name, unsigned int length = 1) {
		THROW_ERROR(!length, -420, err_SLMZ);
		data_chunk temp;
		temp.depName = Name;
		if (!FREE.empty()) {
			for (int i = 0; i < FREE.size(); i++) {
				if (getTypeSize(_TYPE(Type), length) <= FREE[i].ptr_Fin - FREE[i].ptr_Beg + 1) {
					temp.ptr = FREE[i].ptr_Beg;
					if (getTypeSize(_TYPE(Type), length) == FREE[i].ptr_Fin - FREE[i].ptr_Beg + 1)
						FREE.erase(FREE.begin() + i);
					else
						FREE[i].ptr_Beg += getTypeSize(_TYPE(Type), length);
				}
			}


		}
		else {
			temp.ptr = pos;
			pos += getTypeSize(Type, length); // byte - 2^0 = 1 byte, word - 2^1 = 2 bytes and so on
		}

		temp.type = _TYPE(Type);
		temp.length = length;

		DATA.push_back(std::move(temp));
	}

	inline optional<unsigned int> find(string& Name) {
		for (unsigned int i = 0; i < DATA.size(); i++) {
			if (DATA[i].depName == Name) return i;
		}
		return nullopt;
	}

	inline unsigned char getType(string& Name) {
		return unsigned char(DATA[find(Name).value()].type);
	}

	void free(string& Name) {
		optional<unsigned int> posDataList = find(Name).value();
		THROW_ERROR(!posDataList.has_value(), 420, err_UNKV);

		Ptr the = DATA[posDataList.value()].ptr;
		Ptr end = the + getTypeSize(DATA[posDataList.value()].type, DATA[posDataList.value()].length);
		if (end == this->pos) {
			DATA.pop_back();
			this->pos -= end - the; // substracting the length of data chunk being removed
			if (!FREE.empty()) if (FREE.back().ptr_Fin + 1 == the) {
				this->pos -= FREE.back().ptr_Fin - FREE.back().ptr_Beg + 1; // substracting the length of free chunk being removed
				FREE.pop_back();
			}
			return;
		}

		end--; // it gets accurate as adding its size to the variable of "end" gives ptr 
		//to the next probable data chunk, not the ending of one being removed

		DATA.erase(DATA.begin() + posDataList.value());

		bool can_push_back_free = false;

		if (!FREE.empty()) {
			for (int i = 0; i < FREE.size(); i++) {
				if (FREE[i].ptr_Beg == end) { // if our piece is left of i
					FREE[i].ptr_Beg = the; // we simply replace its beginning with our beginning
					if (i + 1 < FREE.size()) if (FREE[i + 1].ptr_Fin + 1 == the) { // if BOTH left of i and right of i+1 (in the middle)
						FREE[i].ptr_Fin = FREE[i + 1].ptr_Fin; // WE SIMPLY replace i's fin with i+1's fin
						FREE.erase(FREE.begin() + i + 1); // ...and delete i+1 so theres only 1 piece left
					}

					return;																	//   --[-]-		
				}
				else if (FREE[i].ptr_Fin + 1 == the) { // if our piece is right of i				
					FREE[i].ptr_Fin = end; // we simply replace its end with our end
					return;																//       V
				}
			}
			can_push_back_free = true;
		}
		else
			can_push_back_free = true;

		if (can_push_back_free)
			FREE.push_back(free_chunk(the, end));
	}

	inline bool empty() {
		return DATA.empty();
	}
};*/

void native(ifstream& _file, ofstream& _offile) {
	// SIGNATURE
	// AMOUNT OF FUNCTIONS
	// MAX LAST FREE TEMP DATA
	int tab_lv = 0;
	int let = 0;

	{
		char CHECK_SIGNATURE[LENGTH(SIGNATURE)];
		_file.read(CHECK_SIGNATURE, LENGTH(SIGNATURE) - 1);
		CHECK_SIGNATURE[LENGTH(SIGNATURE) - 1] = '\0';
		if (strcmp(CHECK_SIGNATURE, SIGNATURE)) {
			std::cout << "\nByte code file corrupted";
			return;
		}
	}
	auto read_byte_code = [&_file, &let](bool add_space = true) {
		let = _file.get();
		if (_file.eof()) {
			std::cout << "\nUNEXPECTED END OF FILE";
			exit(EXIT_FAILURE);
		}
		if (add_space) std::cout << ' ';
		std::cout << bctable[let];
	};



	auto read_int = [&_file]() {
		char INTBUFFER[5];
		_file.read(INTBUFFER, 4);
		unsigned int INTbuf = get_int(INTBUFFER);
		std::cout << ' ' << INTbuf;
	};

	auto address_we_change = [&]() {
		read_byte_code();
		if (let == to_obj) {
			read_int();
		}
		else if (let == to_adr) {
			read_byte_code();
			read_byte_code();
			if (let != _reg)
				read_int();
		}
	};

	constexpr int info_length = 12;
	char temp[info_length];
	_file.read(temp, info_length);
	// 1
	string CONST_STRINGS;
	// 2
	string NATIVE_CODE;
	// 3
	stack<pair<string, int>> CURRENT_CYCLE;
	// 4



	const unsigned functions_amount = get_int(temp);
	const unsigned max_last_free = get_int(temp + 4);
	const unsigned const_strings_amt = get_int(temp + 8);
	std::cout << functions_amount << ' ' << max_last_free << ' ' << const_strings_amt;
	const char** FUNCTION_ARGS = new const char* [functions_amount];
	string temp_args;
	bool is_home = false;
	int* FUNCTION_ADDRESSES = new int [functions_amount];
	unsigned LAST_FUNCTION_ID = 0;
	char** VIRTUAL_HEAP = new char* [max_last_free];
	char INTBUFFER[5];
	unsigned int length; 
	char* CHAR_BUFFER;
	std::stringstream ss();
	
	unsigned POINTER = 0;

	int i = 1;
	while (true) {
		let = _file.get();
		if (let < 0) break; // end of file

		std::cout << endl << '#' << i << "\t";
		for (int i = 0; i < tab_lv; ++i)
			std::cout << '\t';
		std::cout << bctable[let];
		i++;

		// tabs


		switch (let) {
		case home:
			tab_lv++;
			break;
		case ByteCode::end:
			tab_lv--;
			break;
		case allocate:
			read_byte_code();
			break;
		case initialize_reg:
			read_byte_code();
			// MOV
			_offile << "\x66\xc7";
			if (let == Int) {
				// 0x00 si+bx
				// 0x04 si
				// 0x40 si+bx+off
				// 0x44 si+off
				INTBUFFER[0] = '\x44';
				if (true) {

				}
				_offile << '\x44';
				_file.read(INTBUFFER, 4);
				_offile.write(INTBUFFER, 4);
			}
			else if (let == ByteCode::byte) {

			}
			break;
		case declfnc:
			FUNCTION_ADDRESSES[LAST_FUNCTION_ID] = POINTER;
			std::cout << '('; 
			read_byte_code(false);
			temp_args.push_back(char(let));
			while (let != null) {
				std::cout << ',';
				temp_args.push_back(char(let));
				read_byte_code();
			}
			std::cout << ')';
			tab_lv++;
			CHAR_BUFFER = new char[temp_args.size() + 1];
			strcpy(CHAR_BUFFER, temp_args.c_str());
			FUNCTION_ARGS[LAST_FUNCTION_ID++] = CHAR_BUFFER;
			delete[] CHAR_BUFFER;
			break;
		case call:
		case callret:
		case retval:
		case retobj:
		case argconst:
		case argobj:
		case dwordadr_obj:
		case wordadr_obj:
		case byteadr_obj:
		case obj_address:
			read_int();
			break;
		case const_string:
			_file.read(INTBUFFER, 4);
			length = get_int(INTBUFFER);
			CHAR_BUFFER = new char[length];
			_file.read(CHAR_BUFFER, length);
			_offile.write(CHAR_BUFFER, length);
			delete[] CHAR_BUFFER;
			break;


		case initialize_val:
			read_int();
			read_byte_code();
			break;
		case _2obj:
		case _byobj:
		case _obj:
			read_byte_code();
			read_int();
			std::cout << ',';
			read_int();
			break;
		case write:
			address_we_change();
			read_byte_code();
			if (let != _reg)
				read_int();
			break;
		case increment:
		case decrement:
			address_we_change();
			break;
		case If:
		case cyc:
		case whl:
		case til:
			tab_lv++;
			read_byte_code();
			if (let != _reg)
				read_int();
			break;
		default:
			break;
		}
		POINTER++;
	}
	std::cout << "\nend of file";
	return;
}