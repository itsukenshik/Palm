//////////////////////////////////////////////////////
//  This file has been made with purpose to convert //
// complex algebraic expressions to local byte code //
//////////////////////////////////////////////////////

#pragma once
#include <iostream>
#include <optional>
#include <vector>
#include <stack>
#include <deque>
#include "throwerror.h"

inline void multiple(char I, int Times) {
	for (int i = 0; i < Times; i++) std::cout << I;
}

inline bool is_hex_symbol(char& _ch) {
	unsigned char symbol = toupper(_ch);
	return symbol >= 'A' || symbol <= 'F';
}

bool is_digit(string& _string) {
	int i = 0;
	bool isHex = false;
	if (_string.size() > 1) {
		if (_string[0] == '-')
			++i;
		else if (_string.size() > 2 && _string.substr(0, 2) == "0x") {
			isHex = true;
			i += 2;
		}
	}
	
	for (auto i : _string) 
		if (!isdigit(i) || (isHex && is_hex_symbol(i))) return false;
	return true;
}

struct LineElement {
	string Name;
	int Position = 0;
};

unsigned int s_to_i(LineElement& _string) {
	bool isHex = (_string.Name.size() > 2 && _string.Name.substr(0, 2) == "0x");
	bool isNeg = _string.Name[0] == '-';
	unsigned int ret = 0;

	for (int i = 0 + 2 * isHex + isNeg; i < _string.Name.size(); ++i) {
		unsigned char let = _string.Name[i];
		if (isalpha(let)) {
			let = toupper(let);
			THROW_ERROR(let < 'A' || let > 'F', _string.Position + i, err_NCONST);
			let -= 0x37; // 0x41 - 0xF
		}
		else {
			THROW_ERROR(!isdigit(let), _string.Position + i, err_NCONST);
			let -= 0x30;
		}

		ret += let * pow(10 + 6 * isHex, _string.Name.size() - i - 1);
	}

	if (isNeg)
		ret = 0 - ret;

	return ret;
}



class tempDataChunk {
public:
	char* Name;
	char* Type;
public:
	tempDataChunk(vector<LineElement>& Line) {
		Type = new char[Line[1].Name.size() + 1];// reinterpret_cast<char*>(malloc(Line[1].Name.length() + 1)); // Line[1] holds a type
		strcpy(Type, Line[1].Name.c_str());
		//Type = const_cast<char*>(Line[1].Name.c_str());
		Name = new char[Line[2].Name.size() + 1];// reinterpret_cast<char*>(malloc(Line[2].Name.length() + 1)); // and Line[2] must contain an arbitrary name
		strcpy(Name, Line[2].Name.c_str());
		//Name = const_cast<char*>(Line[2].Name.c_str());
	}

	tempDataChunk(string& Type, string& Name) {
		this->Type = new char[Type.size() + 1]; 
		strcpy(this->Type, Type.c_str());
		this->Name = new char[Name.size() + 1];
		strcpy(this->Name, Name.c_str());
	}


	void Delete() {
		delete[] Name;
		delete[] Type;
	}

	inline bool check_for_name(string& _string) {
		return (_string == Name);
	}
};

class TempData : public map<unsigned int, tempDataChunk> {
private:
	unsigned int lastFree;
	unsigned int max_lastFree;
public:
	TempData() {
		lastFree = 0;
		max_lastFree = 0;
	}

	~TempData() {
		for (auto i : *this) {
			i.second.Delete();
		}
	}

	inline bool does_exist(unsigned int _name) {
		for (auto& i : *this) {
			if (i.first == _name) return true;
		}
		return false;
	}

	unsigned int push(tempDataChunk* _n) {
		int oldLast = lastFree;
		this->emplace(lastFree, *_n);
		delete _n;
		while (this->does_exist(++lastFree));
		if (lastFree > max_lastFree)
			max_lastFree = lastFree;
		return oldLast;
	}

	inline unsigned int max_last_free() {
		return max_lastFree;
	}

	inline unsigned int last_free(int _offset) {
		while (this->does_exist(++_offset));
		return _offset;
	}

	inline unsigned int last_free() {
		return lastFree;
	}

	inline void max_last_free_assign(unsigned _int) {
		if (max_lastFree < _int)
			max_lastFree = _int;
	}

	inline void max_last_free_assign_after(unsigned _int) {
		auto temp = last_free(_int);
		if (max_lastFree < temp)
			max_lastFree = temp;
	}

	unsigned int name_index(string& _string) {
		for (auto& i : *this) {
			if (i.second.Name == _string) return i.first;
		}
		return UINT32_MAX;
	}

	inline bool contains_name(string& _string) {
		return (this->name_index(_string) != UINT32_MAX) ? true : false;
	}

	inline void remove(unsigned int _id) {
		this->erase(_id);
		if (_id < lastFree)
			lastFree = _id;
	}
};

