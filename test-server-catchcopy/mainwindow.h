#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QStringList>
#include <QList>
#include <QListWidgetItem>

#include "catchcopy-api-0002/ServerCatchcopy.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();
	protected:
		void changeEvent(QEvent *e);
	private slots:
		void on_actionListen_triggered();
		void on_actionDisconnect_triggered();
		void connectedClient(quint32 id);
		void updateClientName(quint32 id);
		void disconnectedClient(quint32 id);
		void on_toolButton_clicked();
		void newQuery(quint32 client,quint32 orderId,QStringList returnList);
		void on_listClient_itemSelectionChanged();
		void on_listCommand_itemSelectionChanged();
		void on_sendRawList_clicked();
		void on_RawSendAdd_clicked();
		void on_RawSendRemove_clicked();
		void replySend();
		void on_pushButtonReplyProtocoSupported_clicked();
		void on_pushButtonReplyProtocolNotSupported_clicked();
		void on_pushButtonReplyUnknowOrder_clicked();
		void on_pushButtonReplyWrongArg_clicked();
		void on_pushButtonReplyWrongArgListSize_clicked();
		void on_pushButtonReplyCopyCanceled_clicked();
		void on_pushButtonReplyCopyFinishedWithError_clicked();
		void on_pushButtonReplyCopyFinished_clicked();
		void on_pushButtonReplyServerName_clicked();
		void on_pushButtonReplyClientRegistred_clicked();
		void on_pushButtonReplyExtensionNotSupported_clicked();
		void on_pushButtonReplyExtensionSupported_clicked();
		void clientName(quint32 client,QString name);
		void newError(QString error);
		void on_checkBoxAutoReply_toggled(bool checked);
private:
		Ui::MainWindow *ui;
		ServerCatchcopy server;
		struct Client
		{
			quint32 id;
			QListWidgetItem *guiItem;
			QList<quint32> idQuery;
			QList<QStringList> queryContent;
			QList<QTime> timeQuery;
			QString name;
		};
		typedef struct Client Client;
		QList<Client> ClientList;
		void isConnected();
		void isDisconnected();
		quint32 getClientSelected();
		quint32 getQuerySelected();
};

#endif // MAINWINDOW_H
