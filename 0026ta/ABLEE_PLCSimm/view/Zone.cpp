#include "Zone.hpp"
#include "ui_Zone.h"

CZone::CZone( int ch, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CZone)
{
    m_nChannelNo = ch;
    InitVariables();
    InitUi();
}

CZone::~CZone()
{
    delete ui;
}

void CZone::InitVariables()
{
    m_stZoneStatus.nTempRunMode  = EN_STATUS_STOP;
    m_stZoneStatus.nDoorCloseComplete = 1;
    m_stZoneStatus.nSlot1PowerOnMode = 0;
    m_stZoneStatus.nSlot2PowerOnMode = 0;
    m_stZoneStatus.nMaskOnMode = 0;
    m_stZoneStatus.nMaskOffMode = 1;
    m_stZoneStatus.nSystemPowerOnMode = 0;
}
void CZone::InitUi()
{
    ui->setupUi(this);
    InitChannelTitle();
    InitChamberStatus();
    InitSlotPower();
    InitDoorAndMask();
    InitSvPv();
    InitAlarmButtons();
    ClearAlarm();
    InitTesterAlarm(false);
    SetSv( 0.0 );
    SetDv( 0.0 );
}

void CZone::InitChannelTitle()
{
    ui->lblChannelTitle->setText( tr("CHANNEL#%1").arg(m_nChannelNo+1) );
    ui->lblChannelTitle->setStyleSheet( TITLE_CH_STYLE );
}

void CZone::InitChamberStatus()
{
    SetChamberStatus( m_stZoneStatus.nTempRunMode );
}

void CZone::InitDoorAndMask()
{
    if( m_stZoneStatus.nDoorCloseComplete ) ui->lblDoorClose->setStyleSheet(STATUS_RUN_STYLE);
    else                                    ui->lblDoorClose->setStyleSheet(STATUS_NORM_STYLE);

    if( m_stZoneStatus.nMaskOnMode ) ui->lblMask->setStyleSheet(STATUS_RUN_STYLE);
    else                             ui->lblMask->setStyleSheet(STATUS_NORM_STYLE);
}

void CZone::InitSlotPower()
{
    if( m_stZoneStatus.nSlot1PowerOnMode)   ui->lblPowerSlot1->setStyleSheet(STATUS_RUN_STYLE);
    else                                    ui->lblPowerSlot1->setStyleSheet(STATUS_NORM_STYLE);
    if( m_stZoneStatus.nSlot2PowerOnMode)   ui->lblPowerSlot2->setStyleSheet(STATUS_RUN_STYLE);
    else                                    ui->lblPowerSlot2->setStyleSheet(STATUS_NORM_STYLE);
}

void CZone::InitSystemPower()
{
    if( m_stZoneStatus.nSystemPowerOnMode)  { ui->btnSysPwr->setChecked( true);  ui->btnSysPwr->setStyleSheet(STATUS_RUN_STYLE); }
    else                                    { ui->btnSysPwr->setChecked( false); ui->btnSysPwr->setStyleSheet(STATUS_NORM_STYLE); }
}

void CZone::InitSvPv()
{
    ui->spinPV->setValue(25.0);
    ui->lcdSV->display( QString("0.0") );
}

void CZone::InitTesterAlarm(bool bOn)
{
    if( bOn )
    {
        if( m_AlarmLevel[51] == EN_HEAVY_LEVEL ) { m_pAlarmBtns[51]->setStyleSheet(ALARM_ALARM_STYLE  ); m_pAlarmBtns[51]->setChecked( true ); }
        else                                     { m_pAlarmBtns[51]->setStyleSheet(ALARM_WARNING_STYLE); m_pAlarmBtns[51]->setChecked( true ); }
    }
    else
    {
        m_pAlarmBtns[51]->setStyleSheet(ALARM_NORMAL_STYLE);
        m_pAlarmBtns[51]->setChecked( false );
    }
}