class TempDataFunc : public TempData {
public:
	unsigned int hmuch_args(string& _string){
		for (auto& i : *this) {
			if (i.second.Name == _string) {
				return strlen(i.second.Type) - 1;
			}
		}
		return UINT32_MAX;
	}

	inline unsigned int hmuch_args(unsigned& _uint) {
		return strlen(this->at(_uint).Type) - 1;
	}
};
TempData GLOBAL_OBJECT_LIST;
TempDataFunc GLOBAL_FUNCTION_LIST;
TempData GLOBAL_CONST_LIST;
stack<deque<unsigned>> OBJ_STACK;
// in the aforesited object we use types numbers given in the Keywords array


#undef MEMORY 

class WholeExpression : public vector<ByteCode> {
public:
	void append(WholeExpression* Another) {
#ifndef MEMORY
		if (Another == nullptr) return; // dont try to append nullptr!
		for (int i = 0; i < Another->size(); i++) {
			this->push_back(move(*(Another->begin() + i)));
		}

#endif // PREFERENCE MEMORY
#ifdef MEMORY
		while (!Another->empty()) {
			this->push_back(*Another->begin());
			Another->erase(Another->begin());
		}
#endif

		delete Another;
	}

	void add_string(string& String) {
		for (int i = 0; i < String.size(); i++)
			this->push_back(ByteCode(String[i]));
		this->push_back(ByteCode::null);
	}

	void add_string_funcparams(string& String) {
		for (int i = 0; i < String.size(); i++)
			this->push_back(ByteCode(String[i] + 3));
		this->push_back(ByteCode::null);
	}

	void append_int(unsigned int _int) {
		for (int i = 0; i < 4; i++) {
			this->push_back(ByteCode(_int));
			_int >>= 8;
		}
	}
};

void addStringToByteCode(string* String, WholeExpression* Code) {
	for (int i = 0; i < String->length(); i++) {
		Code->push_back(ByteCode(*(String->begin() + i)));
	}
}

void removeSpaces(std::string& Expression) {
	int o = 0;
	while (o < Expression.size())
		if (std::isspace(Expression[o])) {
			Expression.erase(o, 1);
			if (o > 0 && o < Expression.size() - 1) {
				bool First = isdigit(Expression[o - 1]) || isalpha(Expression[o - 1]);
				bool Second = isdigit(Expression[o]) || isalpha(Expression[o]);
				THROW_ERROR(First && Second, o, err_BSTX);
			}
		}
		else o++;
			
} 

const char Operations[] = "+-*/%&|^><=";

const char OpLevels[6][4] = { "%\0\0", "*/\0", "+-\0", "><=", "^|\0", "&\0\0" };
constexpr int Operations_size = 12;
constexpr int OpLevelAmount = 7;

bool isOneOfString(char Char, const char* String) {
	for (unsigned int i = 0; String[i]; i++) {
		if (String[i] == Char) return true;
	}
	return false;
}




// =<>!+-*/%(),[]:\'|&
// =, <, >, !, +, -, *, /, %, &, |, ==, <<, >>, &&, ||, <=, >=, !=
const char* SIGNS[][4] = { {"!", "~"}, {"*", "/", "%"}, {"+", "-"},
					{ "<<", ">>"}, {"<", ">", "<=", ">="}, {"==", "!="}, {"&"},
					{"^"}, {"|"}, {"&&"}, {"||"} };// {"+=", "-=", "*=", "/=", "%=", "&=", "|=", "^="}};
constexpr unsigned int precedence_levels = sizeof(SIGNS) / sizeof(SIGNS[0]);
unsigned int how_much_in_prec_lev[] = {2, 3, 2, 2, 4, 2, 1, 1, 1, 1, 1};

const char* SIGNS_no_precedence[] = { "!", "~", "*", "/", "%" , "+", "-" ,
 "<<", ">>", "<", ">", "<=", ">=", "==", "!=", "&", "^", "|", "&&",
	"||" };// "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=" };

constexpr unsigned int signs_amount = sizeof(SIGNS_no_precedence) / sizeof(SIGNS_no_precedence[0]);

int whichOneOfArray(string& _string, const char** _array, const int& _size) {
	for (unsigned int i = 0; i < _size; i++) {
		if (_array[i] == _string) return i;
	}
	return -1;
}

inline bool isOneOfArray(string& _string, const char** _array, const int& _sizeofarray) {
	if (whichOneOfArray(_string, _array, _sizeofarray) != -1)
		return true;
	return false;
}

int find_same_level_bracket(vector<LineElement>& Expression, int _Begin) {
	int bracket_level = 1;
	while (bracket_level != 0) {
		if (Expression[_Begin].Name == "(") {
			if (!bracket_level++) continue;
		}
		else if (Expression[_Begin].Name == "(") {
			if (!--bracket_level) continue;
		}
		_Begin++;
		THROW_ERROR(_Begin < Expression.size(), Expression.back().Position, err_PARNTHS);
	}
	return _Begin;
}



inline bool is_obj(string& _string) {
	return GLOBAL_OBJECT_LIST.contains_name(_string);
}

