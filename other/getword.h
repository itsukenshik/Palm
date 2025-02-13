#pragma once
#include <queue>

int strlen(char* _Ch) {
	int i = 0;
	for (; *(_Ch + i) != '\0'; i++) {}
	return i;
}

unsigned int CHAR_TO_INT(char* _Ch) {
	unsigned int r = 0; // our number that we'll return
	int i = 0; // iterator basically
	int RADIX = 10;
	if (*_Ch == '0' && *(_Ch + 1) == 'x') {
		i += 2; // starting after "0x"
		RADIX += 6; // hexadecimal numbers
	}
	for (; *(_Ch + i) != '\0'; i++) {
		unsigned char t = *(_Ch + i);
		if (t >= '0' && t <= '9')
			r = r * RADIX + (t - ASCII_NUMBER_OFFSET); // adding if 0123456789
		else
			if (t >= 'A' && t <= 'F')
				r = r * RADIX + (t - ASCII_CLETTER_OFFSET + HEX_LETTER_OFFSET); // adding if ABCDEF
			else
				if (t >= 'a' && t <= 'f')
					r = r * RADIX + (t - ASCII_SLETTER_OFFSET + HEX_LETTER_OFFSET); // adding if abcdef
	}
	return r;
}

void stoa(char* a, int num, string& s) {
	for (int i = 0; i < num && i < s.length(); i++) {
		a[i] = s[i];
	}
}

int ContainsThatVar(vector<pair<string, unsigned char>>& dat, std::string& name) {

	for (int i = 0; i < dat.size(); i++) {
		if (dat.at(i).first == name)
			return i;
	}
	return -1;
}

int ContainsThatVar(DATA_HEAP& dat, std::string& name) {
	auto temp = dat.find(name);
	if (temp.has_value())
		return temp.value();
	else return -1;
}

template <class R>

int ContainsThatValue(vector<R>& dat, R& name) {

	for (int i = 0; i < dat.size(); i++) {
		if (dat[i] == name)
			return i;
	}
	return -1;
}

void getToCharFromStr(char* t, string& str, int offset, size_t howm) {
	int i = 0;
	for (; i < howm && i < str.length(); i++) {
		*(t + i) = str[offset + i];
	}
	*(t + i) = '\0';
}

unsigned int STR_TO_INT(string& str) {
	char r[50];
	getToCharFromStr(r, str, 0, str.length());
	return CHAR_TO_INT(r);
}

template<class T>

inline string TO_STR(T val) {
	stringstream buff;
	buff << val;
	return (buff);
}

inline string TO_STRHEX(unsigned int _Int) {
	queue<char> temp;
	string Str;
	for (; _Int > 9; _Int /= 10)
		temp.push(char(0x30 + _Int % 10));
	char i = NULL;
	while (!temp.empty()) {
		Str.push_back(temp.front());
		temp.pop();
	}
	return Str;
}

void WriteString(ofstream& file, string& code) {
	char t[256]; // инициализируем массив символов
	int16_t t_t = int16_t(code.length() / 256); // сколько раз нужно записать символов по 256
	int8_t rest = code.length() % 256; // оставшиеся
	for (int i = 0; i < t_t; i++) {
		getToCharFromStr(t, code, i * 256, 256); // преобразуем string в char[]
		file.write(t, 256); // записываем
	} // следующие 256
	getToCharFromStr(t, code, t_t * 256, rest);
	file.write(t, rest); // записываем всё остальное
}

void getToNSpace(int& s, string& str) {
	for (; s < str.length() && (str[s] == 0 || str[s] == ' ' || str[s] == '\t'); s++) {}
}

void getwordUntilSpace(string& name, string& str, int& s) {
	name.clear();
	for (; s < str.length() && !(str[s] == '\0' || str[s] == ' ' || str[s] == '\t'); s++) {
		name.push_back(str.at(s));
	}
}

void getword(string& name, int& s, string& str) {
	name.clear();
	for (; s < str.length() && !(str[s] == '\0' || str[s] == ' ' || str[s] == '\t' || str[s] == '=' || str[s] == '+' || str[s] == '-' || str[s] == '*' || str[s] == '/' || str[s] == '\\' || str[s] == '(' || str[s] == ')' || str[s] == ':' || str[s] == ',' || str[s] == '&' || str[s] == '{' || str[s] == '}' || str[s] == '[' || str[s] == ']'); s++) {
		name.push_back(str.at(s));
	}
}

void getword(string& name, string& str, int s = 0) {
	name.clear();
	for (; !(str[s] == '\0' || str[s] == ' ' || str[s] == '\t' || str[s] == '\n' || str[s] == '=' || str[s] == '+' || str[s] == '-' || str[s] == '*' || str[s] == '/' || str[s] == '\\' || str[s] == '(' || str[s] == ')' || str[s] == ':' || str[s] == ',' || str[s] == '&') || str[s] == '{' || str[s] == '}' || str[s] == '[' || str[s] == ']'; s++) {
		name.push_back(str.at(s));
	}
}

inline void getToNSpaceIn(int& s, string& cd, int _Howm = 1) {
	s += _Howm;
	getToNSpace(s, cd);
}


inline vector <pair <string, unsigned char>>& CURRDATA() {
	return is_func ? DATA_FUNC : (is_home ? DATA : DATA_PUBL);
}

inline vector <string>& CURROUT() {
	return is_func ? FUNC : OUT;
}

