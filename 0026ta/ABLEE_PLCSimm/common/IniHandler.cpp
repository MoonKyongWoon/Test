#include "IniHandler.hpp"

CIniHandler::CIniHandler( QString file)
{
    m_file = file;
   // m_pSettings = new QSettings( file, QSettings::IniFormat);
}

bool CIniHandler::writeSettings(QString key, QString val)
{
    QMutexLocker locker(&m_mutex);
    QSettings settings(m_file, QSettings::IniFormat);
    settings.setValue(key, val);
    return true;
}

bool CIniHandler::writeSettings(QString key, int val)
{
    QMutexLocker locker(&m_mutex);
    QSettings settings(m_file, QSettings::IniFormat);
    settings.setValue(key, val);
    return true;
}

bool CIniHandler::readSettings(QString key, QString& rValue)
{
    QMutexLocker locker(&m_mutex);
    QSettings settings(m_file, QSettings::IniFormat);
    if( !settings.contains(key) ) return false;

    rValue = settings.value(key, INI_ERROR_VAL).toString();
    return true;
}

bool CIniHandler::readSettings(QString key, std::string& rValue)
{
    QMutexLocker locker(&m_mutex);
    QSettings settings(m_file, QSettings::IniFormat);
    if( !settings.contains(key) ) return false;

    rValue = settings.value(key, INI_ERROR_VAL).toString().toStdString();
    return true;
}

bool CIniHandler::readSettings(QString key, int& rValue)
{
    QMutexLocker locker(&m_mutex);
    bool bOk = false;
    QSettings settings(m_file, QSettings::IniFormat);
    if( !settings.contains(key) ) return false;

    rValue = settings.value(key, 0).toInt(&bOk);
    if( bOk ) return true;
    else      return false;
}

bool CIniHandler::readSettings(QString key, uint& rValue)
{
    QMutexLocker locker(&m_mutex);
    bool bOk = false;
    QSettings settings(m_file, QSettings::IniFormat);
    if( !settings.contains(key) ) return false;

    rValue = settings.value(key, 0).toUInt(&bOk);
    if( bOk ) return true;
    else      return false;
}

bool CIniHandler::readSettings(QString key, double& rValue)
{
    QMutexLocker locker(&m_mutex);
    bool bOk = false;
    QSettings settings(m_file, QSettings::IniFormat);
    if( !settings.contains(key) ) return false;

    rValue = settings.value(key, 0.0).toDouble(&bOk);
    if( bOk ) return true;
    else      return false;
}

bool CIniHandler::removeSection(QString key)
{
    {
    QMutexLocker locker(&m_mutex);
    QSettings settings(m_file, QSettings::IniFormat);
    settings.beginGroup(key);
    const QStringList childKeys = settings.childKeys();
    if( childKeys.count() > 0 )
        settings.remove("");
    settings.endGroup();
    }
    return existSection(key);
}

bool CIniHandler::existSection(QString key)
{
    QMutexLocker locker(&m_mutex);
    QSettings settings(m_file, QSettings::IniFormat);
    settings.beginGroup(key);
    const QStringList childKeys = settings.childKeys();
    settings.endGroup();
    if( childKeys.count() > 0 ) return true;
    else return false;
}

QStringList CIniHandler::allKeys()
{
    QMutexLocker locker(&m_mutex);
    QSettings settings(m_file, QSettings::IniFormat);
    return settings.childGroups();
}
