#pragma once
#include <optional>
using namespace std;

struct FunctionArgument {
	string Name;
	char Type;
};

struct Function {
	string Name;
	vector<FunctionArgument> Arguments;
};

vector <string> IN;
vector <string> OUT;
vector <string> FUNC;

#define BC \
X(null) \
X(home) \
X(end) \
X(do_b) \
X(notype) \
X(Int) \
X(word) \
X(byte) \
X(adr) \
X(dwordadr) \
X(wordadr) \
X(byteadr) \
X(signed_int) \
X(signed_word) \
X(signed_byte) \
X(qword) \
X(qwordadr) \
X(signed_qword) \
X(allocate) \
X(initialize_val) \
X(initialize_reg) \
X(initialize_obj) \
X(const_val) \
X(const_reg) \
X(If) \
X(declfnc) \
X(declproc) \
X(call) \
X(callret) \
X(ret) \
X(retval) \
X(retobj) \
X(retreg) \
X(argconst) \
X(argobj) \
X(argreg) \
X(obj_address) \
X(dwordadr_val) \
X(wordadr_val) \
X(byteadr_val) \
X(dwordadr_obj) \
X(wordadr_obj) \
X(byteadr_obj) \
X(dwordadr_reg) \
X(wordadr_reg) \
X(byteadr_reg) \
X(const_string) \
X(_2Fh) \
X(ZERO) \
X(ONE) \
X(TWO) \
X(THREE) \
X(FOUR) \
X(FIVE) \
X(SIX) \
X(SEVEN) \
X(EIGHT) \
X(NINE) \
X(write) \
X(to_adr) \
X(to_obj) \
X(_val) \
X(_reg) \
X(til) \
X(whl) \
X(A) \
X(B) \
X(C) \
X(D) \
X(E) \
X(F) \
X(G) \
X(H) \
X(I) \
X(J) \
X(K) \
X(L) \
X(M) \
X(N) \
X(O) \
X(P) \
X(Q) \
X(R) \
X(S) \
X(T) \
X(U) \
X(V) \
X(W) \
X(X) \
X(Y) \
X(Z) \
X(orl) \
X(orb) \
X(andl) \
X(andb) \
X(UNDERSCORE) \
X(xorb) \
X(a) \
X(b) \
X(c) \
X(d) \
X(e) \
X(f) \
X(g) \
X(h) \
X(i) \
X(j) \
X(k) \
X(l) \
X(m) \
X(n) \
X(o) \
X(p) \
X(q) \
X(r) \
X(s) \
X(t) \
X(u) \
X(v) \
X(w) \
X(x) \
X(y) \
X(z) \
X(notl) \
X(notb) \
X(eql) \
X(neq) \
X(lss) \
X(lsseq) \
X(nlss) \
X(lgr) \
X(lgreq) \
X(nlgr) \
X(add) \
X(increment) \
X(sub) \
X(decrement) \
X(cyc) \
X(mul) \
X(Div) \
X(mod) \
X(shl) \
X(shr) \
X(_2obj) \
X(_byobj) \
X(_obj) \
X(_rgobj) \
X(_objbyreg) \
X(_rgconst) \
X(_constbyreg) \
X(assembler) \
X(argument_start)
// do not disassemble:
// - call, callret
// - types (int, word, byte...)
// - addressing functions (wordadr...)
#define X(bytec) bytec,
enum ByteCode : unsigned char {
	BC
	
};

#undef X
// use # to create a string type macro
#define X(bytec) #bytec,

const char* bctable[256] = {
	BC
};

constexpr int size_of_bctable = sizeof(bctable) / sizeof(bctable[0]);


string code;
int line = 0;

inline unsigned int_length(unsigned value) {
	return value != 0 ? ceil(log10(value)) : 1;
}


/*null = 0x00,
	home = 0x01,*/
	/*hend, // 0x02
	do_b, // 0x03
	end, // 0x04
	Int, // 0x05
	word, // 0x06
	byte, // 0x07
	adr, // 0x08
	dwordadr, //0x09
	wordadr, //0x0A
	byteadr, // 0x0B
	signed_int, // 0x0C
	signed_word, // 0x0D
	signed_byte, // 0x0E
	qword, // 0x0F
	qwordadr, // 0x10
	signed_qword, // 0x11
	allocate, // 0x12 - allocate int
	initialize_val, //0x13 - initialize with one certain value given
	initialize_reg, // 0x14 - allocate int = expr (preceding operations have prepared a value in either eax or rax
	initialize_obj, //0x15 - initialize with an existing object value
	const_val, // 0x16, const type arbit = val
	const_reg, // 0x17, const type arbit = reg
	If, // if_bytecode
	ifnt, //
	declfnc, // function declaration
	declproc,        // procedure declaration
	call, // function (procedure) call
	retval, // return a value
	retobj, // return an object
	retreg, // retrun an register value
	argconst, // assign with a constant
	argobj, // assign an argument with an object
	argreg, // assign an argument with a current register value
	ptr, // pointer*/
	/*dwordadr_val, //0x09
	wordadr_val, //0x0A
	byteadr_val, // 0x0B
	dwordadr_obj, //0x09
	wordadr_obj, //0x0A
	byteadr_obj, // 0x0B
	dwordadr_reg, //0x09
	wordadr_reg, //0x0A
	byteadr_reg, // 0x0B
	ZERO = 0x30,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	til, // 0x3A - until
	whl, // 0x3B - while
	notl, // 0x3C - logical not for object
	notb, // 0x3D - bit not for object
	orl, // logical or
	orb, // bit or
	andl, // logical and
	andb, // bit and
	xorb, // bit xor
	eql, // equal (logical operation)
	neq, // not equal
	lss, // less
	lsseq, // less or equal
	nlss, // not less
	lgr, // larger
	lgreq, // larger or equal
	nlgr, // not larger
	add,
	increment, // 0xA2
	sub,
	decrement, // 0xBC
	cyc = 0xC8, // cycle
	mul,
	Div,
	mod, // 0xDE
	shl, // shift left (<<)
	shr,// shift right (>>)
	// we automatically push the calculated value to the heap
	// only if it's not followed by "initialize" utility
	// or "reg" operations (objbyreg...)
	_2obj, // obj + obj
	_byobj, // const / obj
	_obj, // obj + const
	_rgobj, // reg + obj
	_objbyreg, // obj / reg
	_rgconst, // reg + const
	_constbyreg, // const / reg
	allocreg, // mov the register value to RAM*/
