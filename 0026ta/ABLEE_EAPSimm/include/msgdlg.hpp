#ifndef MSGDLG_HPP
#define MSGDLG_HPP

#include <QDialog>
#include <QMessageBox>

namespace Ui {
	class CMsgDlg;
}

class CMsgDlg : public QDialog
{
	Q_OBJECT

public:
	explicit CMsgDlg(QWidget *parent = nullptr);
	~CMsgDlg();
private:
	void InitUi();
public:
	void SetSectionList(QStringList);
	void SetSection(QString);
	void SetDescription(QString);
	void SetSML(QString);
public:
	void Clear();
signals:
	void SigSave( QString, QString, QString );
public slots:
	void SlotSave();
	void SlotCancel();
private:
	Ui::CMsgDlg *ui;
	QStringList m_listSection;
	QString m_sSection;
	QString m_sDescription;
	QString m_sSML;
};

#endif // MSGDLG_HPP