inline bool is_func(string& _string) {
	return GLOBAL_FUNCTION_LIST.contains_name(_string);
}

WholeExpression* build_from_lineelements(std::vector<LineElement>& _expression);

class LinePart {
private:
	std::vector<LineElement>::iterator begin;
	std::vector<LineElement>::iterator end;
public:
	LinePart(std::vector<LineElement>::iterator _begin, std::vector<LineElement>::iterator _end) {
		begin = _begin;
		end = _end;
	}

	// get rid of external parentheses
	void cut_parentheses() {
		while (begin->Name == "(") {
			THROW_ERROR(end->Name == ")", begin->Position, err_PARNTHS);
			begin++; end--;
		}
	}

	// does this linepart include one single lineelement
	bool has_single_object() {
		return begin == end; 
	}

	std::vector<LineElement>::iterator Begin() {
		return begin;
	}

	std::vector<LineElement>::iterator End() {
		return begin;
	}
};

unsigned optimize_two_consts(LineElement* _main, int _op, LineElement* _sec = nullptr) {
	// _op = i * size + j
	int main = s_to_i(*_main);
	int sec = 0;
	if (_op > 1) { // if the operator is "!" or "~"
		sec = s_to_i(*_sec);
	}
	switch (_op) {
	case 0: // !
		return !main;
		break;
	case 1: // ~
		return ~main;
		break;
	case 2: // *
		return main * sec;
		break;
	case 3: // /
		return main / sec;
		break;
	case 4: // %
		return main % sec;
		break;
	case 5: // +
		return main + sec;
		break;
	case 6: // -
		return main - sec;
		break;
	case 7: // <<
		return main << sec;
		break;
	case 8: // >>
		return main >> sec;
		break;
	case 9: // <
		return main < sec;
		break;
	case 10: // >
		return main > sec;
		break;
	case 11: // <=
		return main <= sec;
		break;
	case 12: // >=
		return main >= sec;
		break;
	case 13: // ==
		return main == sec;
		break;
	case 14: // !=
		return main != sec;
		break;
	case 15: // &
		return main & sec;
		break;
	case 16: // ^
		return main ^ sec;
		break;
	case 17: // |
		return main | sec;
		break;
	case 18: // &&
		return main && sec;
		break;
	case 19: // ||
		return main || sec;
		break;
	default:
		return 1;
		break;
	}
}

ByteCode proper_bc(int _a, int _error_pos = 100) {
	switch (_a) {
	case 0: // !
		return(ByteCode::notl);
		break;
	case 1: // ~
		return(ByteCode::notb);
		break;
	case 2: // *
		return(ByteCode::mul);
		break;
	case 3: // /
		return(ByteCode::Div);
		break;
	case 4: // %
		return(ByteCode::mod);
		break;
	case 5: // +
		return(ByteCode::add);
		break;
	case 6: // -
		return(ByteCode::sub);
		break;
	case 7: // <<
		return(ByteCode::shl);
		break;
	case 8: // >>
		return(ByteCode::shr);
		break;
	case 9: // <
		return(ByteCode::lss);
		break;
	case 10: // >
		return(ByteCode::lgr);
		break;
	case 11: // <=
		return(ByteCode::lsseq);
		break;
	case 12: // >=
		return(ByteCode::lgreq);
		break;
	case 13: // ==
		return(ByteCode::eql);
		break;
	case 14: // ==
		return(ByteCode::neq);
		break;
	case 15: // &
		return(ByteCode::andb);
		break;
	case 16: // ^
		return(ByteCode::xorb);
		break;
	case 17: // |
		return(ByteCode::orb);
		break;
	case 18: // &&
		return(ByteCode::andl);
		break;
	case 19: // ||
		return(ByteCode::orl);
		break;
	default:
		THROW_ERROR(true, _error_pos, err_UNKN);
		break;
	}
}

struct OperatorValue {
	unsigned Value;
	bool IsAddress;
};

class Operator {
private:
	vector<LineElement>::iterator iter;
	optional<OperatorValue> Value;
public:
	Operator(vector<LineElement>::iterator& _it) {
		iter = _it;
	}

	vector<LineElement>::iterator& it() {
		return iter;
	}

	unsigned& address() {
		if (Value.has_value())
			return Value.value().Value;

		throw (-1);
	}

	inline void assign_address(unsigned& _int) {
		Value->Value = _int;
		Value->IsAddress = true;
	}

	inline void assign_const(unsigned& _int) {
		Value->Value = _int;
		Value->IsAddress = false;
	}

	inline bool has_value() {
		return Value.has_value();
	}
};

const char* Keywords[] = { "then", "allocate",
"int", "word", "byte", "adr", "dwordadr", "wordadr", "byteadr", "signed_int", "signed_word", "signed_byte", "qword", "qwordadr", "signed_qword",
						"while", "until", "home", "return", "cycle", "allocarray", "const", "constarray", "macro", "true", "false", "asm"};

