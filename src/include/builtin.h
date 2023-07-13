#pragma once

#ifndef BUILTIN_H
#define BUILTIN_H


enum {
	Ttrue,		// Bool true
	Tfalse,		// Bool false
	Tnull,		// String null
	Tcond,		// if-condition
	Telif,		// elif-condition
	Telse,		// else-condition
	Tdowh,		// do-while-loop
	Twhil,		// while-loop
	Tfunc,		// function
	Tretn,		// return
	Tassn,		// =
	Tnega,		// !
	Tequl,		// ==
	Tneql,		// !=
	Tlthn,		// <
	Tgthn,		// >
	Tlteq,		// <=
	Tgteq,		// >=
	Tconj,		// &&
	Tdisj,		// ||
	Tplus,		// +
	Tmins,		// -
	Tmult,		// *
	Tdivi,		// /
	Tmodu,		// %
	Tpequ,		// +=
	Tmequ,		// -=
	Tplpl,			// ++
	Tmimi,		// --
	Tques,		// ?	
	Tscol,		// ;
	Tlpar,		// (
	Trpar,		// )
	Tlbrc,		// {
	Trbrc,		// }
	Tlbra,		// [
	Trbra,		// ]
	Tcoln,		// :
	Tdott,		// .
	Tcoma		// ,
};



static const char *const tokens [] = {
    "true", "false", "null", "if", "elif",
    "else", "do", "while", "function",
    "return", "=", "!", "==", "!=", "<",
    ">", "<=", ">=", "&&", "||", "+", "-",
    "*", "/", "%", "+=", "-=", "++", "--",
    "?", ";", "(", ")", "{", "}", "[", "]", ":",
    ".", ","
};

#endif