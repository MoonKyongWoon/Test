#ifndef DISKSPACE_HPP
#define DISKSPACE_HPP

#include <QDebug>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QStorageInfo>
#include <QTimer>
#include <QWidget>

#define TIME_DISK_SPACE     (3600) //1Hour
#define STYLE_NORM          "QProgressBar {border: 1px solid grey;border-radius: 3px;} QProgressBar::chunk { background-color: #05B8CC; width: 20px; margin : 0.5px;}"
#define STYLE_FULL          "QProgressBar {border: 1px solid grey;border-radius: 3px;} QProgressBar::chunk { background-color: #DC0000; width: 20px; margin : 0.5px;}"

class CDiskSpace : public QWidget
{
    Q_OBJECT
public:
    explicit CDiskSpace(QWidget *parent = nullptr);
    ~CDiskSpace();
private:
    void InitUi();
    void InitTimer();
    void SetDiskUsagePercent(int);
    void SetRoot(QString);
    void SetDiskDescription(QString);
    void DisplayDiskSpace(QString);
private:
    QTimer* m_tmrDiskSpace;
    QString m_sRoot;
    QProgressBar* m_pUsage;
    QLabel* m_pUsageDir;
    QLabel* m_pDiskDesc;
public slots:
    void SlotTimerDiskSizeView();
    void SlotDiskRootPath(QString);

};

#endif // DISKSPACE_HPP