constexpr int Keywords_size = sizeof(Keywords) / sizeof(Keywords[0]);

#define Types_begin 2
#define Types_end 15

unsigned checkForType(string& _string) {
	unsigned int Type = Types_begin;
	while (Keywords[Type] != _string) {
		if (Type == Types_end) return UINT32_MAX;
		Type++;
	}
	return Type;
};

class OperatorVector : public vector<Operator> {
public:
	void push(vector<LineElement>::iterator& _it) {
		this->push_back(Operator(_it));
	}
};
/*
// should we replace "treepart" with "WholeExpression" here? - done!
WholeExpression* build_from_lineelements(std::vector<LineElement>& _expression) {
	WholeExpression* ret = new WholeExpression;
	// a+foo(5, 0x86, bar(56))*speed
	// a		+		foo		(	
	// 5		,		0x86	,
	// bar		(		56		)
	// )		*		speed
	int bracket_level = 0;
	OperatorVector operators;
	
	for (auto i = _expression.begin(); i != _expression.end(); i++) { // we conserve iterators that refer to operator line elements

		if (i->Name == "(") bracket_level++;
		else if (i->Name == ")")
			THROW_ERROR(--bracket_level < 0, i->Position, err_PRTS);

		if (isOneOfArray(i->Name, SIGNS_no_precedence) && bracket_level == 0)
			operators.push(i);
	}

	// in order not to check all of precedence levels when there are actually much less operators,
	// we will terminate the process when we check all of the operators
	int signs_need_to_proc = int(operators.size());
	

	// here we take every level of precedence and check "operators" for one of
	// included operator. If there is one, then we 
	for (int i = 0; i < precedence_levels; ++i) {
		for (int j = 0; j < operators.size(); ++j) {
			//same level-operators in row (how much of them)
			int inarow = 0;
			bool left_is_calculated = false;
			if (j) { // not the first one
				left_is_calculated = operators[j - 1].has_value();
			}

			bool right_is_calculated = false;
			if (j != operators.size() - 1) { // not the last one
				right_is_calculated = operators[j + 1].has_value();
			}
			
		
			if (isOneOfArray(operators[j].it()->Name, SIGNS[i])) {
				inarow++;

				LinePart Left((j > 0) ? (operators[j - 1].it() + 1) : _expression.begin(),
					operators[j].it() - 1);

				LinePart Right(operators[j].it() + 1,
					(j != operators.size() - 1) ? (operators[j + 1].it() - 1) : _expression.end());

				Left.cut_parentheses();

				Right.cut_parentheses();



				auto proper_bc = [&Left](int a) {
					switch (a) {
					case 0: // !
						return(ByteCode::notl);
						break;
					case 1: // ~
						return(ByteCode::notb);
						break;
					case 4: // *
						return(ByteCode::mul);
						break;
					case 5: // /
						return(ByteCode::Div);
						break;
					case 6: // %
						return(ByteCode::mod);
						break;
					case 8: // +
						return(ByteCode::add);
						break;
					case 9: // -
						return(ByteCode::sub);
						break;
					case 12: // <<
						return(ByteCode::shl);
						break;
					case 13: // >>
						return(ByteCode::shr);
						break;
					case 16: // <
						return(ByteCode::lss);
						break;
					case 17: // >
						return(ByteCode::lgr);
						break;
					case 18: // <=
						return(ByteCode::lsseq);
						break;
					case 19: // >=
						return(ByteCode::lgreq);
						break;
					case 20: // ==
						return(ByteCode::eql);
						break;
					case 24: // &
						return(ByteCode::andb);
						break;
					case 28: // ^
						return(ByteCode::xorb);
						break;
					case 32: // |
						return(ByteCode::orb);
						break;
					case 36: // &&
						return(ByteCode::andl);
						break;
					case 40: // ||
						return(ByteCode::orl);
						break;
					default:
						THROW_ERROR(true, Left.Begin()->Position, err_UNKN);
						break;
					}
				};
				int current_operatorID = i * how_much_in_prec_lev + whichOneOfArray(operators[j].it()->Name, SIGNS[i]);
				
				if (right_is_calculated && left_is_calculated) {
					ret->push_back(ByteCode::_2obj);
					ret->push_back(ByteCode(int(proper_bc(current_operatorID))));
					auto left_adr = operators[j - 1].address();
					auto right_adr = operators[j + 1].address();
					ret->push_back(ByteCode(left_adr));
					ret->push_back(ByteCode(right_adr));
				}
				
				
				
				bool is_left_single = Left.has_single_object();
				bool is_right_single = Right.has_single_object();

				if (is_left_single && is_right_single) {
					
					bool is_left_obj = is_obj(Left.Begin()->Name);
					bool is_right_obj = is_obj(Right.Begin()->Name);
					// two parameters: operation and object factor
					if (is_left_obj) { // either obj + obj or obj + const
						if (is_right_obj)
							ret->push_back(ByteCode::_2obj);
						else
							ret->push_back(ByteCode::_obj);
						

					}
					else if (!(is_left_obj || is_right_obj)) { // const + const (automatically optimized)
						THROW_ERROR(is_digit(Left.Begin()->Name), Left.Begin()->Position, err_NCONST);
						THROW_ERROR(is_digit(Right.Begin()->Name), Right.Begin()->Position, err_NCONST);
						unsigned value;
						if (i > 1)
							value = optimize_two_consts(&Left.Begin()->Name, current_operatorID, &Right.Begin()->Name);
						else
							value = optimize_two_consts(&Right.Begin()->Name, current_operatorID);
						operators[j].assign_const(value);
						continue;
					}
					else { // const + obj
						ret->push_back(ByteCode::_byobj);
						// требуется таблица, которая бы переворачивала знак там, где это надо
						// там, где не надо этого и где не важен порядок объектов, нужно поменять их местами
						// там, где порядок важен (-, /, <<, >>, %), использовать byobj-коды
						// ahahahh nope ничего уже не надо ≧▽≦
						
					}

					ret->push_back(ByteCode(int(proper_bc(current_operatorID)) + !is_right_obj)); // and 1 if obj + const

					if (i) // left part is excluded if we're dealing with the operator of either ! or ~
						ret->push_back(ByteCode(GLOBAL_OBJECT_LIST.name_index(Left.Begin()->Name)));
					ret->push_back(is_right_obj ?
						ByteCode(GLOBAL_OBJECT_LIST.name_index(Right.Begin()->Name)) :
						ByteCode(atoi(Right.Begin()->Name.c_str())));

				}
				else { // both left'n'righ' migh bi whol espressine in paranthisiz or funkshon!
					if (!is_left_single && !is_right_single) {
						vector<LineElement> expression(Right.Begin(), Right.End() + 1);
						WholeExpression* One(move(build_from_lineelements(expression)));
						ret->append(One);
						delete One;

						ret->push_back(ByteCode::allocreg);
						unsigned int rightID = GLOBAL_OBJECT_LIST.last_free();

						expression = vector<LineElement>(Left.Begin(), Left.End() + 1);
						One = (move(build_from_lineelements(expression)));
						ret->append(One);
						delete One;

						ret->push_back(ByteCode::_rgobj);
						ret->append_int(GLOBAL_OBJECT_LIST.last_free());

					}
					else if (is_left_single && !is_right_single) {
						vector<LineElement> expression(Right.Begin(), Right.End() + 1);
						WholeExpression* One(move(build_from_lineelements(expression)));
						ret->append(One);
						delete One;

						if (is_obj(Left.Begin()->Name)) {
							ret->push_back(ByteCode::_objbyreg);
							ret->push_back(ByteCode(proper_bc(current_operatorID))); 
							ret->append_int(GLOBAL_OBJECT_LIST.name_index(Left.Begin()->Name));
						}
					}
					else if (!is_left_single && is_right_single) { // expr + obj, expr + const
						
						vector<LineElement> expression(Left.Begin(), Left.End() + 1);
						WholeExpression* One(move(build_from_lineelements(expression)));
						ret->append(One);
						delete One;

						int address = 0;
						if (is_obj(Right.Begin()->Name)) { // object
							ret->push_back(ByteCode::_rgobj);
							address = GLOBAL_OBJECT_LIST.name_index(Left.Begin()->Name);
						}
						else {
							ret->push_back(ByteCode::_rgconst); // const
							THROW_ERROR(is_digit(Right.Begin()->Name), Right.Begin()->Position, err_NCONST);
							address = atoi(Left.Begin()->Name.c_str());
						}
						ret->push_back(ByteCode(proper_bc(current_operatorID)));
						ret->append_int(address);
					}
					
					
					vector<LineElement> expression;
					if (!is_left_single)
						vector<LineElement> expression(Left.Begin(), Left.End() + 1);
					else
						vector<LineElement> expression(Right.Begin(), Right.End() + 1);

					WholeExpression* One(move(build_from_lineelements(expression)));
					ret->append(One);
					delete One;
				}
				// have processed
				signs_need_to_proc--;
			} 
			else {
				// j = 0 does not count bc it is the first operator
				// j = 1 does not count bc there are at least 2 ops in a same-level expr
				// and in that very case j must equal 2 (the first non-included op)
				// the last expr member must be at (j - 1)
				// and the first at (j - inarow - 1)
				if (j > 1) {
					operators[j - inarow - 1].address() = operators[j - 1].address();
				}
				inarow = 0;
			}
		}
		// hav we processed them all??!
		if (!signs_need_to_proc) break;
	}
	return ret;
}


*/

