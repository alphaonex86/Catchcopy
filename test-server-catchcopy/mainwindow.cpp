#include <QStringList>
#include <QListWidgetItem>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	isDisconnected();
	connect(&server,SIGNAL(connectedClient(quint32)),this,SLOT(connectedClient(quint32)));
	connect(&server,SIGNAL(disconnectedClient(quint32)),this,SLOT(disconnectedClient(quint32)));
	connect(&server,SIGNAL(newQuery(quint32,quint32,QStringList)),this,SLOT(newQuery(quint32,quint32,QStringList)));
	connect(&server,SIGNAL(clientName(quint32,QString)),this,SLOT(clientName(quint32,QString)));
	connect(&server,SIGNAL(error(QString)),this,SLOT(newError(QString)));
	server.setName("Test server catchcopy");
	server.setAutoReply(false);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::clientName(quint32 client,QString name)
{
	int index=0;
	while(index<ClientList.size())
	{
		if(ClientList.at(index).id==client)
		{
			if(ClientList.at(index).name=="")
			{
				ClientList[index].name=name;
				ClientList.at(index).guiItem->setText(QString::number(ClientList.at(index).id)+") "+name);
			}
			return;
		}
		index++;
	}
}

void MainWindow::isConnected()
{
	ui->actionDisconnect->setEnabled(true);
	ui->actionListen->setEnabled(false);
	ui->groupBoxClients->setEnabled(true);
	ui->groupBoxQuery->setEnabled(false);
	ui->groupBoxReply->setEnabled(false);
}

void MainWindow::isDisconnected()
{
	ClientList.clear();
	ui->listClient->clear();
	ui->listCommandListSended->clear();
	ui->listCommand->clear();
	ui->actionDisconnect->setEnabled(false);
	ui->actionListen->setEnabled(true);
	ui->groupBoxClients->setEnabled(false);
	ui->groupBoxQuery->setEnabled(false);
	ui->groupBoxReply->setEnabled(false);
}

void MainWindow::on_actionListen_triggered()
{
	if(server.listen())
		isConnected();
	else
		isDisconnected();
}

void MainWindow::on_actionDisconnect_triggered()
{
	server.close();
	isDisconnected();
}

void MainWindow::connectedClient(quint32 id)
{
	Client newClient;
	newClient.id=id;
	newClient.guiItem=new QListWidgetItem("???",ui->listClient);
	ui->listClient->addItem(newClient.guiItem);
	ClientList << newClient;
	updateClientName(id);
}

void MainWindow::updateClientName(quint32 id)
{
	int index=0;
	while(index<ClientList.size())
	{
		if(ClientList.at(index).id==id)
		{
			ClientList.at(index).guiItem->setText(QString::number(ClientList.at(index).id)+") Unknow");
			return;
		}
		index++;
	}
}

void MainWindow::disconnectedClient(quint32 id)
{
	int index=0;
	while(index<ClientList.size())
	{
		if(ClientList.at(index).id==id)
		{
			delete ClientList.at(index).guiItem;
			ClientList.removeAt(index);
			return;
		}
		index++;
	}
}

void MainWindow::on_toolButton_clicked()
{
	int index=0;
	while(index<ClientList.size())
	{
		if(ClientList.at(index).guiItem==ui->listClient->currentItem())
		{
			server.disconnectClient(ClientList.at(index).id);
			return;
		}
		index++;
	}
}

void MainWindow::newQuery(quint32 client,quint32 orderId,QStringList returnList)
{
	int index=0;
	while(index<ClientList.size())
	{
		if(ClientList.at(index).id==client)
		{
			ClientList[index].idQuery << orderId;
			ClientList[index].queryContent << returnList;
			ClientList[index].timeQuery << QTime::currentTime();
			if(ui->listClient->currentItem()==ClientList.at(index).guiItem)
			{
				ui->listCommand->addItem(new QListWidgetItem(
				ClientList.at(index).timeQuery.last().toString()+
				") "+QString::number(ClientList.at(index).idQuery.last())+
				" - "+ClientList.at(index).queryContent.last().first()+" ("+QString::number(ClientList.at(index).queryContent.last().size())+")",
				ui->listCommand));
			}
			on_listCommand_itemSelectionChanged();
			return;
		}
		index++;
	}
}

void MainWindow::on_listClient_itemSelectionChanged()
{
	ui->listCommandListSended->clear();
	ui->listCommand->clear();
	int index=0;
	while(index<ClientList.size())
	{
		if(ClientList.at(index).guiItem==ui->listClient->currentItem())
		{
			ui->groupBoxQuery->setEnabled(true);
			ui->groupBoxReply->setEnabled(true);
			int idQuery=0;
			while(idQuery<ClientList.at(index).idQuery.size())
			{
				ui->listCommand->addItem(new QListWidgetItem(
				ClientList.at(index).timeQuery.at(idQuery).toString()+
				") "+QString::number(ClientList.at(index).idQuery.at(idQuery))+
				" - "+ClientList.at(index).queryContent.at(idQuery).first(),
				ui->listCommand));
				idQuery++;
			}
			on_listCommand_itemSelectionChanged();
			return;
		}
		index++;
	}
	ui->groupBoxQuery->setEnabled(false);
	ui->groupBoxReply->setEnabled(false);
}