void CZone::InitAlarmButtons()
{
    m_pAlarmBtns[0 ] = ui->btnChAlarm0 ;      m_AlarmLevel[0 ] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[1 ] = ui->btnChAlarm1 ;      m_AlarmLevel[1 ] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[2 ] = ui->btnChAlarm2 ;      m_AlarmLevel[2 ] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[3 ] = ui->btnChAlarm3 ;      m_AlarmLevel[3 ] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[4 ] = ui->btnChAlarm4 ;      m_AlarmLevel[4 ] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[5 ] = ui->btnChAlarm5 ;      m_AlarmLevel[5 ] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[6 ] = ui->btnChAlarm6 ;      m_AlarmLevel[6 ] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[7 ] = nullptr;               m_AlarmLevel[7 ] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[8 ] = ui->btnChAlarm8 ;      m_AlarmLevel[8 ] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[9 ] = ui->btnChAlarm9 ;      m_AlarmLevel[9 ] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[10] = ui->btnChAlarm10;      m_AlarmLevel[10] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[11] = ui->btnChAlarm11;      m_AlarmLevel[11] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[12] = ui->btnChAlarm12;      m_AlarmLevel[12] = EN_WARNING_LEVEL;
    m_pAlarmBtns[13] = ui->btnChAlarm13;      m_AlarmLevel[13] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[14] = ui->btnChAlarm14;      m_AlarmLevel[14] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[15] = ui->btnChAlarm15;      m_AlarmLevel[15] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[16] = ui->btnChAlarm16;      m_AlarmLevel[16] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[17] = ui->btnChAlarm17;      m_AlarmLevel[17] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[18] = ui->btnChAlarm18;      m_AlarmLevel[18] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[19] = ui->btnChAlarm19;      m_AlarmLevel[19] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[20] = nullptr;               m_AlarmLevel[20] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[21] = nullptr;               m_AlarmLevel[21] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[22] = ui->btnChAlarm22;      m_AlarmLevel[22] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[23] = ui->btnChAlarm23;      m_AlarmLevel[23] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[24] = ui->btnChAlarm24;      m_AlarmLevel[24] = EN_WARNING_LEVEL;
    m_pAlarmBtns[25] = ui->btnChAlarm25;      m_AlarmLevel[25] = EN_WARNING_LEVEL;
    m_pAlarmBtns[26] = ui->btnChAlarm26;      m_AlarmLevel[26] = EN_WARNING_LEVEL;
    m_pAlarmBtns[27] = ui->btnChAlarm27;      m_AlarmLevel[27] = EN_WARNING_LEVEL;
    m_pAlarmBtns[28] = ui->btnChAlarm28;      m_AlarmLevel[28] = EN_WARNING_LEVEL;
    m_pAlarmBtns[29] = ui->btnChAlarm29;      m_AlarmLevel[29] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[30] = nullptr;               m_AlarmLevel[30] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[31] = nullptr;               m_AlarmLevel[31] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[32] = nullptr;               m_AlarmLevel[32] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[33] = ui->btnChAlarm33;      m_AlarmLevel[33] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[34] = ui->btnChAlarm34;      m_AlarmLevel[34] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[35] = ui->btnChAlarm35;      m_AlarmLevel[35] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[36] = ui->btnChAlarm36;      m_AlarmLevel[36] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[37] = ui->btnChAlarm37;      m_AlarmLevel[37] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[38] = ui->btnChAlarm38;      m_AlarmLevel[38] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[39] = ui->btnChAlarm39;      m_AlarmLevel[39] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[40] = ui->btnChAlarm40;      m_AlarmLevel[40] = EN_WARNING_LEVEL;
    m_pAlarmBtns[41] = ui->btnChAlarm41;      m_AlarmLevel[41] = EN_WARNING_LEVEL;
    m_pAlarmBtns[42] = ui->btnChAlarm42;      m_AlarmLevel[42] = EN_WARNING_LEVEL;
    m_pAlarmBtns[43] = ui->btnChAlarm43;      m_AlarmLevel[43] = EN_WARNING_LEVEL;
    m_pAlarmBtns[44] = ui->btnChAlarm44;      m_AlarmLevel[44] = EN_WARNING_LEVEL;
    m_pAlarmBtns[45] = ui->btnChAlarm45;      m_AlarmLevel[45] = EN_WARNING_LEVEL;
    m_pAlarmBtns[46] = ui->btnChAlarm46;      m_AlarmLevel[46] = EN_WARNING_LEVEL;
    m_pAlarmBtns[47] = ui->btnChAlarm47;      m_AlarmLevel[47] = EN_WARNING_LEVEL;
    m_pAlarmBtns[48] = ui->btnChAlarm48;      m_AlarmLevel[48] = EN_WARNING_LEVEL;
    m_pAlarmBtns[49] = ui->btnChAlarm49;      m_AlarmLevel[49] = EN_WARNING_LEVEL;
    m_pAlarmBtns[50] = ui->btnChAlarm50;      m_AlarmLevel[50] = EN_WARNING_LEVEL;
    m_pAlarmBtns[51] = ui->btnChAlarm51;      m_AlarmLevel[51] = EN_WARNING_LEVEL;
    m_pAlarmBtns[52] = nullptr;               m_AlarmLevel[52] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[53] = nullptr;               m_AlarmLevel[53] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[54] = nullptr;               m_AlarmLevel[54] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[55] = nullptr;               m_AlarmLevel[55] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[56] = nullptr;               m_AlarmLevel[56] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[57] = nullptr;               m_AlarmLevel[57] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[58] = nullptr;               m_AlarmLevel[58] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[59] = nullptr;               m_AlarmLevel[59] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[60] = nullptr;               m_AlarmLevel[60] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[61] = nullptr;               m_AlarmLevel[61] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[62] = nullptr;               m_AlarmLevel[62] = EN_HEAVY_LEVEL;
    m_pAlarmBtns[63] = nullptr;               m_AlarmLevel[63] = EN_HEAVY_LEVEL;


    for( int alarm=0;alarm<EN_ALARM_COUNT::EN_CHANANEL_ALARM_COUNT;alarm++ )
    {
        if( m_pAlarmBtns[alarm] != nullptr )
        {
            connect( m_pAlarmBtns[alarm], &QPushButton::clicked, this, &CZone::SlotAlarmClicked );
        }
    }
    connect( ui->btnSysPwr, &QPushButton::clicked, this, &CZone::SlotSysPwrClicked );
}

