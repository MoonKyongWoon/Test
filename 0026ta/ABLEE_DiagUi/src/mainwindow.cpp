#include "mainwindow.hpp"

CDiagUi::CDiagUi(QString name, QWidget *parent) : QMainWindow(parent), m_sApplicationName(name)
{
    InitVariable();
    InitInstance();
    InitUi();
}

CDiagUi::~CDiagUi()
{

}

void CDiagUi::InitVariable()
{
    m_pSlotBox = nullptr;
    m_pMainView = nullptr;
    m_listLogMode.clear();
}

void CDiagUi::InitInstance()
{
    m_pSlotBox = new CSlotBox();
	m_pSlotBox->setProperty("CSlotBox", true);
    m_pMainView = new CMainView;
	m_listLogMode << "ALL LOG" << "FAIL LOG";
}

void CDiagUi::InitUi()
{
    InitTitles();
    CreateToolbar();
    CreateDockWindows();
    SetStatusBarText( "Ready" );
}

void CDiagUi::InitTitles()
{
	setWindowTitle( QString("%1").arg(ATCALDIAG_VERSION) );
}

void CDiagUi::CreateToolbar()
{
    QToolBar *fileToolBar = addToolBar(tr("Toolbar"));
    fileToolBar->setFloatable(false);
    fileToolBar->setMovable(false);
    fileToolBar->setHidden(false);
    fileToolBar->setIconSize(QSize(36,36));
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    fileToolBar->setObjectName("ToolBar");

    //SLOT CHECK BOX
    fileToolBar->addWidget( m_pSlotBox );
    fileToolBar->addSeparator();

    //LOAD
    const QIcon icoSetup = QIcon::fromTheme( tr("load"), QIcon(":/icon/icon/load.ico"));
    m_pActLoad = new QAction(icoSetup, tr("LOAD"), this);
    m_pActLoad->setToolTip(tr("Load Program"));
    fileToolBar->addAction(m_pActLoad);

    //Reload
    const QIcon icoReoad = QIcon::fromTheme( tr("reload"), QIcon(":/icon/icon/reload.ico"));
    m_pActReload = new QAction(icoReoad, tr("RELOAD"), this);
    m_pActReload->setToolTip(tr("Reload test program"));
    fileToolBar->addAction(m_pActReload);
    fileToolBar->addSeparator();

    //LOOP COUNT
    QWidget* pLoop = new QWidget;
    pLoop->setMaximumWidth( 120 );
    pLoop->setMinimumWidth( 100 );
    QVBoxLayout* layout = new QVBoxLayout;
    QLabel* pLblLoop = new QLabel("REPEAT COUNT");
    QSpinBox* pSpinLoop = new QSpinBox;
    //pLblLoop->setFrameShape( QFrame::Shape::StyledPanel );
    pLblLoop->setAlignment( Qt::AlignCenter );
    pLblLoop->setMaximumWidth( 130 );
    pLblLoop->setMinimumHeight( 20 );
    pSpinLoop->setMaximumWidth( 130 );
	pSpinLoop->setMinimumHeight( 30 );
    pSpinLoop->setMinimum( 1 );
    pSpinLoop->setMaximum( 10000 );
    layout->addWidget( pLblLoop );
    layout->addWidget( pSpinLoop );
	layout->setContentsMargins( 1, 8, 1, 8 );
	layout->setSpacing( 3 );
    pLoop->setLayout( layout );
    fileToolBar->addWidget( pLoop );
    fileToolBar->addSeparator();

	//LOG MODE
	QWidget* pMode = new QWidget;
	pMode->setMaximumWidth( 160 );
	pMode->setMinimumWidth( 140 );
	QVBoxLayout* vl = new QVBoxLayout;
	QLabel* lblmode = new QLabel("LOG MODE");
	QComboBox* cbomode = new QComboBox;
	//lblmode->setFrameShape( QFrame::Shape::StyledPanel );
	lblmode->setAlignment( Qt::AlignCenter );
	lblmode->setMaximumWidth( 150 );
	lblmode->setMinimumHeight( 20 );
	cbomode->setMaximumWidth( 150 );
	cbomode->setMinimumHeight( 30 );
	cbomode->addItems( m_listLogMode );
	vl->addWidget( lblmode );
	vl->addWidget( cbomode );
	vl->setContentsMargins( 1, 8, 1, 8 );
	vl->setSpacing( 3 );
	pMode->setLayout( vl );
	fileToolBar->addWidget( pMode );
	fileToolBar->addSeparator();

    //TEST
    const QIcon icoTest = QIcon::fromTheme( tr("start"), QIcon(":/icon/icon/test.ico"));
	m_pActTest = new QAction(icoTest, tr("ALL START"), this);
	m_pActTest->setToolTip(tr("All Slot test"));
    fileToolBar->addAction(m_pActTest);

    //STOP
	const QIcon icoStop = QIcon::fromTheme( tr("STOP"), QIcon(":/icon/icon/stop_circle.ico"));
	m_pActStop = new QAction(icoStop, tr("ALL STOP"), this);
	m_pActStop->setToolTip(tr("All Slot stop"));
    fileToolBar->addAction(m_pActStop);
    fileToolBar->addSeparator();

	connect( m_pActLoad    , &QAction::triggered , this, &CDiagUi::SlotActLoad     );     //Load
	connect( m_pActReload  , &QAction::triggered , this, &CDiagUi::SlotActReload   );     //Reload
	connect( m_pActTest    , &QAction::triggered , this, &CDiagUi::SlotActTest     );     //Test
	connect( m_pActStop    , &QAction::triggered , this, &CDiagUi::SlotActStop     );     //Stop
}

void CDiagUi::SetStatusBarText( QString msg )
{
    statusBar()->showMessage( msg );
}

void CDiagUi::CreateDockWindows()
{
    this->setCentralWidget( m_pMainView );
}

void CDiagUi::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED( event )

}

void CDiagUi::ResizeWidgets()
{

}

void CDiagUi::closeEvent(QCloseEvent *event)
{
    if(event->type() == QEvent::Close)
    {
        QMessageBox::StandardButton resBtn =
        QMessageBox::question( this, m_sApplicationName, tr("Do you want to quit program?"),
        QMessageBox::No | QMessageBox::Yes,
        QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
        {
            if( 0 )
            {
                QSettings settings( CONFIG_INI_KEY, m_sApplicationName );
                settings.setValue( "geometry"   , saveGeometry() );
                settings.setValue( "windowState", saveState()    );
            }
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
}

void CDiagUi::SlotActLoad()
{

}

void CDiagUi::SlotActReload()
{

}

void CDiagUi::SlotActTest()
{

}

void CDiagUi::SlotActStop()
{

}