constexpr auto REFERENCE_DESIGNATION = "&o";

class TreePart {
public:
	LineElement Element;
	const bool* isOperator;
	TreePart* Left;
	TreePart* Right;
	// for functions (array of tree parts, as one argument may be an entire expression with its own root system)
	TreePart* Args;

	TreePart() {
		isOperator = nullptr;
		Left = nullptr;
		Right = nullptr;
		Args = nullptr;
	}

	void Delete() {
		if (isOperator != nullptr)
			delete isOperator;
		if (Left != nullptr) {
			Left->Delete();
			delete Left;
		}
		if (Right != nullptr) {
			Right->Delete();
			delete Right;
		}
		if (Args != nullptr) {
			unsigned argc;
			if (this->Element.Name == REFERENCE_DESIGNATION)
			argc = 1;
			else
			argc = GLOBAL_FUNCTION_LIST.hmuch_args(this->Element.Name);

			for (int i = 0; i < argc; ++i) {
 				(Args + i)->Delete();
				 // removing the array of Args;
			}
			delete[] Args;
		}
	}

	WholeExpression* to_byte_code(int _address = GLOBAL_OBJECT_LIST.last_free(), bool isProcess = false) {
		WholeExpression* Ret = new WholeExpression;
		unsigned size = signs_amount;
		int currentOperator = whichOneOfArray(Element.Name, SIGNS_no_precedence, size);
		bool MainIsComplex = true, SecdIsComplex = true;
		unsigned mainID = 0;
		unsigned secdID = 0;
		unsigned funcID = GLOBAL_FUNCTION_LIST.name_index(Element.Name);
		if (!(*isOperator)) { // if current branch is no more than a value 
			unsigned const_type = GLOBAL_OBJECT_LIST.name_index(Element.Name);
			// const strings!
			/*if (const_type != UINT32_MAX) {
				Ret->push_back(ByteCode::obj_address);
				Ret->append_int(const_type);
			} else */return nullptr;
		} // we do nothing
		else if (this->Element.Name == REFERENCE_DESIGNATION) {
			Ret->push_back(ByteCode::obj_address);
			Ret->append_int(GLOBAL_OBJECT_LIST.name_index(Args->Element.Name));
		}
		else if (funcID == UINT32_MAX){

			WholeExpression* Sub;
			Sub = Left->to_byte_code(_address);
			if (Sub == nullptr) {
				MainIsComplex = false;
				mainID = GLOBAL_OBJECT_LIST.name_index(Left->Element.Name);
			}
			else
				Ret->append(Sub);

			int next_last_free = GLOBAL_OBJECT_LIST.last_free(_address);
			Sub = Right->to_byte_code(next_last_free);
			if (Sub == nullptr) {
				SecdIsComplex = false;
				secdID = GLOBAL_OBJECT_LIST.name_index(Right->Element.Name);
			}
			else
				Ret->append(Sub);

			if (MainIsComplex || mainID != UINT32_MAX)
				if (SecdIsComplex || secdID != UINT32_MAX)
					Ret->push_back(ByteCode::_2obj); // obj + obj
				else
					Ret->push_back(ByteCode::_obj); // obj + const
			else
				if (SecdIsComplex || secdID != UINT32_MAX)
					Ret->push_back(ByteCode::_byobj); // const + obj
				else { // const + const 
					THROW_ERROR(Left == nullptr && Right == nullptr && Args == nullptr, Element.Position, err_GOTNOARGS);
					THROW_ERROR(!is_digit(Left->Element.Name), Left->Element.Position, err_NCONST);
					THROW_ERROR(!is_digit(Right->Element.Name), Right->Element.Position, err_NCONST);
					unsigned value;
					if (currentOperator > 1)
						value = optimize_two_consts(&Left->Element, currentOperator, &Right->Element);
					else
						value = optimize_two_consts(&Right->Element, currentOperator);
					
					char* buffer = new char[int_length(value) + 1];
					_itoa(value, buffer, 10);
					Element.Name = buffer;
					delete[] buffer;
					delete Right;
					delete Left;
					delete isOperator;
					isOperator = new bool(false); // no more an operator
					return Ret;
				}

			// max last free assignment
			if (MainIsComplex || SecdIsComplex)
				if (MainIsComplex && SecdIsComplex)
					GLOBAL_OBJECT_LIST.max_last_free_assign_after(next_last_free);
				else
					GLOBAL_OBJECT_LIST.max_last_free_assign(next_last_free);


			Ret->push_back(proper_bc(currentOperator));
			if (MainIsComplex) 
				Ret->append_int(_address);
			else if (mainID != UINT32_MAX) {
				Ret->append_int(mainID);
			}
			else
				Ret->append_int(s_to_i(Left->Element));
				

			if (SecdIsComplex) {
				if (MainIsComplex)
					Ret->append_int(next_last_free);
				else
					Ret->append_int(_address);

			}
			else if (secdID != UINT32_MAX) {
				Ret->append_int(secdID);
			}
			else
				Ret->append_int(s_to_i(Right->Element));
		}
		else {

			if (funcID < 3) {// dwordadr, wordadr...
				WholeExpression* temp = Args->to_byte_code();
				if (temp == nullptr) {
					auto argID = GLOBAL_OBJECT_LIST.name_index(Args->Element.Name);
					if (argID != UINT32_MAX) {
						Ret->push_back(ByteCode(unsigned char(ByteCode::dwordadr_obj) + funcID)); // 39 - dwordadr_obj
						Ret->append_int(argID);
					}
					else {
						Ret->push_back(ByteCode(unsigned char(ByteCode::dwordadr_val) + funcID)); // 36 - dwordadr_val
						Ret->append_int(s_to_i(Args->Element));
					}
				}
				else {
					Ret->append(temp);
					Ret->push_back(ByteCode(unsigned char(ByteCode::dwordadr_reg) + funcID)); // 42 - dwordadr_reg
				}
				return Ret;
			}

			Ret->push_back(ByteCode(char(ByteCode::callret) - isProcess));
			Ret->append_int(funcID); // wonderful!!! (turned out that it'd be better for
									// this part of code to be there in order to simplify
									// the processing of bytecode

			unsigned argc = GLOBAL_FUNCTION_LIST.hmuch_args(this->Element.Name);
			for (int i = 0; i < argc; ++i) {
				WholeExpression* temp = (Args + i)->to_byte_code();
				if (temp == nullptr) { // single value or object
					auto argID = GLOBAL_OBJECT_LIST.name_index((Args + i)->Element.Name);
					if (argID != UINT32_MAX) {
						Ret->push_back(ByteCode::argobj);
						Ret->append_int(argID);
					}
					else {
						Ret->push_back(ByteCode::argconst);
						Ret->append_int(s_to_i((Args + i)->Element));
					}
						
				} else {
					Ret->append(temp);
					Ret->push_back(ByteCode::argreg);
				}
			}
		}
		return Ret;
	}