void MainWindow::on_listCommand_itemSelectionChanged()
{
	ui->listCommandListSended->clear();
	int index=0;
	while(index<ClientList.size())
	{
		if(ClientList.at(index).guiItem==ui->listClient->currentItem())
		{
			if(ui->listCommand->currentRow()>=0 && ui->listCommand->currentRow()<ClientList.at(index).idQuery.size())
			{
				ui->labelQuery->setEnabled(true);
				ui->listCommandListSended->setEnabled(true);
				ui->groupBoxReply->setEnabled(true);
				ui->listCommandListSended->addItems(ClientList.at(index).queryContent.at(ui->listCommand->currentRow()));
			}
			else
			{
				ui->labelQuery->setEnabled(false);
				ui->listCommandListSended->setEnabled(false);
				ui->groupBoxReply->setEnabled(false);
			}
			return;
		}
		index++;
	}
	ui->labelQuery->setEnabled(false);
	ui->listCommandListSended->setEnabled(false);
	ui->groupBoxReply->setEnabled(false);
}

void MainWindow::on_sendRawList_clicked()
{
	if(ui->listRawSend->count()==0)
	{
		QMessageBox::information(this,"Warning","You need put one argument into the reply list, thanks");
		return;
	}
	ui->listCommandListSended->clear();
	QStringList reply;
	int indexGui=0;
	while(indexGui<ui->listRawSend->count())
	{
		reply << ui->listRawSend->item(indexGui)->text();
		indexGui++;
	}
	server.reply(getClientSelected(),getQuerySelected(),ui->spinBoxReturnCode->value(),reply);
	replySend();
}

void MainWindow::replySend()
{
	quint32 client=getClientSelected();
	quint32 orderId=getQuerySelected();
	ui->listCommandListSended->clear();
	int index=0;
	while(index<ClientList.size())
	{
		if(ClientList.at(index).id==client)
		{
			int indexGui=0;
			while(indexGui<ClientList.at(index).idQuery.size())
			{
				if(ClientList.at(index).idQuery.at(indexGui)==orderId)
				{
					ClientList[index].idQuery.removeAt(indexGui);
					ClientList[index].queryContent.removeAt(indexGui);
					ClientList[index].timeQuery.removeAt(indexGui);
					on_listClient_itemSelectionChanged();
				}
				indexGui++;
			}
			return;
		}
		index++;
	}
	return;
}

void MainWindow::on_RawSendAdd_clicked()
{
	ui->listRawSend->addItem(new QListWidgetItem(ui->lineEditRawSendText->text(),ui->listRawSend));
}

void MainWindow::on_RawSendRemove_clicked()
{
	if(ui->listRawSend->currentItem()!=NULL)
		delete ui->listRawSend->currentItem();
}

quint32 MainWindow::getClientSelected()
{
	int index=0;
	while(index<ClientList.size())
	{
		if(ClientList.at(index).guiItem==ui->listClient->currentItem())
			return ClientList.at(index).id;
		index++;
	}
	qWarning() << "client not found";
	return -1;
}

quint32 MainWindow::getQuerySelected()
{
	int index=0;
	while(index<ClientList.size())
	{
		if(ClientList.at(index).guiItem==ui->listClient->currentItem())
		{
			if(ui->listCommand->currentRow()>=0 && ui->listCommand->currentRow()<ClientList.at(index).idQuery.size())
				return ClientList.at(index).idQuery.at(ui->listCommand->currentRow());
			qWarning() << "query not found";
			return 0;
		}
		index++;
	}
	qWarning() << "client not found";
	return 0;
}


void MainWindow::on_pushButtonReplyProtocoSupported_clicked()
{
	server.protocolSupported(getClientSelected(),getQuerySelected(),true);
	replySend();
}

void MainWindow::on_pushButtonReplyProtocolNotSupported_clicked()
{
	server.protocolSupported(getClientSelected(),getQuerySelected(),false);
	replySend();
}

void MainWindow::on_pushButtonReplyUnknowOrder_clicked()
{
	server.unknowOrder(getClientSelected(),getQuerySelected());
	replySend();
}

void MainWindow::on_pushButtonReplyWrongArg_clicked()
{
	server.incorrectArgument(getClientSelected(),getQuerySelected());
	replySend();
}

void MainWindow::on_pushButtonReplyWrongArgListSize_clicked()
{
	server.incorrectArgumentListSize(getClientSelected(),getQuerySelected());
	replySend();
}

void MainWindow::on_pushButtonReplyCopyCanceled_clicked()
{
	server.copyCanceled(getClientSelected(),getQuerySelected());
	replySend();
}

void MainWindow::on_pushButtonReplyCopyFinishedWithError_clicked()
{
	server.copyFinished(getClientSelected(),getQuerySelected(),true);
	replySend();
}

void MainWindow::on_pushButtonReplyCopyFinished_clicked()
{
	server.copyFinished(getClientSelected(),getQuerySelected(),false);
	replySend();
}

void MainWindow::on_pushButtonReplyServerName_clicked()
{
	server.serverName(getClientSelected(),getQuerySelected(),"Test server catchcopy");
	replySend();
}

void MainWindow::on_pushButtonReplyClientRegistred_clicked()
{
	server.clientRegistered(getClientSelected(),getQuerySelected());
	replySend();
}

void MainWindow::on_pushButtonReplyExtensionNotSupported_clicked()
{
	server.protocolExtensionSupported(getClientSelected(),getQuerySelected(),false);
	replySend();
}

void MainWindow::on_pushButtonReplyExtensionSupported_clicked()
{
	server.protocolExtensionSupported(getClientSelected(),getQuerySelected(),true);
	replySend();
}

void MainWindow::newError(QString error)
{
	ui->listError->addItem(error);
}

void MainWindow::on_checkBoxAutoReply_toggled(bool checked)
{
	Q_UNUSED(checked)
	server.setAutoReply(ui->checkBoxAutoReply->isChecked());
}
