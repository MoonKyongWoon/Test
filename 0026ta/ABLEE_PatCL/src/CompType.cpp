#include "CompType.h"

SET_REG::SET_REG() : reg(0), a(0), b(0), c(0), d(0) { }

unsigned int SET_REG::groupReg(int iGrp)
{
	return ( reg>>(iGrp*4) ) & 0xf;
}

void SET_REG::mergeGroup(int iGrp, SET_REG& _set_reg)
{
	unsigned int mask[4]= { 0xfff0, 0xff0f, 0xf0ff, 0x0fff };
	reg  = reg&mask[iGrp];
	reg |=  _set_reg.groupReg(iGrp)<<(iGrp*4);
		 if ( iGrp==0 ) a = _set_reg.a;
	else if ( iGrp==1 ) b = _set_reg.b;
	else if ( iGrp==2 ) c = _set_reg.c;
	else if ( iGrp==3 ) d = _set_reg.d;
}

void SET_REG::clear()
{
	reg = a = b = c = d = 0;
}

REGI::REGI() : i2(0), addr(0) {}

ALPG::ALPG()
	: seq_mode(0), alpg_mode(0), sta(0), pc(0)
{
	memset((void *)set_reg, 0, sizeof(SET_REG)*16);
//	nStatic = sizeof(ALPG) - sizeof(reg_list);
}

ALPG::~ALPG()
{
	for( RegList::iterator it=reg_list.begin(); it!=reg_list.end(); ++it)
		delete *it;
	reg_list.clear();
}

WAYS::WAYS()
{
	memset((void *)this, 0, sizeof(WAYS) );
	ts = 0x01;									//default TS1
}

PCS::PCS()
	:opcode(0), operand(0), op_use(0)
{
	nStatic = sizeof(PCS);
}

PGDATA::PGDATA()
{
	strncpy( rev, VER_COMPILER , sizeof(rev) );
	pcs   = NULL;
	Init();
	pcs = new PcList;
}

PGDATA::~PGDATA()
{
	for( AlpgList::iterator mdlIt=module.begin(); mdlIt!=module.end(); ++mdlIt)
		delete *mdlIt;
	module.clear();

	if ( pcs )
	{
		for( PcList::iterator it=pcs->begin(); it!=pcs->end(); ++it)
			delete *it; //~PCS()
		pcs->clear();

		delete pcs; pcs = NULL;
	}
}

void PGDATA::Init()
{
	memset( name, 0, sizeof(name) );

	for( AlpgList::iterator mdlIt=module.begin(); mdlIt!=module.end(); ++mdlIt)
		delete *mdlIt;
	module.clear();

	if ( pcs )
	{
		for( PcList::iterator it=pcs->begin(); it!=pcs->end(); ++it)
			delete *it; //~PCS()
		pcs->clear();
	}

}

PCS *PGDATA::GetPCS(unsigned int iPc)
{
	if ( ! pcs              ) return NULL;
	if ( iPc >= pcs->size() ) return NULL;

	unsigned i = 0;
	for( PcList::iterator it=pcs->begin(); it!=pcs->end(); ++it, ++i)
	{
		if ( i==iPc)
		{
			return *it;
		}
	}

	return NULL;
}

NOLABEL::NOLABEL()
{
	memset( label, 0, sizeof(label) );
	pc  = 0;
	reg = NULL;
}

IFE_EMT::IFE_EMT(                ) :opr(   0), r(-1) {}
IFE_EMT::IFE_EMT(int _opr, int _r) :opr(_opr), r(_r) {}

IFE_GRADE::IFE_GRADE(int _grade) :grade(_grade), opr(0) {}
IFE_GRADE::~IFE_GRADE()
{
	for( IFE_LIST::iterator it=ife_list.begin(); it!=ife_list.end(); ++it )
	{
		delete *it;
	}
	ife_list.clear();
}
