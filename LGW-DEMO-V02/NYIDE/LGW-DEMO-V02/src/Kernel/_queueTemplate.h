#ifndef	NX1_QUEUE_TEMPLATE_H
#define	NX1_QUEUE_TEMPLATE_H
#include "nx1_lib.h"
#include <string.h>

#define _queueMask(a) ((((a)&((a)-1))==0)?((a)-1): \
(((a)>>31)|((a)>>30)|((a)>>29)|((a)>>28)|((a)>>27)|((a)>>26)|((a)>>25)|((a)>>24)|\
 ((a)>>23)|((a)>>22)|((a)>>21)|((a)>>20)|((a)>>19)|((a)>>18)|((a)>>17)|((a)>>16)|\
 ((a)>>15)|((a)>>14)|((a)>>13)|((a)>>12)|((a)>>11)|((a)>>10)|((a)>> 9)|((a)>> 8)|\
 ((a)>> 7)|((a)>> 6)|((a)>> 5)|((a)>> 4)|((a)>> 3)|((a)>> 2)|((a)>> 1)|((a)>> 0)))
//length use roundup power of 2(equal to _queueMask(_LENGTH)+1)
#define _queueDefine(_TYPE,_LENGTH,_NAME)                                       \
    typedef struct {                                                            \
        U16     pr;                                                             \
        U16     pw;                                                             \
        _TYPE   buf[_queueMask(_LENGTH)+1];                                     \
    }S_queue_##_NAME;                                                           \
    S_queue_##_NAME s_queue_##_NAME;                                            \
    void _queue_##_NAME##_reset(void) {                                         \
        S_queue_##_NAME* p_queue=&s_queue_##_NAME;                              \
        p_queue->pr = p_queue->pw = 0;                                          \
    }                                                                           \
    U16  _queue_##_NAME##_putAvailable(void) {                                  \
        S_queue_##_NAME* p_queue=&s_queue_##_NAME;                              \
        typeof(p_queue->pr) _pr = p_queue->pr;                                  \
        typeof(p_queue->pw) _pw = p_queue->pw;                                  \
        return (_pw==_pr)?                                                      \
            (_queueMask(_LENGTH)+1):((_pr-_pw)&_queueMask(_LENGTH));            \
    }                                                                           \
    U16  _queue_##_NAME##_getAvailable(void) {                                  \
        S_queue_##_NAME* p_queue=&s_queue_##_NAME;                              \
        typeof(p_queue->pr) _pr = p_queue->pr;                                  \
        typeof(p_queue->pw) _pw = p_queue->pw;                                  \
        return (_pw==(_pr^(_queueMask(_LENGTH)+1)))?                            \
            (_queueMask(_LENGTH)+1):((_pw-_pr)&_queueMask(_LENGTH));            \
    }                                                                           \
    bool _queue_##_NAME##_isFull(void) {                                        \
        S_queue_##_NAME* p_queue=&s_queue_##_NAME;                              \
        typeof(p_queue->pr) _pr = p_queue->pr;                                  \
        typeof(p_queue->pw) _pw = p_queue->pw;                                  \
        return _pw==(_pr^(_queueMask(_LENGTH)+1));                              \
    }                                                                           \
    bool _queue_##_NAME##_isEmpty(void) {                                       \
        S_queue_##_NAME* p_queue=&s_queue_##_NAME;                              \
        typeof(p_queue->pr) _pr = p_queue->pr;                                  \
        typeof(p_queue->pw) _pw = p_queue->pw;                                  \
        return _pw==_pr;                                                        \
    }                                                                           \
    S32  _queue_##_NAME##_putData(_TYPE *data) {                                \
        if(!data || _queue_##_NAME##_isFull()) return EXIT_FAILURE;             \
        S_queue_##_NAME* p_queue=&s_queue_##_NAME;                              \
        typeof(p_queue->pw) _pw = p_queue->pw;                                  \
        p_queue->buf[_pw&_queueMask(_LENGTH)]=*data;                            \
        p_queue->pw=(_pw+1)&((2*(_queueMask(_LENGTH)+1))-1);                    \
        return EXIT_SUCCESS;                                                    \
    }                                                                           \
    S32  _queue_##_NAME##_peekData(_TYPE *data) {                               \
        if(!data || _queue_##_NAME##_isEmpty()) return EXIT_FAILURE;            \
        S_queue_##_NAME* p_queue=&s_queue_##_NAME;                              \
        *data=p_queue->buf[(p_queue->pr)&_queueMask(_LENGTH)];                  \
        return EXIT_SUCCESS;                                                    \
    }                                                                           \
    S32  _queue_##_NAME##_getData(_TYPE *data) {                                \
        if(!data || _queue_##_NAME##_isEmpty()) return EXIT_FAILURE;            \
        S_queue_##_NAME* p_queue=&s_queue_##_NAME;                              \
        typeof(p_queue->pr) _pr = p_queue->pr;                                  \
        *data=p_queue->buf[(p_queue->pr)&_queueMask(_LENGTH)];                  \
        p_queue->pr=(_pr+1)&((2*(_queueMask(_LENGTH)+1))-1);                    \
        return EXIT_SUCCESS;                                                    \
    }
    
#define _queueReset(_NAME)            _queue_##_NAME##_reset()
#define _queuePutAvailable(_NAME)     _queue_##_NAME##_putAvailable()
#define _queueGetAvailable(_NAME)     _queue_##_NAME##_getAvailable()
#define _queueIsFull(_NAME)           _queue_##_NAME##_isFull()
#define _queueIsEmpty(_NAME)          _queue_##_NAME##_isEmpty()
#define _queuePutData(_NAME,pData)    _queue_##_NAME##_putData(pData)
#define _queuePeekData(_NAME,pData)   _queue_##_NAME##_peekData(pData)
#define _queueGetData(_NAME,pData)    _queue_##_NAME##_getData(pData)
#endif