#ifndef _SW_CONTROL
#define _SW_CONTROL

const unsigned int ODD                     = 1;
const unsigned int EVEN                    = 2;
const unsigned int _OR                     = 0;
const unsigned int _AND                    = 1;
const unsigned int _NOT                    = 2;
const unsigned int FAIL                    = 1;
const unsigned int PASS                    = 2;
const unsigned int F_MASK                  = 4;
const unsigned int F_FAIL                  = 5;
const unsigned int F_PASS                  = 6;
const unsigned int L_MASK                  = 8;
const unsigned int L_FAIL                  = 9;
const unsigned int L_PASS                  = 10;
const unsigned int D_MASK                  = 16;
const unsigned int G_MASK                  = 32;

const unsigned int MAX_NSLOT               = 1;
const unsigned int MAX_STEP                = 1000;
const unsigned int MAX_STEP_BIT_L          = (((MAX_STEP) + 2) / 8)+1;
const unsigned int MAX_CATEGORY            = 1000;
const unsigned int MAX_CATEGORY_BIT_L      = (MAX_CATEGORY / 8)+1;
const unsigned int MAX_NBYTE               = 32;
const unsigned int MAX_DQ_VALUE            = 256;
const unsigned int MAX_SCAN                = 32;
const unsigned int MAX_DUT_PER_FC          = 24;
const unsigned int MAX_CE                  = 8;
const unsigned int MAX_IO                  = 96;

const unsigned int CUM_FAIL                = 0x00000;
const unsigned int ITEM_FAIL               = 0x10000;
const unsigned int CD_WRITE                = 0;
const unsigned int IMPRINT                 = 1;
const unsigned int MEMORY                  = 2;
const unsigned int ALL_FAIL                = 1;
const unsigned int ALL_BLOCK_FAIL          = 2;

const unsigned int NORMAL                  = 0x0;
const unsigned int INIT_FAIL               = 0x1;
const unsigned int ADD_FAIL                = 0x2;
const unsigned int ECC_FAIL                = 0x3;
const unsigned int MUST_FAIL               = 0x4;
const unsigned int AGING                   = 0x5;
const unsigned int GRADE_TEST              = 0x6;
const unsigned int BLOCK                   = 0x7;

const unsigned int JUD_NORMAL              = 0;
const unsigned int JUD_CLEAN               = 1;
const unsigned int JUD_64M1                = 2;
const unsigned int JUD_64M2                = 3;
const unsigned int JUD_BCAU                = 4;
const unsigned int FAIL_MASK               = 1;
const unsigned int PASS_MASK               = 2;
const unsigned int BLOCK_DATA              = 4;

const unsigned int MULTI_AND               = 0;
const unsigned int MULTI_OR                = 1;

const unsigned int CONSTANT                = 0;
const unsigned int AMBIENT                 = 1;
const unsigned int CMBSTOP                 = 3;
const unsigned int DAMPER_OPEN             = 2;
const unsigned int DAMPER_CLOSE            = 0;
const unsigned int IO_DEFINE_BUFFER_LENGTH = 8;

#endif
