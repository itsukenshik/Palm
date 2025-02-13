#pragma once
#include <iostream>
int error = 0;
int error_cause = 0;

#define BOOT_SIZE 39
#define ASCII_NUMBER_OFFSET 0x30
#define ASCII_CLETTER_OFFSET 0x41
#define ASCII_SLETTER_OFFSET 0x61
#define HEX_LETTER_OFFSET 0xA
using namespace std;

const string ERRORS[] = {
	"GOOD", // 0
#define err_GOOD 0
"UNKNOWN SIGN", // 1
#define err_UNKS 1
"BAD SYMBOL", // 2
#define err_BSYM 2
"HAS ALREADY DECLAIRED", // 3
#define err_HALD 3
"BAD SYNTAX", // 4
#define err_BSTX 4
"BOOT DIRECTIVE TWICE", // 5
#define err_BTTW 5
"UNKNOWN TYPE", // 6
#define err_UNKT 6
"UNKNOWN VARIABLE" // 7
#define err_UNKV 7
, "FILE ERROR" // 8
#define err_FERR 8
, "DECLAIRING FUNCTION INSIDE HOME SECTION" // 9
#define err_DFIH 9
, "DECLAIRING FUNCTION INSIDE ANOTHER FUNCTION" // 10
#define err_DFIF 10
, "UNKNOWN" // 11
#define err_UNKN 11 
, "UNKNOWN FUNCTION" // 12
#define err_UNFC 12
, "CANT USE VOID FUNCTION AS OPERAND" // 13
#define err_CUFO 13
, "TOO MUCH OPERANDS" // 14
#define err_TMOP 14
, "TOO LITTLE OPERANDS" // 15
#define err_TLOP 15
, "USING NOTH FUNCTION AS OPERAND" // 16
#define err_UNAO 16
, "EXPRESSION REQUIRED" // 17
#define err_EXRQ 17
, "BAD VARIABLE NAME" // 18
#define err_BVRN 18
, "CAN'T PUT STRING INTO SHORTER STRING" // 19
#define err_CSIS 19
, "STR LENGTH MUST BE MORE THAN ZERO" // 20
#define err_SLMZ 20
, "UNEXPECTED SYMBOL" // 21
#define err_UNXPSYM 21
, "BAD ALLOCATE UTILITY USAGE" // 22
#define err_BAUU 22
, "EXPECTED AN \'=\' SIGN" // 23
#define err_EQUSIGN 23
, "EXPECTED A COMMA" // 24
#define err_EXCM 24
, "EXPRESSION NOT CLOSED: EXPECTED A PARENTHESIS" // 25
#define err_PARNTHS 25
, "NUMERIC CONSTANT WAS NOT CLARIFIED PROPERLY" // 26
#define err_NCONST 26
, "BAD FUNCTION CALL SEMANTICS" // 27
#define err_BFUNCSEM 27
, "FUNCTION REQUIRES PARENTHESES OPERATOR" // 28
#define err_PARTHOP 28
, "MISSED AN OPENING PARENTHESIS" // 29
#define err_MISSPRTH 29
, "OPERATOR GOT NO ARGUMENTS" // 30
#define err_GOTNOARGS 30
, "CONSTANT VALUE MUST BE INITIALIZED" // 31
#define err_CONSTINIT 31
, "BAD TYPE" // 32
#define err_BADTYPE 32
};
const string BOOT[] = { "org 7c00h",
"start:",
"cli" ,
"xor ax, ax",
"mov ds, ax",
"mov es, ax",
"mov ss, ax",
"mov sp, 07c00h",
"sti",

"mov al, 3",
"int 10h",
"mov dh, 0",
"mov dl, 0",
"xor bh, bh",
"int 10h",


"push 0x50",
"pop es",

"mov dx, 0; disk A, head 0 (dl - disk, dh - head)",
"mov ch, 0; 10bit - cyl, 6bit - sector",
"mov cl, 2; sector",
"mov al, 7",
"xor bx, bx",

"mov ah, 02h",
"int 13h",

"mov dl, 0",
"mov dh, 1",
"mov bx, 0x0E00",
"mov ch, 0",
"mov cl, 1",
"mov al, 8",

"mov ah, 02h",
"int 13h",


"jmp 0080:0000",
"jmp $",

"fd db 'error!'",

"times 510 - ($ - 7c00h) db 0",
"db 055h, 0AAh; ƒ¿À‹ÿ≈ «¿√–”«◊»  Õ≈ »ƒ®“",
"ORG 500h",
"jmp HOME",
"program_size = HEND - $"};

void THROW_ERROR(bool Condition, int error_cause, int err = error);


inline string TO_STRHEX(unsigned int _Int);

// if CONDIITION is MET then we throw an error
void THROW_ERROR(bool Condition, int error_cause, int err) {
	if (Condition) {
		error = err;
		std::cerr << code << " : LINE " << line << " : COLUMN " << error_cause << " : " << ERRORS[error];
		exit(error);
	}
}

unsigned char type_xtrg = 0;


struct dothing {
	string first, second;
	char sign = '\0';
	unsigned char ptrtype = 0;
};

bool is_func_returning = true;