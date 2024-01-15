#include "nx1_lib.h"
#include "include.h"

PARA_TYPE U8 u8_DeBruijn_Bit_PosTab32[32] = 
{
	0,	1,	28,	2,	29,	14,	24,	3,	30,	22,	20,	15,	25,	17,	4,	8, 
	31,	27,	13,	23,	21,	19,	16,	7,	26,	12,	18,	6,	11,	5,	10,	9,
};//map for 0x077CB531
//------------------------------------------------------------------//
// Get lowest set bit of variable
// Parameter: 
//          _value:1~U32_Max(Warning, 0 is illegal input)
// return value:
//          0~31(bit0~bit31)
//------------------------------------------------------------------//
U8	Get_LS1B_Position(U32 _value)
{
	_value=_value&(-_value);//keepLowestBit, equal _value&((~_value)+1)
	return u8_DeBruijn_Bit_PosTab32[((_value*0x077CB531)>>27)];
}
//------------------------------------------------------------------//
// Get highest set bit of variable
// Parameter: 
//          _value:1~U32_Max(Warning, 0 is illegal input)
// return value:
//          0~31(bit0~bit31)
//------------------------------------------------------------------//
U8 Get_HS1B_Position(U32 _value)
{
	_value |= (_value >>  1);
	_value |= (_value >>  2);
	_value |= (_value >>  4);
	_value |= (_value >>  8);
	_value |= (_value >> 16);
	_value -= (_value >> 1);//keepHighestBit
	return u8_DeBruijn_Bit_PosTab32[((_value*0x077CB531)>>27)];
}