void CZone::SlotAlarmClicked( bool bChecked )
{
    for( int alarm=0;alarm<EN_ALARM_COUNT::EN_CHANANEL_ALARM_COUNT;alarm++ )
    {
        if( m_pAlarmBtns[alarm] == nullptr ) continue;
        if( sender() == m_pAlarmBtns[alarm] )
        {
            if( bChecked )
            {
                if( m_AlarmLevel[alarm] == EN_HEAVY_LEVEL ) m_pAlarmBtns[alarm]->setStyleSheet(ALARM_ALARM_STYLE  );
                else                                        m_pAlarmBtns[alarm]->setStyleSheet(ALARM_WARNING_STYLE);
            }
            else
            {
                m_pAlarmBtns[alarm]->setStyleSheet(ALARM_NORMAL_STYLE);
            }
        }
    }
}

////////////////////////////////////////
/// \brief CZone::SlotSysPwrClicked
/// \author KWMOON
/// \date 2021.05.31
////////////////////////////////////////
void CZone::SlotSysPwrClicked(bool bChecked)
{
    if( bChecked ) m_stZoneStatus.nSystemPowerOnMode = 0x1;
    else           m_stZoneStatus.nSystemPowerOnMode = 0x0;
    InitSystemPower();
}

void CZone::ClearAlarm()
{
    for( int alarm=0;alarm<EN_CHANANEL_ALARM_COUNT;alarm++ )
    {
        if( m_pAlarmBtns[alarm] != nullptr )
        {
            m_pAlarmBtns[alarm]->setChecked( false );
            m_pAlarmBtns[alarm]->setStyleSheet(ALARM_NORMAL_STYLE);
        }
    }
}

void CZone::SetChamberStatus(EN_CHAMBER_STATUS stat)
{
    switch( stat )
    {
        case EN_CHAMBER_STATUS::EN_STATUS_RUN:
            ui->lblRun->setStyleSheet(STATUS_RUN_STYLE);
            ui->lblStop->setStyleSheet(STATUS_NORM_STYLE);
            ui->lblAmb->setStyleSheet(STATUS_NORM_STYLE);
            ui->lblDV->setStyleSheet(STATUS_NORM_STYLE);
            m_stZoneStatus.nTempRunMode  = EN_STATUS_RUN;
            break;
        case EN_CHAMBER_STATUS::EN_STATUS_STOP:
            ui->lblStop->setStyleSheet(STATUS_STOP_STYLE);
            ui->lblRun->setStyleSheet(STATUS_NORM_STYLE);
            ui->lblAmb->setStyleSheet(STATUS_NORM_STYLE);
            ui->lblDV->setStyleSheet(STATUS_NORM_STYLE);
            m_stZoneStatus.nTempRunMode  = EN_STATUS_STOP;
            break;
        case EN_CHAMBER_STATUS::EN_STATUS_AMBIENT:
            ui->lblAmb->setStyleSheet(STATUS_AMBIENT_STYLE);
            ui->lblStop->setStyleSheet(STATUS_NORM_STYLE);
            ui->lblRun->setStyleSheet(STATUS_NORM_STYLE);
            ui->lblDV->setStyleSheet(STATUS_NORM_STYLE);
            m_stZoneStatus.nTempRunMode  = EN_STATUS_AMBIENT;
            break;
        case EN_CHAMBER_STATUS::EN_STATUS_DV:
            ui->lblDV->setStyleSheet(STATUS_DV_STYLE);
            ui->lblStop->setStyleSheet(STATUS_NORM_STYLE);
            ui->lblRun->setStyleSheet(STATUS_RUN_STYLE);
            ui->lblAmb->setStyleSheet(STATUS_NORM_STYLE);
            m_stZoneStatus.nTempRunMode  = EN_STATUS_DV;
            break;
        default:
            ui->lblDV->setStyleSheet(STATUS_NORM_STYLE);
            ui->lblStop->setStyleSheet(STATUS_NORM_STYLE);
            ui->lblRun->setStyleSheet(STATUS_NORM_STYLE);
            ui->lblAmb->setStyleSheet(STATUS_NORM_STYLE);
            m_stZoneStatus.nTempRunMode  = EN_STATUS_UNKNOWN;
            break;
    }
}

