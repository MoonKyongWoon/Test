#include "diskspace.hpp"

CDiskSpace::CDiskSpace(QWidget *parent) : QWidget(parent)
{
    m_tmrDiskSpace = nullptr;
    InitUi();
    DisplayDiskSpace( QDir::homePath() );
    InitTimer();
}

CDiskSpace::~CDiskSpace()
{
    if( m_tmrDiskSpace->isActive() )
        m_tmrDiskSpace->stop();
}

void CDiskSpace::InitUi()
{
    m_pUsageDir = new QLabel;
    m_pDiskDesc = new QLabel;
    m_pUsage = new QProgressBar;

    m_pUsageDir->setFrameShape( QFrame::Shape::Panel );
    m_pUsageDir->setFrameShadow( QFrame::Shadow::Raised );
    m_pUsageDir->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    m_pUsageDir->setMaximumWidth( 110 );
    m_pUsageDir->setStyleSheet("font-weight: bold; color: white");
    m_pUsage->setMaximumWidth( 150 );
    m_pUsage->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );

    m_pDiskDesc->setMaximumWidth( 180 );
    m_pDiskDesc->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
    this->setMaximumWidth( 440 );

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget( m_pUsageDir );
    layout->addWidget( m_pUsage );
    layout->addWidget( m_pDiskDesc );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 1 );
    setLayout( layout );
}

void CDiskSpace::InitTimer()
{
    m_tmrDiskSpace = new QTimer(this);
    connect(m_tmrDiskSpace   , &QTimer::timeout, this, &CDiskSpace::SlotTimerDiskSizeView  );
}

void CDiskSpace::SetDiskUsagePercent(int nPercent)
{
    if( nPercent < 95 ) m_pUsage->setStyleSheet(STYLE_NORM);
    else                m_pUsage->setStyleSheet(STYLE_FULL);
    m_pUsage->setValue(nPercent);
}

void CDiskSpace::SetRoot(QString root)
{
    m_sRoot = root;
    m_pUsageDir->setText( root );
}

void CDiskSpace::SetDiskDescription(QString sDesc)
{
    m_pDiskDesc->setText(sDesc);
}

void CDiskSpace::DisplayDiskSpace(QString disk)
{
    QStorageInfo storage;

    SetRoot( disk );
    storage.setPath( disk );

    if( storage.isValid() )
    {
        qint64 nTotalSize = storage.bytesTotal();       //     / ( 1024*1024*1024);   //GB
        qint64 nAvailSize = storage.bytesAvailable();   // / ( 1024*1024*1024);     //GB
        double dTotalGB   = static_cast<double>(nTotalSize) / ( 1024*1024*1024);   //GB
        double dAvailGB   = static_cast<double>(nAvailSize) / ( 1024*1024*1024);   //GB
        double dUsedGB    = dTotalGB - dAvailGB;
        double dPercent   = (dUsedGB/dTotalGB)*100.0;

        int    nPercent   = static_cast<int>(dPercent+0.5);
        QString sMessage  = QString("").asprintf("%6.1fGB / %6.1fGB Free", dTotalGB, dAvailGB);
        //QString sFormat   = QString("").sprintf("%5.1fGB Used", dUsedGB);
        SetDiskUsagePercent( nPercent );
        SetDiskDescription( sMessage );
    }
}

void CDiskSpace::SlotTimerDiskSizeView()
{
    DisplayDiskSpace( m_sRoot );
}

void CDiskSpace::SlotDiskRootPath( QString disk )
{
    if( !disk.isEmpty() )
    {
        DisplayDiskSpace( disk );
        if( !m_tmrDiskSpace->isActive() )
        {
            m_tmrDiskSpace->start( TIME_DISK_SPACE * 1000 );
        }
    }
}
