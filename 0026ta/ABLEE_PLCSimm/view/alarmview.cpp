#include "alarmview.hpp"
#include "ui_alarmview.h"

CAlarmView::CAlarmView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CAlarmView)
{
	ui->setupUi(this);
	Init();
}

CAlarmView::~CAlarmView()
{
	delete ui;
}

void CAlarmView::Init()
{
	InitBtnPtr();
	InitButtons();
}

void CAlarmView::InitBtnPtr()
{
	m_pAlrmBtn[0][0 ] = ui->D150_1 ;
	m_pAlrmBtn[0][1 ] = ui->D150_2 ;
	m_pAlrmBtn[0][2 ] = ui->D150_3 ;
	m_pAlrmBtn[0][3 ] = ui->D150_4 ;
	m_pAlrmBtn[0][4 ] = ui->D150_5 ;
	m_pAlrmBtn[0][5 ] = ui->D150_6 ;
	m_pAlrmBtn[0][6 ] = ui->D150_7 ;
	m_pAlrmBtn[0][7 ] = ui->D150_8 ;
	m_pAlrmBtn[0][8 ] = ui->D150_9 ;
	m_pAlrmBtn[0][9 ] = ui->D150_10;
	m_pAlrmBtn[0][10] = ui->D150_11;
	m_pAlrmBtn[0][11] = ui->D150_12;
	m_pAlrmBtn[0][12] = ui->D150_13;
	m_pAlrmBtn[0][13] = ui->D150_14;
	m_pAlrmBtn[0][14] = ui->D150_15;
	m_pAlrmBtn[0][15] = ui->D150_16;

	m_pAlrmBtn[1][0 ] = ui->D151_1 ;
	m_pAlrmBtn[1][1 ] = ui->D151_2 ;
	m_pAlrmBtn[1][2 ] = ui->D151_3 ;
	m_pAlrmBtn[1][3 ] = ui->D151_4 ;
	m_pAlrmBtn[1][4 ] = ui->D151_5 ;
	m_pAlrmBtn[1][5 ] = ui->D151_6 ;
	m_pAlrmBtn[1][6 ] = ui->D151_7 ;
	m_pAlrmBtn[1][7 ] = ui->D151_8 ;
	m_pAlrmBtn[1][8 ] = ui->D151_9 ;
	m_pAlrmBtn[1][9 ] = ui->D151_10;
	m_pAlrmBtn[1][10] = ui->D151_11;
	m_pAlrmBtn[1][11] = ui->D151_12;
	m_pAlrmBtn[1][12] = ui->D151_13;
	m_pAlrmBtn[1][13] = ui->D151_14;
	m_pAlrmBtn[1][14] = ui->D151_15;
	m_pAlrmBtn[1][15] = ui->D151_16;

	m_pAlrmBtn[2][0 ] = ui->D152_1 ;
	m_pAlrmBtn[2][1 ] = ui->D152_2 ;
	m_pAlrmBtn[2][2 ] = ui->D152_3 ;
	m_pAlrmBtn[2][3 ] = ui->D152_4 ;
	m_pAlrmBtn[2][4 ] = ui->D152_5 ;
	m_pAlrmBtn[2][5 ] = ui->D152_6 ;
	m_pAlrmBtn[2][6 ] = ui->D152_7 ;
	m_pAlrmBtn[2][7 ] = ui->D152_8 ;
	m_pAlrmBtn[2][8 ] = ui->D152_9 ;
	m_pAlrmBtn[2][9 ] = ui->D152_10;
	m_pAlrmBtn[2][10] = ui->D152_11;
	m_pAlrmBtn[2][11] = ui->D152_12;
	m_pAlrmBtn[2][12] = ui->D152_13;
	m_pAlrmBtn[2][13] = ui->D152_14;
	m_pAlrmBtn[2][14] = ui->D152_15;
	m_pAlrmBtn[2][15] = ui->D152_16;

	m_pAlrmBtn[3][0 ] = ui->D153_1 ;
	m_pAlrmBtn[3][1 ] = ui->D153_2 ;
	m_pAlrmBtn[3][2 ] = ui->D153_3 ;
	m_pAlrmBtn[3][3 ] = ui->D153_4 ;
	m_pAlrmBtn[3][4 ] = ui->D153_5 ;
	m_pAlrmBtn[3][5 ] = ui->D153_6 ;
	m_pAlrmBtn[3][6 ] = ui->D153_7 ;
	m_pAlrmBtn[3][7 ] = ui->D153_8 ;
	m_pAlrmBtn[3][8 ] = ui->D153_9 ;
	m_pAlrmBtn[3][9 ] = ui->D153_10;
	m_pAlrmBtn[3][10] = ui->D153_11;
	m_pAlrmBtn[3][11] = ui->D153_12;
	m_pAlrmBtn[3][12] = ui->D153_13;
	m_pAlrmBtn[3][13] = ui->D153_14;
	m_pAlrmBtn[3][14] = ui->D153_15;
	m_pAlrmBtn[3][15] = ui->D153_16;

	m_pAlrmBtn[4][0 ] = ui->D154_1 ;
	m_pAlrmBtn[4][1 ] = ui->D154_2 ;
	m_pAlrmBtn[4][2 ] = ui->D154_3 ;
	m_pAlrmBtn[4][3 ] = ui->D154_4 ;
	m_pAlrmBtn[4][4 ] = ui->D154_5 ;
	m_pAlrmBtn[4][5 ] = ui->D154_6 ;
	m_pAlrmBtn[4][6 ] = ui->D154_7 ;
	m_pAlrmBtn[4][7 ] = ui->D154_8 ;
	m_pAlrmBtn[4][8 ] = ui->D154_9 ;
	m_pAlrmBtn[4][9 ] = ui->D154_10;
	m_pAlrmBtn[4][10] = ui->D154_11;
	m_pAlrmBtn[4][11] = ui->D154_12;
	m_pAlrmBtn[4][12] = ui->D154_13;
	m_pAlrmBtn[4][13] = ui->D154_14;
	m_pAlrmBtn[4][14] = ui->D154_15;
	m_pAlrmBtn[4][15] = ui->D154_16;

	m_pAlrmBtn[5][0 ] = ui->D155_1 ;
	m_pAlrmBtn[5][1 ] = ui->D155_2 ;
	m_pAlrmBtn[5][2 ] = ui->D155_3 ;
	m_pAlrmBtn[5][3 ] = ui->D155_4 ;
	m_pAlrmBtn[5][4 ] = ui->D155_5 ;
	m_pAlrmBtn[5][5 ] = ui->D155_6 ;
	m_pAlrmBtn[5][6 ] = ui->D155_7 ;
	m_pAlrmBtn[5][7 ] = ui->D155_8 ;
	m_pAlrmBtn[5][8 ] = ui->D155_9 ;
	m_pAlrmBtn[5][9 ] = ui->D155_10;
	m_pAlrmBtn[5][10] = ui->D155_11;
	m_pAlrmBtn[5][11] = ui->D155_12;
	m_pAlrmBtn[5][12] = ui->D155_13;
	m_pAlrmBtn[5][13] = ui->D155_14;
	m_pAlrmBtn[5][14] = ui->D155_15;
	m_pAlrmBtn[5][15] = ui->D155_16;

	m_pAlrmBtn[6][0 ] = ui->D156_1 ;
	m_pAlrmBtn[6][1 ] = ui->D156_2 ;
	m_pAlrmBtn[6][2 ] = ui->D156_3 ;
	m_pAlrmBtn[6][3 ] = ui->D156_4 ;
	m_pAlrmBtn[6][4 ] = ui->D156_5 ;
	m_pAlrmBtn[6][5 ] = ui->D156_6 ;
	m_pAlrmBtn[6][6 ] = ui->D156_7 ;
	m_pAlrmBtn[6][7 ] = ui->D156_8 ;
	m_pAlrmBtn[6][8 ] = ui->D156_9 ;
	m_pAlrmBtn[6][9 ] = ui->D156_10;
	m_pAlrmBtn[6][10] = ui->D156_11;
	m_pAlrmBtn[6][11] = ui->D156_12;
	m_pAlrmBtn[6][12] = ui->D156_13;
	m_pAlrmBtn[6][13] = ui->D156_14;
	m_pAlrmBtn[6][14] = ui->D156_15;
	m_pAlrmBtn[6][15] = ui->D156_16;

	m_pAlrmBtn[7][0 ] = ui->D157_1 ;
	m_pAlrmBtn[7][1 ] = ui->D157_2 ;
	m_pAlrmBtn[7][2 ] = ui->D157_3 ;
	m_pAlrmBtn[7][3 ] = ui->D157_4 ;
	m_pAlrmBtn[7][4 ] = ui->D157_5 ;
	m_pAlrmBtn[7][5 ] = ui->D157_6 ;
	m_pAlrmBtn[7][6 ] = ui->D157_7 ;
	m_pAlrmBtn[7][7 ] = ui->D157_8 ;
	m_pAlrmBtn[7][8 ] = ui->D157_9 ;
	m_pAlrmBtn[7][9 ] = ui->D157_10;
	m_pAlrmBtn[7][10] = ui->D157_11;
	m_pAlrmBtn[7][11] = ui->D157_12;
	m_pAlrmBtn[7][12] = ui->D157_13;
	m_pAlrmBtn[7][13] = ui->D157_14;
	m_pAlrmBtn[7][14] = ui->D157_15;
	m_pAlrmBtn[7][15] = ui->D157_16;

	m_pAlrmBtn[8][0 ] = ui->D158_1 ;
	m_pAlrmBtn[8][1 ] = ui->D158_2 ;
	m_pAlrmBtn[8][2 ] = ui->D158_3 ;
	m_pAlrmBtn[8][3 ] = ui->D158_4 ;
	m_pAlrmBtn[8][4 ] = ui->D158_5 ;
	m_pAlrmBtn[8][5 ] = ui->D158_6 ;
	m_pAlrmBtn[8][6 ] = ui->D158_7 ;
	m_pAlrmBtn[8][7 ] = ui->D158_8 ;
	m_pAlrmBtn[8][8 ] = ui->D158_9 ;
	m_pAlrmBtn[8][9 ] = ui->D158_10;
	m_pAlrmBtn[8][10] = ui->D158_11;
	m_pAlrmBtn[8][11] = ui->D158_12;
	m_pAlrmBtn[8][12] = ui->D158_13;
	m_pAlrmBtn[8][13] = ui->D158_14;
	m_pAlrmBtn[8][14] = ui->D158_15;
	m_pAlrmBtn[8][15] = ui->D158_16;

	m_pAlrmBtn[9][0 ] = ui->D159_1 ;
	m_pAlrmBtn[9][1 ] = ui->D159_2 ;
	m_pAlrmBtn[9][2 ] = ui->D159_3 ;
	m_pAlrmBtn[9][3 ] = ui->D159_4 ;
	m_pAlrmBtn[9][4 ] = ui->D159_5 ;
	m_pAlrmBtn[9][5 ] = ui->D159_6 ;
	m_pAlrmBtn[9][6 ] = ui->D159_7 ;
	m_pAlrmBtn[9][7 ] = ui->D159_8 ;
	m_pAlrmBtn[9][8 ] = ui->D159_9 ;
	m_pAlrmBtn[9][9 ] = ui->D159_10;
	m_pAlrmBtn[9][10] = ui->D159_11;
	m_pAlrmBtn[9][11] = ui->D159_12;
	m_pAlrmBtn[9][12] = ui->D159_13;
	m_pAlrmBtn[9][13] = ui->D159_14;
	m_pAlrmBtn[9][14] = ui->D159_15;
	m_pAlrmBtn[9][15] = ui->D159_16;

}
void CAlarmView::InitButtons()
{
	for( int d=0;d<10;d++ )
	{
		for( int e=0;e<16;e++ )
		{
			m_pAlrmBtn[d][e]->setStyleSheet( COMM_NORMAL_STYLE );
			connect( m_pAlrmBtn[d][e], &QPushButton::toggled, this, &CAlarmView::SlotBtnToggled );
		}
	}
}

