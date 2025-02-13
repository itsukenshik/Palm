#pragma once
#include <vector>
#include <cstring>
#include <list>
#include <map>
#include <algorithm>
#include "exp.h"


#define THEN_s Keywords[0]
#define ALLOCATE 1
#define INT Keywords[2]
#define WORD Keywords[3]
#define BYTE 4
#define ADR Keywords[5]
#define DWORDADR 6
#define WORDADR 7
#define BYTEADR 8
#define SIGNED_INT Keywords[9]
#define SIGNED_WORD Keywords[10]
#define SIGNED_BYTE Keywords[11]
#define QWORD Keywords[12]
#define QWORDADR Keywords[13]
#define SIGNED_QWORD Keywords[14]
#define WHILE 15
#define UNTIL 16
#define HOME 17
#define RETURN 18
#define CYCLE 19
#define ALLOCARRAY Keywords[20]
#define CONST Keywords[21]
#define CONSTARRAY 22
#define MACRO Keywords[23]
#define TRUE Keywords[24]
#define FALSE Keywords[25]
#define ASM 26

#define Types_begin 2
#define Types_end 15

bool NameFits(string& Name) {
	return (std::count_if(Name.begin(), Name.end(), 
		[](unsigned char c) { return std::isalnum(c) || c == '_'; }) && !isOneOfArray(Name, Keywords, Keywords_size)); // object name should not be a keyword
}

class dataChunkType : public list<LineElement> {
public:
	dataChunkType(vector<LineElement>& Vec, int Elements) {
		for (int i = 0; i < Elements; i++)
			this->push_back(Vec[i + 1]);
	}
};


bool isAssemblerMode = false;
unsigned const_strings = 0;