EN_CHAMBER_STATUS CZone::GetStatus()
{
    return m_stZoneStatus.nTempRunMode;
}

void CZone::SetMaskRemote(bool bMask)
{
    if( bMask )
    {
        m_stZoneStatus.nMaskOnMode  = 1;
        m_stZoneStatus.nMaskOffMode = 0;
        ui->lblMask->setText("MASK ON");
        ui->lblMask->setStyleSheet( MASK_REMOTE_ON );
    }
    else
    {
        m_stZoneStatus.nMaskOnMode  = 0;
        m_stZoneStatus.nMaskOffMode = 1;
        ui->lblMask->setText("MASK OFF");
        ui->lblMask->setStyleSheet( MASK_REMOTE_OFF );
    }
}

bool CZone::GetMaskRemote()
{
    if( m_stZoneStatus.nMaskOnMode == 1 ) return true;
    return false;
}

void CZone::SetSv(double sv)
{
	QString sFmt = QString("").asprintf("%5.1f", sv);
    m_stZoneStatus.dSv = sv;
    ui->lcdSV->display(sFmt);
}

void CZone::SetDv(double dv)
{
	QString sFmt = QString("").asprintf("%5.1f", dv);
    m_stZoneStatus.dDv = dv;
    ui->lcdDV->display(sFmt);
}

void CZone::SetAmbientSv(double sv)
{
	QString sFmt = QString("").asprintf("%5.1f", sv);
    m_stZoneStatus.dAmbSv = sv;
}

void CZone::GetTemp(double& sv,double& pv, double& dv )
{
    pv = ui->spinPV->value();
    sv = m_stZoneStatus.dSv;
    dv = ui->lcdDV->value();
}

