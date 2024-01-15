/* Includes ------------------------------------------------------------------*/
#include "include.h"


#if _QFID_MODULE
#if _QCODE_BUILD
#else
/*============================================================================
QFID detect/leave behavior control

Example. Single Group(Group0), and use 4 Tags (Tag0~3).

 //Presetting-----------------------------------------------------------------
 // - Group0 Detect (fill 0 : No function)
        void g0_dt_tag0(void);
        void g0_dt_tag1(void);
        void g0_dt_tag3(void);

        void (*QfidFunc_Group0_detect[_QFID_GRP0_TAG_NUM])(void)	= { g0_dt_tag0, g0_dt_tag1, 0, g0_dt_tag3 };

 // - Group0 Leave (fill 0 : No function)
        void g0_lv_tag0(void);
        void g0_lv_tag3(void);

        void (*QfidFunc_Group0_leave[_QFID_GRP0_TAG_NUM])(void)     = { g0_lv_tag0, 0         , 0, g0_lv_tag3 };

 //User defined code-----------------------------------------------------------
        void g0_dt_tag0(void){
            //do soming...
        }
        void g0_dt_tag1(void){
            //do soming...
        }
        void g0_dt_tag3(void){
            //do soming...
        }

        void g0_lv_tag0(void){
            //do soming...
        }
        void g0_lv_tag3(void){
            //do soming...
        }
//============================================================================*/

/* Function Sets -------------------------------------------------------------*/

// Group 0_Functions----------------------------------------------------------
#if _QFID_GROUP_NUM>0

void (*QfidFunc_Group0_detect[_QFID_GRP0_TAG_NUM])(void)    = {};
void (*QfidFunc_Group0_leave[_QFID_GRP0_TAG_NUM])(void)     = {};
#endif

// Group 1_Functions----------------------------------------------------------
#if _QFID_GROUP_NUM>1

void (*QfidFunc_Group1_detect[_QFID_GRP1_TAG_NUM])(void) 	= {};
void (*QfidFunc_Group1_leave[_QFID_GRP1_TAG_NUM])(void)     = {};
#endif

// Group 2_Functions----------------------------------------------------------
#if _QFID_GROUP_NUM>2

void (*QfidFunc_Group2_detect[_QFID_GRP2_TAG_NUM])(void) 	= {};
void (*QfidFunc_Group2_leave[_QFID_GRP2_TAG_NUM])(void)     = {};
#endif

// Group 3_Functions----------------------------------------------------------
#if _QFID_GROUP_NUM>3

void (*QfidFunc_Group3_detect[_QFID_GRP3_TAG_NUM])(void)	= {};
void (*QfidFunc_Group3_leave[_QFID_GRP3_TAG_NUM])(void)		= {};
#endif

// Group 4_Functions----------------------------------------------------------
#if _QFID_GROUP_NUM>4

void (*QfidFunc_Group4_detect[_QFID_GRP4_TAG_NUM])(void)	= {};
void (*QfidFunc_Group4_leave[_QFID_GRP4_TAG_NUM])(void)		= {};
#endif

// Group 5_Functions----------------------------------------------------------
#if _QFID_GROUP_NUM>5

void (*QfidFunc_Group5_detect[_QFID_GRP5_TAG_NUM])(void)	= {};
void (*QfidFunc_Group5_leave[_QFID_GRP5_TAG_NUM])(void)		= {};
#endif

// Group 6_Functions----------------------------------------------------------
#if _QFID_GROUP_NUM>6

void (*QfidFunc_Group6_detect[_QFID_GRP6_TAG_NUM])(void)	= {};
void (*QfidFunc_Group6_leave[_QFID_GRP6_TAG_NUM])(void)		= {};
#endif

// Group 7_Functions----------------------------------------------------------
#if _QFID_GROUP_NUM>7

void (*QfidFunc_Group7_detect[_QFID_GRP7_TAG_NUM])(void)	= {};
void (*QfidFunc_Group7_leave[_QFID_GRP7_TAG_NUM])(void)		= {};
#endif

//------------------------------------------------------------------//
// QFID _ User define code
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
/* Group0__Detect */
/*
void g0_dt_tag0(void)
{
	//User defined code
}

*/
//-----
//-----

/* Group0__Leave */
/*
void g0_lv_tag0(void)
{
	//User defined code
}
*/


//------------------------------------------------------------------//
// QFID detect/leave process (called by lib)
// Parameter: 
//          tagid : tag id (function no.)
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _QFID_GROUP_NUM>0
void CB_QfidFunc_Grp0_detect(U8 tagid)
{
	void (*func)(void);
	func=QfidFunc_Group0_detect[tagid];
	if(func==0)
		return;
	func();
}
void CB_QfidFunc_Grp0_leave(U8 tagid)
{
    void (*func)(void);
    func=QfidFunc_Group0_leave[tagid];
    if(func==0)
        return;
    func();
}
#endif

#if _QFID_GROUP_NUM>1
void CB_QfidFunc_Grp1_detect(U8 tagid)
{
	void (*func)(void);
	func=QfidFunc_Group1_detect[tagid];	
	if(func==0)
		return;	
	func();
}
void CB_QfidFunc_Grp1_leave(U8 tagid)
{
    void (*func)(void);
    func=QfidFunc_Group1_leave[tagid];
    if(func==0)
        return;
    func();
}
#endif

#if _QFID_GROUP_NUM>2
void CB_QfidFunc_Grp2_detect(U8 tagid)
{
	void (*func)(void);
	func=QfidFunc_Group2_detect[tagid];	
	if(func==0)
		return;
	func();
}
void CB_QfidFunc_Grp2_leave(U8 tagid)
{
    void (*func)(void);
    func=QfidFunc_Group2_leave[tagid];
    if(func==0)
        return;
    func();
}
#endif