#define FUNCTION_VARS_ONLY

 WholeExpression* Analyze(vector<LineElement>& Body, unsigned short& CurrentTabLevel) {
	 if (Body.empty()) return nullptr;

	WholeExpression* ObjectWeReturn = new WholeExpression;
	unsigned front_nameID = whichOneOfArray(Body.front().Name, Keywords, Keywords_size);

	if (Body.back().Name == ":") {
		TreePart* tree_part;
		{ // empty deque
			deque<unsigned> t;
			OBJ_STACK.push(t);
		}
		vector<LineElement> subBody; 
		auto make_a_subBody = [&](bool _isIF) {
			copy(Body.begin() + 1 - _isIF, Body.end() - 1 - _isIF, back_inserter(subBody));
			tree_part = new TreePart(build_expr(subBody));
			WholeExpression* subexpr = tree_part->to_byte_code();
			if (subexpr != nullptr)
				ObjectWeReturn->push_back(argument_start);
			ObjectWeReturn->append(subexpr);
		};

		auto val_obj_reg = [&]() {
			if (ObjectWeReturn->size() != 1) {
				ObjectWeReturn->push_back(ByteCode::_reg);
			}
			else {
				auto objID = GLOBAL_OBJECT_LIST.name_index(tree_part->Element.Name);
				if (objID != UINT32_MAX) {
					ObjectWeReturn->push_back(ByteCode::_obj);
					ObjectWeReturn->append_int(objID);
				} else
				{
					ObjectWeReturn->push_back(ByteCode::_val);

					ObjectWeReturn->append_int(s_to_i(tree_part->Element));
				}
			}
			return;
		};
		if ((Body.end() - 2)->Name == THEN_s) {
			// IF construction: 
			// expr then:
			make_a_subBody(true);
			ObjectWeReturn->push_back(ByteCode::If);
			val_obj_reg();
		}
		else if (front_nameID == WHILE) {
			// WHILE construction:
			// while expr:
			make_a_subBody(false);
			ObjectWeReturn->push_back(ByteCode::whl);
			val_obj_reg();
		}
		else if (front_nameID == UNTIL) {
			// WHILE construction:
			// while expr:
			make_a_subBody(false);
			ObjectWeReturn->push_back(ByteCode::til);
			val_obj_reg();
		}
		else if (front_nameID == CYCLE) {
			// CYClE construction:
			// cycle expr:
			make_a_subBody(false);
			ObjectWeReturn->push_back(ByteCode::cyc);
			val_obj_reg();
		}
		else if (front_nameID == HOME) {
			ObjectWeReturn->push_back(ByteCode::home);
		}
		else if (front_nameID == ASM) {
			ObjectWeReturn->push_back(ByteCode::assembler);
			isAssemblerMode = true;
		}
		// function declaration
		else if (Body.size() >= 4) { // arbit, '(', ')', ':'
			if (Body[1].Name == "(") {
				ObjectWeReturn->push_back(ByteCode::declfnc);
				#ifdef FUNCTION_VARS_ONLY
				unsigned char Type;
				string FuncTypes; // numbers to put in "GLOBAL_FUNCTION_LIST" map
				 // numbers to put in "CurrentFuncVars" array
				int i = int(Body.size());

				if (Body[i - 3].Name == "=") { // checking if it is a function or procedure
					FuncTypes = char(checkForType(Body[i - 2].Name));
					THROW_ERROR(FuncTypes[0] == UINT32_MAX, Body[i - 2].Position, err_UNKT);
				}
				else FuncTypes = "\x1"; // use 0x01 byte if there's no returning value declared
				i = 2;
				if (Body[i].Name != ")") // there may be no arguments
				do {
					bool notEndBrack = true;
					switch (i % 3) {
					case 2: // type
						Type = checkForType(Body[i].Name);
						THROW_ERROR(Type == UINT32_MAX, Body[i].Position, err_UNKT);
						break;
					case 1: // ','
						notEndBrack = Body[i].Name != ")";
						if (i != 2) {
							THROW_ERROR(Body[i].Name != "," && notEndBrack, Body[i].Position, err_EXCM);
							FuncTypes.push_back(Type); // pushing the parameter as a var into GLOBAL_OBJECT_LIST (is to be removed later)
							{ // ArgTemp will be deleted by "push" method
								tempDataChunk* ArgTemp = new tempDataChunk(Body[i - 2].Name, Body[i - 1].Name);
								OBJ_STACK.top().push_back(GLOBAL_OBJECT_LIST.push(ArgTemp));
								GLOBAL_OBJECT_LIST;
							}
						}
						if (!notEndBrack) goto func_args_proc_end; // leace the cycle
						
						break; // 
					case 0: // arbit (parameter name)
						THROW_ERROR(!NameFits(Body[i].Name), Body[i].Position, err_BVRN);
						break;
					default:
						break;
					}
					i++;
				} while (true);

			func_args_proc_end: // jmp!


				// TO DO:
				// prevent duplicate variable names
				{
					tempDataChunk* FunctionTemp = new tempDataChunk(FuncTypes, Body[0].Name);
					GLOBAL_FUNCTION_LIST.push(FunctionTemp);
				}
				ObjectWeReturn->add_string_funcparams(FuncTypes);
				#endif // FUNCTION_VARS_ONLY
			
			}
			else THROW_ERROR(true, Body[1].Position, err_BSTX);
		}// function declaration
		CurrentTabLevel++;
	} 
	else if (front_nameID == ALLOCATE || front_nameID == CONSTARRAY) {
		// ALLOCATE construction:
		// allocate type arbit
		// allocate type arbit = expr
		THROW_ERROR(Body.size() < 3, Body.front().Position, err_BAUU);
		unsigned int Type = checkForType(Body[1].Name); // searching for the requested type if it is valid
		THROW_ERROR(Type == UINT32_MAX, Body[1].Position, err_UNKT);

		//checking whether the name fits
		THROW_ERROR(!NameFits(Body[2].Name), Body[2].Position, err_BVRN);

		//checking whether the name already exists
		THROW_ERROR(is_obj(Body[2].Name), Body[2].Position, err_BVRN);

		//line is NEVER gonna have 4 elements
		THROW_ERROR(Body.size() == 4, Body[2].Position, err_BVRN);

		//checking whether there's an equation sign
		bool _not_initialize = true;
		if (Body.size() >= 5) {
			_not_initialize = Body[3].Name != "=";
			THROW_ERROR(_not_initialize, Body[2].Position, err_EQUSIGN);
		}
		{
			tempDataChunk* BodyTemp = new tempDataChunk(Body);
			// appending current variable to the global list
			// and pending its ID to the global stack
			OBJ_STACK.top().push_back(GLOBAL_OBJECT_LIST.push(BodyTemp));
		}
		
		// CONSTARRAY part (temporary stirng solution)
		if (front_nameID == CONSTARRAY) {
			THROW_ERROR(Type != BYTE, Body[2].Position, err_BADTYPE);
			THROW_ERROR(_not_initialize, Body[3].Position, err_CONSTINIT);
			basic_string<char> symbArray; // массив символов для определения строки
			unsigned row_start = Body[4].Position;

			if (row_start < code.length()) {
				THROW_ERROR(code[row_start] != '\'', row_start, err_BSTX);
				row_start++;

				for (; code[row_start] != '\''; row_start++) {
					THROW_ERROR(row_start >= code.length() - 1, row_start, err_TLOP);

					if (!symbArray.empty()) {
						if (symbArray.back() == '\\') {
							char t[3] = "\0\0";
							unsigned char e = 0;
							switch (code[row_start])
							{
							case '0':
								symbArray.back() = ('\0');
								break;
							case 'x':
								for (unsigned char i = 0; i < 2 && row_start + 1 < code.length(); i++) {
									if ((code[row_start + 2] >= '0' && code[row_start + 1] <= '9') || (code[row_start + 2] >= 'a' && code[row_start + 1] <= 'f') || (code[row_start + 2] >= 'A' && code[row_start + 1] <= 'F')) {
										t[i] = code[row_start + 1];
										row_start++;
									}
								}
								//CHAR_TO_INT
								if (isalpha(t[0])) {
									t[0] = toupper(t[0]);
									t[0] -= 7; // 0x41 - 0x3A
								}
								if (isalpha(t[1])) {
									t[1] = toupper(t[1]);
									t[1] -= 7; // 0x41 - 0x3A
								}
								
								e = (t[0] - 0x30) * 16 + t[1] - 0x30;

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
							case '\'':
								symbArray.back() = ('\'');
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

				if (symbArray.back() != '\0')
					symbArray.push_back('\0');
			}
			string const_type;

			// const str type:
			// const_string, size, type of characters;
			const_type.push_back(char(ByteCode::const_string));
			unsigned size = symbArray.size();
			for (int i = 0; i < 4; i++) {
				const_type.push_back(char(size));
				size  >>= 8;
			}
			const_type.push_back(Type);
			tempDataChunk* temp = new tempDataChunk(const_type, Body[2].Name);
			GLOBAL_OBJECT_LIST.push(temp);
			ObjectWeReturn->push_back(const_string);
			ObjectWeReturn->append_int(symbArray.size());
			for (auto i : symbArray)
				ObjectWeReturn->push_back(ByteCode(i));
			const_strings++;
			return ObjectWeReturn;
		}

		if (_not_initialize) {
			ObjectWeReturn->push_back(ByteCode::allocate);
		}
		else {
				auto subvector = new vector<LineElement>(Body.begin() + 4, Body.end());
				TreePart tree_part = build_expr(*subvector);
				delete subvector;
				WholeExpression* temp = tree_part.to_byte_code();

				if (*tree_part.isOperator) {
					ObjectWeReturn->append(temp);
					ObjectWeReturn->push_back(ByteCode::initialize_reg);
				}
				else {
					auto objID = GLOBAL_OBJECT_LIST.name_index(tree_part.Element.Name);
					if (objID != UINT32_MAX) {
						ObjectWeReturn->push_back(ByteCode::initialize_obj);
						ObjectWeReturn->append_int(objID);
					}
					else {
						ObjectWeReturn->push_back(ByteCode::initialize_val);
						ObjectWeReturn->append_int(s_to_i(tree_part.Element));
					}
				}

				tree_part.Delete();
			// append @allocate@ byte 
		}

		ObjectWeReturn->push_back(ByteCode(Type + ByteCode::do_b)); // the type itself
	}
	else if (front_nameID == RETURN) {
		if (Body.size() == 1) { // for void functions (procedures)
			ObjectWeReturn->push_back(ByteCode::ret);
			return ObjectWeReturn;
		}
		auto subvector = new vector<LineElement>(Body.begin() + 1, Body.end());
		TreePart tree_part = build_expr(*subvector);
		delete subvector;
		if (*(tree_part.isOperator)) {
			WholeExpression* temp = tree_part.to_byte_code();
			ObjectWeReturn->append(temp);
			ObjectWeReturn->push_back(ByteCode::retreg);
		}
		else {
			auto objID = GLOBAL_OBJECT_LIST.name_index(tree_part.Element.Name);
			if (objID != UINT32_MAX) {
				ObjectWeReturn->push_back(ByteCode::retobj);
				ObjectWeReturn->append_int(objID);
			}
			else {
				ObjectWeReturn->push_back(ByteCode::retval);
				ObjectWeReturn->append_int(s_to_i(tree_part.Element));
			}
		}
		// whole expression gonna be deleted by "append" method

		tree_part.Delete();
	}
	else { // if there are no matches found, the whole line must be value assignment
		unsigned destobjID = GLOBAL_OBJECT_LIST.name_index(Body[0].Name);
		unsigned procID = GLOBAL_FUNCTION_LIST.name_index(Body[0].Name);

		// searching for equation sign in order to distinguish procedure call and
		// memory writing (wordadr)
		// (theres no more need)

		if (procID != UINT32_MAX && procID > 2) { // is a procedure and NOT wordadr-like funcs

			TreePart tree_part = build_func_expr(Body, procID);
			WholeExpression* whole_expression = tree_part.to_byte_code(GLOBAL_OBJECT_LIST.last_free(), true);
			ObjectWeReturn->append(whole_expression);

			tree_part.Delete();
		}
		else {
			TreePart* tree_part_internal = nullptr; // tree of the expression inside the wordadr() function
			
			if (front_nameID >= DWORDADR && front_nameID <= BYTEADR) {
				
				THROW_ERROR(Body[1].Name != "(", Body[1].Position, err_PARTHOP);
				int internalexp_end = 2;
				int bracks = 1;
				for (; bracks != 0; ++internalexp_end) { // looking inside bracks
					if (Body[internalexp_end].Name == "(") 
						bracks++;
					else if (Body[internalexp_end].Name == ")") 
						--bracks;

					THROW_ERROR(internalexp_end >= Body.size(), Body[internalexp_end].Position, err_PARNTHS);
				}
				// wordadr(address * i + 2) = whole
				auto subvector = new vector<LineElement>(Body.begin() + 2, Body.begin() + internalexp_end - 1);
				tree_part_internal = new TreePart(build_expr(*subvector));
				delete subvector;
			}

			bool isIncrement = Body.back().Name == "++";
			bool isDecrement = Body.back().Name == "--";
			bool isOneOfThem = !(isIncrement || isDecrement);
			
			TreePart tree_part;
			if (isOneOfThem) {
				int equation_sign = 1;
				for (; Body[equation_sign].Name != "="; ++equation_sign) {
					if (equation_sign >= Body.size())
						return nullptr;
				}

				auto subvector = new vector<LineElement>(Body.begin() + equation_sign + 1, Body.end());
				tree_part = build_expr(*subvector); // outer expression
				delete subvector;
			}

			int firstID = GLOBAL_OBJECT_LIST.last_free();

			if (tree_part_internal != nullptr && *(tree_part_internal->isOperator)) {
				WholeExpression* temp = tree_part_internal->to_byte_code();
				ObjectWeReturn->append(temp);		// ID
			}
			if (isOneOfThem  && *(tree_part.isOperator)) {
				WholeExpression* temp = tree_part.to_byte_code();
				ObjectWeReturn->append(temp);
			}
			// write
			// to_adr / to_obj
			// to_adr:[_val / _obj / _reg]
			// [ID]
			// _val / _obj / _reg
			// [ID]

			if (isIncrement) 
				ObjectWeReturn->push_back(ByteCode::increment);
			else if (isDecrement)
				ObjectWeReturn->push_back(ByteCode::decrement);
			else
				ObjectWeReturn->push_back(ByteCode::write); // write

			if (tree_part_internal != nullptr) {
				ObjectWeReturn->push_back(ByteCode::to_adr); // to_adr
				ObjectWeReturn->push_back(ByteCode(front_nameID + ByteCode::do_b));
				if (*(tree_part_internal->isOperator)) {
					if (*(tree_part.isOperator)) {
						ObjectWeReturn->push_back(ByteCode::_obj);
						ObjectWeReturn->append_int(firstID);		// ID
					}
					else ObjectWeReturn->push_back(ByteCode::_reg);
				}
				else {
					auto objID = GLOBAL_OBJECT_LIST.name_index(tree_part_internal->Element.Name);
					if (objID != UINT32_MAX) {
						ObjectWeReturn->push_back(ByteCode::_obj); // _obj
						ObjectWeReturn->append_int(objID);			// ID
					}
					else {
						ObjectWeReturn->push_back(ByteCode::_val); // _val
						ObjectWeReturn->append_int(s_to_i(tree_part.Element)); // iD
					}
					
				}
				tree_part_internal->Delete();
				delete tree_part_internal;
			}
			else {
				ObjectWeReturn->push_back(ByteCode::to_obj); // to_obj
				ObjectWeReturn->append_int(destobjID);		// ID
			}

			if (isOneOfThem) {
				if (*(tree_part.isOperator)) {
					ObjectWeReturn->push_back(ByteCode::_reg); // _reg
				}
				else {
					auto objID = GLOBAL_OBJECT_LIST.name_index(tree_part.Element.Name);
					if (objID != UINT32_MAX) {
						ObjectWeReturn->push_back(ByteCode::_obj); // _obj
						ObjectWeReturn->append_int(objID);			// ID
					}
					else {
						ObjectWeReturn->push_back(ByteCode::_val); // _val
						ObjectWeReturn->append_int(s_to_i(tree_part.Element)); // iD
					}
				}
			}

				
			tree_part.Delete();
			
		}
	}
	return ObjectWeReturn;
}