bool CZone::DoChannelCmd( EN_PLC_ADDRESS_MATCH_CMD cmd, YOKOGAWA_PLC_CMD_TYPE type, int set, int& get )
{
    double dPv = 0.0;
    double dSv = 0.0;
    double dDv = 0.0;
    switch( cmd )
    {
    case EN_GET_CH_TEMP_RUN_MODE        :
        LogMessage( "GET_TEMP_RUN_MODE", true );
        get = ( m_stZoneStatus.nTempRunMode == EN_CHAMBER_STATUS::EN_STATUS_RUN)?0x1:0x0;
        break;
    case EN_GET_CH_TEMP_STOP_MODE       :
        LogMessage( "GET_TEMP_STOP_MODE", true );
        get = ( m_stZoneStatus.nTempRunMode == EN_CHAMBER_STATUS::EN_STATUS_STOP)?0x1:0x0;
        break;
    case EN_GET_CH_TEMP_AMB_MODE        :
        LogMessage( "GET_TEMP_AMBIENT_MODE", true );
        get = ( m_stZoneStatus.nTempRunMode == EN_CHAMBER_STATUS::EN_STATUS_AMBIENT)?0x1:0x0;
        break;
    //2022.04.04 - KWMOON
    case EN_GET_CH_TEMP_DV_MODE                :
        LogMessage( "GET_CH_TEMP_DV_MODE", true );
        get = ( m_stZoneStatus.nTempRunMode == EN_CHAMBER_STATUS::EN_STATUS_DV)?0x1:0x0;
        break;
    case EN_GET_CH_DOOR_CLOSE_COMPLETE  :
        LogMessage( "GET_DOOR_CLOSE_COMPLETE", true );
        get = m_stZoneStatus.nDoorCloseComplete & 0x1;
        break;
    case EN_GET_CH_SLOT1_POWER_ON       :
        LogMessage( "GET_SLOT1_POWER_ON", true );
        get = m_stZoneStatus.nSlot1PowerOnMode & 0x1;
        break;
    case EN_GET_CH_SLOT2_POWER_ON       :
        LogMessage( "GET_SLOT2_POWER_ON", true );
        get = m_stZoneStatus.nSlot2PowerOnMode & 0x1;
        break;
    case EN_GET_CH_MASK_OFF_MODE        :
        LogMessage( "GET_CH_MASK_OFF_MODE", true );
        get = m_stZoneStatus.nMaskOffMode & 0x1;
        break;
    case EN_GET_CH_MASK_ON_MODE         :
        LogMessage( "GET_CH_MASK_ON_MODE", true );
        get = m_stZoneStatus.nMaskOnMode & 0x1;
        break;
    case EN_GET_CH_STATUS               :
        LogMessage( "GET_CH_STATUS", true );
        if( m_stZoneStatus.bDvMode && m_stZoneStatus.nTempRunMode == EN_CHAMBER_STATUS::EN_STATUS_DV )
        {
            get  = 0x1<<0;
            get |= 0x1<<3;
        }
        else
        {
            get  = (( m_stZoneStatus.nTempRunMode == EN_CHAMBER_STATUS::EN_STATUS_RUN )?0x1<<0:0x0<<0);
            get |= 0x0<<3;
        }
        get |= (( m_stZoneStatus.nTempRunMode == EN_CHAMBER_STATUS::EN_STATUS_STOP   )?0x1<<1:0x0<<1);
        get |= (( m_stZoneStatus.nTempRunMode == EN_CHAMBER_STATUS::EN_STATUS_AMBIENT)?0x1<<2:0x0<<2);
        get |= ( m_stZoneStatus.nDoorCloseComplete & 0x1) << 4;
        get |= ( m_stZoneStatus.nSlot1PowerOnMode  & 0x1) << 5;
        get |= ( m_stZoneStatus.nSlot2PowerOnMode  & 0x1) << 6;
        get |= ( m_stZoneStatus.nSystemPowerOnMode & 0x1) << 7;
        get |= ( m_stZoneStatus.nMaskOffMode       & 0x1) << 8;
        get |= ( m_stZoneStatus.nMaskOnMode        & 0x1) << 9;
        break;
    case EN_GET_CHAMBER_PV              :
        LogMessage( "GET_CHAMBER_PV", true );
        GetTemp( dSv, dPv, dDv );
        get = static_cast<short>(dPv*10.0)&0xFFFF;
        break;
    case EN_GET_CHAMBER_SV              :
        LogMessage( "GET_CHAMBER_SV", true );
        GetTemp( dSv, dPv, dDv );
        get = static_cast<short>(dSv*10.0)&0xFFFF;
        break;
    case EN_GET_CH_ALARM_EACH           :
        LogMessage( "GET_ALARM_EACH", true );
        get = 0x0;
        break;
    case EN_GET_CH_ALARM_G0             :
        LogMessage( "GET_ALARM_G0", true );
        get = 0x0;
        for( int a=0;a<16;a++ )
        {
            if( m_pAlarmBtns[a] == nullptr ) continue;
            if( m_pAlarmBtns[a]->isChecked() )
            {
                get |= (1<<(a-0));
            }
        }
        break;
    case EN_GET_CH_ALARM_G1             :
        LogMessage( "GET_ALARM_G1", true );
        get = 0x0;
        for( int a=16;a<32;a++ )
        {
            if( m_pAlarmBtns[a] == nullptr ) continue;
            if( m_pAlarmBtns[a]->isChecked() )
            {
                get |= (1<<(a-16));
            }
        }
        break;
    case EN_GET_CH_ALARM_G2             :
        LogMessage( "GET_ALARM_G2", true );
        get = 0x0;
        for( int a=32;a<48;a++ )
        {
            if( m_pAlarmBtns[a] == nullptr ) continue;
            if( m_pAlarmBtns[a]->isChecked() )
            {
                get |= (1<<(a-32));
            }
        }
        break;
    case EN_GET_CH_ALARM_G3             :
        LogMessage( "GET_ALARM_G3", true );
        get = 0x0;
        for( int a=48;a<64;a++ )
        {
            if( m_pAlarmBtns[a] == nullptr ) continue;
            if( m_pAlarmBtns[a]->isChecked() )
            {
                get |= (1<<(a-48));
            }
        }
        break;
    case EN_SET_CH_EMPTY                :
        get = 0x0;
        break;
    case EN_SET_CH_TEMP_RUN_TRIGGER     :
        LogMessage( "SET_TEMP_RUN_TRIGGER", true );
        if( m_stZoneStatus.nTempRunMode == EN_CHAMBER_STATUS::EN_STATUS_DV )
            SetChamberStatus( EN_STATUS_DV  );
        else
            SetChamberStatus( EN_STATUS_RUN );
        InitChamberStatus();
        break;
    case EN_SET_CH_TEMP_STOP_TRIGGER    :
        LogMessage( "SET_TEMP_STOP_TRIGGER", true );
        SetChamberStatus( EN_STATUS_STOP );
        InitChamberStatus();
        break;
    case EN_SET_CH_AMBIENT_MODE_TRIGGER :
    case EN_SET_CH_AMBIENT_AUTO_TRIGGER :
        LogMessage( "SET_TEMP_AMBIENT_AUTO_TRIGGER", true );
        SetChamberStatus( EN_STATUS_AMBIENT );
        InitChamberStatus();
        break;
    //2022.04.04 - KWMOON
    case EN_SET_CH_DV_TRIGGER :
        LogMessage( "SET_CH_DV_TRIGGER", true );
        if( set == 0x1 )
        {
            m_stZoneStatus.bDvMode = true;
            SetChamberStatus( EN_STATUS_DV );
            InitChamberStatus();
        }
        else
        {
            m_stZoneStatus.bDvMode = false;
            SetChamberStatus( EN_STATUS_RUN );
            InitChamberStatus();
        }

        break;
    case EN_SET_CH_SLOT1_POWER_ON_OFF       :
        LogMessage( "SET_SLOT1_POWER_ON_OFF", true );
        m_stZoneStatus.nSlot1PowerOnMode = set;
        InitSlotPower();
        break;
    case EN_SET_CH_SLOT2_POWER_ON_OFF       :
        LogMessage( "SET_SLOT2_POWER_ON_OFF", true );
        m_stZoneStatus.nSlot2PowerOnMode = set;
        InitSlotPower();
        break;
    //2021.04.09 - KWMOON
    case EN_SET_CH_SYSTEM_POWER_ON_OFF      :
        LogMessage( "SET_SYSTEM_POWER_ON_OFF", true );
        m_stZoneStatus.nSystemPowerOnMode = set;
        InitSystemPower();
        break;

    case EN_SET_CH_MASK_OFF             :
        LogMessage( "SET_MASK_OFF", true );
        m_stZoneStatus.nMaskOffMode = 0x1;
        m_stZoneStatus.nMaskOnMode  = 0x0;
        InitDoorAndMask();
        break;
    case EN_SET_CH_MASK_ON              :
        LogMessage( "SET_MASK_ON", true );
        m_stZoneStatus.nMaskOffMode = 0x0;
        m_stZoneStatus.nMaskOnMode  = 0x1;
        InitDoorAndMask();
        break;
    case EN_SET_CH_CHAMBER_AMB_SV       :
        LogMessage( "SET_AMBIENT_SV", true );
        SetAmbientSv( static_cast<double>( static_cast<short>(set) / 10.0 ) );
        break;
    case EN_SET_CH_CHAMBER_SV           :
        LogMessage( "SET_TEMP_SV", true );
        SetSv( static_cast<double>( static_cast<short>(set) / 10.0 ) );
        break;
    //2022.04.05 - KWMOON
    case EN_SET_CH_CHAMBER_DV           :
        LogMessage( "SET_TEMP_DV", true );
        SetDv( static_cast<double>( static_cast<short>(set) / 10.0 ) );
        break;
    case EN_SET_CH_TESTER_WARN_ON_OFF    :
        LogMessage( "SET_SYSTEM_POWER_ON_OFF", true );
        InitTesterAlarm( set );
        break;
    case EN_GET_CHAMBER_DV              :
        LogMessage( "GET_CHAMBER_DV", true );
        GetTemp( dSv, dPv, dDv );
        get = static_cast<short>(dDv*10.0)&0xFFFF;
        break;

    default:
        break;
    }
    return true;
}

void CZone::LogMessage( QString msg, bool pass )
{
	QString log = QString("").asprintf( "[CH#%02d ][ CMD][INFO] %s", m_nChannelNo, msg.toLatin1().data() );
    emit SigPlcLogMsg( log, pass );

}