	void ShortToByteCode(WholeExpression* Expression, void* num) {
		char* temp = static_cast<char*>(num);
		Expression->push_back(ByteCode(*temp));
		Expression->push_back(ByteCode(*(temp + 1)));
	}
};


TreePart build_expr(vector<LineElement>& _expression);

void addBranch(vector<TreePart>& BranchLevel, vector<LineElement>& Subexpr) {
	TreePart* One = new TreePart;
	*One = build_expr(Subexpr);
	BranchLevel.push_back(std::move(*One)); //making a new branch of what we got from adding symbol by symbol
}

// g * (a + 39) - e
// _____  ___  _____  ___  _____
// | g |--|*|--|   |--|-|--| e |	level 0
// -----  ---  -----  ---  -----
//               |
//       _____  ___  _____
//       | a |--|+|--|39 |			sub-level
//       -----  ---  -----

TreePart build_func_expr(vector<LineElement>& _function, unsigned funcID) {
	auto size = GLOBAL_FUNCTION_LIST.hmuch_args(funcID);
	THROW_ERROR(_function.size() < 3, _function[0].Position, err_BFUNCSEM);
	THROW_ERROR(_function[1].Name != "(", _function[1].Position, err_BFUNCSEM);
	THROW_ERROR(_function.back().Name != ")", _function.back().Position, err_PARNTHS);
	TreePart* ret = new TreePart;

	unsigned Arguments = 0; // analyzing the function call
	int Bracks = 1;
	// the beginning of our next internal expression (End gonna be its end)
	unsigned Beginning = 2;
	unsigned End = 2;
	if (size != 0) {
	vector<TreePart>* ArgsTemp = new vector<TreePart>;
	do {
		bool isLastBrack = false;
		if (_function[End].Name == "(") {
			Bracks++;
		}
		else if (_function[End].Name == ")") {
			THROW_ERROR(--Bracks < 0, _function[End].Position, err_PARNTHS);
			isLastBrack = true;
		}

		if (Bracks < 2 && (_function[End].Name == "," || isLastBrack)) { // 1 level of brack and either came across a comma or last parenthesis
			THROW_ERROR(++Arguments > size, _function[End].Position, err_TMOP);
			vector<LineElement> substr(_function.begin() + Beginning, _function.begin() + End);
			ArgsTemp->push_back(build_expr(substr));
			if (isLastBrack)
				break;
			Beginning = End + 1; // the beginning of the next expression
		}

		End++;
	} while (End < _function.size());

	
	THROW_ERROR(Arguments < size, _function[End].Position, err_TLOP);

	ret->Args = new TreePart[ArgsTemp->size()];
	for (int i = 0; i < ArgsTemp->size(); ++i)
		ret->Args[i] = ArgsTemp->at(i);
	delete ArgsTemp;
	}



	ret->Element = _function[0];
	ret->isOperator = new bool(true); // as far as function has its arguments and its computions, jsut like operations, we may regard it as one
	return move(*ret);
}

