#pragma once
#include <vector>
/*void PreProcess(std::vector<string>& code_vector) {
	string name;
	line = 0;
	while (line < code_vector.size()) {
		int row_start = 0;
		code = code_vector[line];
		getToNSpace(row_start, code);
		if (code[row_start] == '$') {
			code_vector.erase(code_vector.begin() + line);

			getToNSpaceIn(row_start, code);
			if (code.substr(row_start, 4) == "BOOT") {
				THROW_ERROR(is_boot, row_start, err_BTTW);
				for (string f : BOOT) {
					CURROUT().push_back(f);
				}
				is_boot = true;
			}
			else if (code.substr(row_start, 4) == "MEAN") {
				string to_get_replaced, to_replace_with;
				getToNSpaceIn(row_start, code, 5);
				getword(to_get_replaced, row_start, code);

				for (int iw = 0; iw < to_get_replaced.length(); iw++) {
					THROW_ERROR(!(std::isalpha(to_get_replaced.at(iw)) || (to_get_replaced.at(iw) == '_')), iw, err_BVRN);
				}

				getToNSpaceIn(row_start, code);
				for (; row_start < code.length(); row_start++) {
					to_replace_with.push_back(code[row_start]);
				}
				int diff = int(to_get_replaced.length() - to_replace_with.length());
				for (int ie = line; ie < code_vector.size(); ie++) {
					row_start = 0;
					while (row_start < code_vector[ie].size())//row start
					{
						
						if (std::isalpha(code_vector[ie][row_start]) || code_vector[ie][row_start] == '_') {
							getword(name, row_start, code_vector[ie]);
							int where_started = row_start - (int)to_get_replaced.length();
							if (name == to_get_replaced) {
								code_vector[ie].erase(code_vector[ie].begin() + where_started, code_vector[ie].begin() + row_start); // erasing old word
								if (where_started > code_vector[ie].length()) // if it went out of vector range
									where_started = (int)code_vector[ie].length();
								code_vector[ie].insert(where_started, to_replace_with); // writing new one
							}
						}
						else
							row_start++;

						getToNSpace(row_start, code_vector[ie]);
					}
				}
			}
			else if (code.substr(row_start, 3) == "GET") {
				name.clear();
				for (row_start += 4; row_start < code.length(); row_start++) {
					name.push_back(code[row_start]);
				}
				ifstream GET; // достаём требующийся файл
				GET.open(name);

				THROW_ERROR(!GET.is_open(), row_start - (int)name.length(), err_FERR);

				int lineGET = line;
				while (!GET.eof()) { // 
					bool is_comment = false;
					char f = 0;
					code.clear();
					while (f != '\n' && !GET.eof()) {
						f = GET.get();
						if (f != '\n' && !GET.eof() && !is_comment)
							if (f != '#') // comment?
								code.push_back(f);
							else
								is_comment = true;


					}
					if (!code.empty()) { // if there's anything to write
						code_vector.insert(code_vector.begin() + lineGET++, code);

					}
				}
				GET.close();
			}

		}
		else line++;
	}
	
}*/