#if _QFID_GROUP_NUM>3
void CB_QfidFunc_Grp3_detect(U8 tagid)
{
	void (*func)(void);
	func=QfidFunc_Group3_detect[tagid];	
	if(func==0)
		return;
	func();
}
void CB_QfidFunc_Grp3_leave(U8 tagid)
{
    void (*func)(void);
    func=QfidFunc_Group3_leave[tagid];
    if(func==0)
        return;
    func();
}
#endif

#if _QFID_GROUP_NUM>4
void CB_QfidFunc_Grp4_detect(U8 tagid)
{
	void (*func)(void);
	func=QfidFunc_Group4_detect[tagid];	
	if(func==0)
		return;
	func();
}
void CB_QfidFunc_Grp4_leave(U8 tagid)
{
    void (*func)(void);
    func=QfidFunc_Group4_leave[tagid];
    if(func==0)
        return;
    func();
}
#endif

#if _QFID_GROUP_NUM>5
void CB_QfidFunc_Grp5_detect(U8 tagid)
{
	void (*func)(void);
	func=QfidFunc_Group5_detect[tagid];	
	if(func==0)
		return;
	func();
}
void CB_QfidFunc_Grp5_leave(U8 tagid)
{
    void (*func)(void);
    func=QfidFunc_Group5_leave[tagid];
    if(func==0)
        return;
    func();
}
#endif

#if _QFID_GROUP_NUM>6
void CB_QfidFunc_Grp6_detect(U8 tagid)
{
	void (*func)(void);
	func=QfidFunc_Group6_detect[tagid];	
	if(func==0)
		return;
	func();
}
void CB_QfidFunc_Grp6_leave(U8 tagid)
{
    void (*func)(void);
    func=QfidFunc_Group6_leave[tagid];
    if(func==0)
        return;
    func();
}
#endif

#if _QFID_GROUP_NUM>7
void CB_QfidFunc_Grp7_detect(U8 tagid)
{
	void (*func)(void);
	func=QfidFunc_Group7_detect[tagid];
	if(func==0)
		return;
	func();
}
void CB_QfidFunc_Grp7_leave(U8 tagid)
{
    void (*func)(void);
    func=QfidFunc_Group7_leave[tagid];
    if(func==0)
        return;
    func();
}
#endif

void (* const CB_QfidFuncTable_detect[])(U8) = {
#if _QFID_GROUP_NUM>0
			CB_QfidFunc_Grp0_detect,
#endif
#if _QFID_GROUP_NUM>1
			CB_QfidFunc_Grp1_detect,
#endif
#if _QFID_GROUP_NUM>2
			CB_QfidFunc_Grp2_detect,
#endif
#if _QFID_GROUP_NUM>3
			CB_QfidFunc_Grp3_detect,
#endif
#if _QFID_GROUP_NUM>4
			CB_QfidFunc_Grp4_detect,
#endif
#if _QFID_GROUP_NUM>5
			CB_QfidFunc_Grp5_detect,
#endif
#if _QFID_GROUP_NUM>6
			CB_QfidFunc_Grp6_detect,
#endif
#if _QFID_GROUP_NUM>7
			CB_QfidFunc_Grp7_detect,
#endif
};
void (* const CB_QfidFuncTable_leave[])(U8) = {
#if _QFID_GROUP_NUM>0
			CB_QfidFunc_Grp0_leave,
#endif
#if _QFID_GROUP_NUM>1
			CB_QfidFunc_Grp1_leave,
#endif
#if _QFID_GROUP_NUM>2
			CB_QfidFunc_Grp2_leave,
#endif
#if _QFID_GROUP_NUM>3
			CB_QfidFunc_Grp3_leave,
#endif
#if _QFID_GROUP_NUM>4
			CB_QfidFunc_Grp4_leave,
#endif
#if _QFID_GROUP_NUM>5
			CB_QfidFunc_Grp5_leave,
#endif
#if _QFID_GROUP_NUM>6
			CB_QfidFunc_Grp6_leave,
#endif
#if _QFID_GROUP_NUM>7
			CB_QfidFunc_Grp7_leave,
#endif
};
#endif//#if _QCODE_BUILD
//------------------------------------------------------------------//
// Tag Detect process (Ref. to qfid_callback.c)
// Parameter:
//     		_group:(0 ~ _QFID_GROUP_NUM-1)
//			_tagid:(0 ~ _QFID_GRPn_TAG_NUM-1)
// return value:
//     		NONE
//------------------------------------------------------------------//
void CB_QfidFunc_Grp_Tag_detect(U8 _group, U8 _tagid)
{
#if _QCODE_BUILD
	QC_QFID_Grp_Tag_detect(_group, _tagid);
#else
	CB_QfidFuncTable_detect[_group](_tagid);
#endif//#if _QCODE_BUILD
}
//------------------------------------------------------------------//
// Tag Leave process (Ref. to qfid_callback.c)
// Parameter:
//     		_group:(0 ~ _QFID_GROUP_NUM-1)
//			_tagid:(0 ~ _QFID_GRPn_TAG_NUM-1)
// return value:
//     		NONE
//------------------------------------------------------------------//
void CB_QfidFunc_Grp_Tag_leave(U8 _group, U8 _tagid)
{
#if _QCODE_BUILD
	QC_QFID_Grp_Tag_leave(_group, _tagid);
#else
	CB_QfidFuncTable_leave[_group](_tagid);
#endif//#if _QCODE_BUILD
}
#endif  // end off _QFID_MODULE