TreePart build_expr(vector<LineElement>& _expression) { // разделяем выражение на скобки
	if (_expression.size() == 1) {
		TreePart* Ret = new TreePart; //returning the whole string if there arent any signs
		Ret->Element = _expression[0];
		Ret->isOperator = new bool(false); // is not operator
		/*if (is_obj(Value)) { // if IS object
			Ret->Value.value()->Value = GLOBAL_OBJECT_LIST.name_index(Value);
			Ret->Value.value()->isObject = true;
		}
		else { // if IS a constant value
			THROW_ERROR(is_digit(Value), _expression[0].Position, err_NCONST);
			Ret->Value.value()->Value = atoi(Value.c_str());
			Ret->Value.value()->isObject = false;
		}*/
		return *Ret;
	}
	int bracket_level = 0;

	// it might bi a function
	// wordadr(a) + byteadr(0x56) * (parent + u)
	unsigned funcID = GLOBAL_FUNCTION_LIST.name_index(_expression[0].Name);
	

	
	vector<TreePart> BranchLevel;
	vector<string> Signs;
	int offset = 0;
	bool is_last_not_a_reference = true;
		for (auto i = _expression.begin(); i != _expression.end(); i++) { // we conserve iterators that refer to operator line elements
			bool brack_just_closed = false;
			if (i->Name == "(") {
				bracket_level++;
			} 
			else if (i->Name == ")") {
				THROW_ERROR(--bracket_level < 0, i->Position, err_MISSPRTH);
				brack_just_closed = true;
			}
			const int size = signs_amount;

			// references 
			if (i->Name == "&" && // address operator?
				((i + 1) != _expression.end() && is_obj((i + 1)->Name) && // is the following lineelement an object?
					(i == _expression.begin() || isOneOfArray((i - 1)->Name, SIGNS_no_precedence, size)))) // is the preceding lineelement an operator or does not exist at all?
			{ // references
				// &ref
				TreePart temp;
				temp.Args = new TreePart[1]; // using delete[], we delete ONLY arrays
				temp.Element.Name = REFERENCE_DESIGNATION; // &a Name => Argument is an referred object
				temp.Element.Position = i->Position;
				temp.isOperator = new bool(true);
				temp.Args->Element = *(i + 1); // argument (ref)
				temp.Args->isOperator = new bool(false);
				BranchLevel.push_back(temp);
				i++;
				is_last_not_a_reference = false;
				continue;
			}

			if (bracket_level == 0 && (isOneOfArray(i->Name, SIGNS_no_precedence, size))) {
				if (is_last_not_a_reference) {
					auto beg = i - offset;
					auto fin = i;
					while (beg->Name == "(" && (fin - 1)->Name == ")") {
						beg++; fin--; // removing extra brackets
					}
					vector<LineElement> substr(beg, fin);
					BranchLevel.push_back(std::move(build_expr(substr)));
				}
				else is_last_not_a_reference = true;
				
				
				Signs.push_back(i->Name); // pushing all signs into one vector string to use them as strings
				offset = 0; // nullification
			}
			else {
				offset++;
			}
		}

	if (!Signs.empty()) { // if there are any signs, we ultimately cut the last part of string
		if (is_last_not_a_reference) {
			auto beg = _expression.end() - offset;
			auto fin = _expression.end();
			while (beg->Name == "(" && (fin - 1)->Name == ")") {
				beg++; fin--; // removing extra brackets
			}
			vector<LineElement> substr(beg, fin);
			BranchLevel.push_back(std::move(build_expr(substr)));
		}
	}
	else {//returning the whole string if there arent any signs
		if (funcID != UINT32_MAX) {
			return build_func_expr(_expression, funcID);
		}
		if (!BranchLevel.empty())
			return BranchLevel[0];
		TreePart* Ret = new TreePart; //returning the whole string if there arent any signs
		Ret->Element = _expression[0];
		Ret->isOperator = new bool(false); // is not operator
		return *Ret;
	}
	//we got all no-signed parts at this point,
	//now we allocate them by sign strength
	//first is mod (no)
	for (unsigned int u = 0; u < precedence_levels; u++) {
		int i = 0;
		while (i < Signs.size()) {
			 // checking for sign superiority
			const int size = how_much_in_prec_lev[u];
			if (isOneOfArray(Signs[i], SIGNS[u], size)) { // if this step the sign is superior enough,
				TreePart Temp; // we combine two branches into one
				Temp.Left = new TreePart;
				*Temp.Left = BranchLevel[i];
				Temp.Right = new TreePart;
				*Temp.Right = BranchLevel[i + 1];
				Temp.Element.Name = Signs[i];
				Temp.isOperator = new bool(true); // IS operator
				auto tempPos = BranchLevel.begin() + i;
				BranchLevel.erase(tempPos, tempPos + 2);
				BranchLevel.insert(BranchLevel.begin() + i, std::move(Temp));
				Signs.erase(Signs.begin() + i);
			}
			else i++;
		}
		if (BranchLevel.size() == 1) break;
	}
	return std::move(BranchLevel.at(0));
}


WholeExpression* DWL(vector<LineElement>& _exp) {
	TreePart root = build_expr(_exp);
	WholeExpression* ret = root.to_byte_code();
	root.Delete();
	return ret;
}