#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect( &m_systray_x_link, &SysTrayXLink::signalReceivedMessageLength, this, &MainWindow::slotReceivedMessageLength );
    connect( &m_systray_x_link, &SysTrayXLink::signalReceivedMessage, this, &MainWindow::slotReceivedMessage );

    connect( this, &MainWindow::signalWriteMessage, &m_systray_x_link, &SysTrayXLink::slotLinkWrite );
}


MainWindow::~MainWindow()
{
    delete ui;
}


void    MainWindow::slotReceivedMessageLength( qint32 msglen )
{
    ui->label_length->setText(QString::number( msglen ) );
}


void    MainWindow::slotReceivedMessage( QByteArray message )
{
    ui->label_message->setText( QString::fromStdString( message.toStdString() ) );

    /*
     * Reply
     */
    QByteArray reply = QString( "\"Hallo other world!\"" ).toUtf8();
    emit signalWriteMessage( reply );
}