void CAlarmView::SlotBtnToggled( bool bChecked )
{
	QPushButton* sender = qobject_cast<QPushButton*>(QObject::sender());
	if( sender != nullptr )
	{
		if( bChecked )
			sender->setStyleSheet( COMM_ALARM_STYLE );
		else
			sender->setStyleSheet( COMM_NORMAL_STYLE );
	}
}

void CAlarmView::SlotAlarmReset()
{
	for( int d=0;d<10;d++ )
	{
		for( int e=0;e<16;e++ )
		{
			m_pAlrmBtn[d][e]->setChecked( false );
		}
	}
}

bool CAlarmView::DoReadAlarm( EN_PLC_ADDRESS_MATCH_CMD cmd, int& rValue )
{
	int offset = static_cast<int>( cmd - EN_GET_COMMON_ALARM_D150 );
	switch( cmd )
	{
		case EN_PLC_ADDRESS_MATCH_CMD::EN_GET_COMMON_ALARM_D150:
		case EN_PLC_ADDRESS_MATCH_CMD::EN_GET_COMMON_ALARM_D151:
		case EN_PLC_ADDRESS_MATCH_CMD::EN_GET_COMMON_ALARM_D152:
		case EN_PLC_ADDRESS_MATCH_CMD::EN_GET_COMMON_ALARM_D153:
		case EN_PLC_ADDRESS_MATCH_CMD::EN_GET_COMMON_ALARM_D154:
		case EN_PLC_ADDRESS_MATCH_CMD::EN_GET_COMMON_ALARM_D155:
		case EN_PLC_ADDRESS_MATCH_CMD::EN_GET_COMMON_ALARM_D156:
		case EN_PLC_ADDRESS_MATCH_CMD::EN_GET_COMMON_ALARM_D157:
		case EN_PLC_ADDRESS_MATCH_CMD::EN_GET_COMMON_ALARM_D158:
		case EN_PLC_ADDRESS_MATCH_CMD::EN_GET_COMMON_ALARM_D159:
		{
			rValue = 0;
			for( int e=0;e<16;e++ )
			{
				if( m_pAlrmBtn[offset][e]->isChecked() )
				{
					rValue |= (1<<e);
				}
			}
		}
		break;

		default:
		{
			rValue = 0;
			return false;
		}
		break;
	}
	return true;
}
