#include "preferencesdialog.h"
#include "ui_preferences.h"

/*
 *	Local includes
 */
#include "systrayxlink.h"

/*
 *	Qt includes
 */
#include <QColor>
#include <QPixmap>
#include <QFileDialog>
#include <QColorDialog>
#include <QMimeDatabase>
#include <QJsonDocument>
#include <QJsonObject>

/*
 *  Constructor
 */
PreferencesDialog::PreferencesDialog( SysTrayXLink *link, Preferences *pref, QWidget *parent ) : QDialog( parent ), m_ui( new Ui::PreferencesDialog )
{
    m_ui->setupUi( this );

    /*
     *  Store link adn preferences
     */
    m_link = link;
    m_pref = pref;

    /*
     *  Set minimize type button Ids
     */
    m_ui->minimizeTypeGroup->setId( m_ui->defaultMinimizeRadioButton, Preferences::PREF_DEFAULT_MINIMIZE);
    m_ui->minimizeTypeGroup->setId( m_ui->minimizeMethod1RadioButton, Preferences::PREF_MINIMIZE_METHOD_1 );
    m_ui->minimizeTypeGroup->setId( m_ui->minimizeMethod2RadioButton, Preferences::PREF_MINIMIZE_METHOD_2 );

#ifdef Q_OS_WIN

    m_ui->minimizeMethod1RadioButton->setText( "Minimize to tray" );
    m_ui->minimizeMethod2RadioButton->hide();

#endif

    /*
     *  Set icon type button Ids
     */
    m_ui->iconTypeGroup->setId( m_ui->blankRadioButton, Preferences::PREF_BLANK_ICON );
    m_ui->iconTypeGroup->setId( m_ui->newMailButton, Preferences::PREF_NEWMAIL_ICON );
    m_ui->iconTypeGroup->setId( m_ui->customRadioButton, Preferences::PREF_CUSTOM_ICON );

    /*
     *  Set count type button Ids
     */
    m_ui->countTypeGroup->setId( m_ui->unreadRadioButton, Preferences::PREF_COUNT_UNREAD );
    m_ui->countTypeGroup->setId( m_ui->newRadioButton, Preferences::PREF_COUNT_NEW );

    /*
     *  Hide the count type for now
     */
    m_ui->countTypeGroupBox->setVisible(false);
//    m_ui->unreadRadioButton->setVisible(false);
//    m_ui->newRadioButton->setVisible(false);

    /*
     *  Set icon type defaults
     */
    m_tmp_icon_data = QByteArray();
    m_tmp_icon_mime = QString();

    /*
     *  Set default icon type button Ids
     */
    m_ui->defaultIconTypeGroup->setId( m_ui->defaultIconRadioButton, Preferences::PREF_DEFAULT_ICON_DEFAULT );
    m_ui->defaultIconTypeGroup->setId( m_ui->hideDefaultIconRadioButton, Preferences::PREF_DEFAULT_ICON_HIDE );
    m_ui->defaultIconTypeGroup->setId( m_ui->customDefaultIconRadioButton, Preferences::PREF_DEFAULT_ICON_CUSTOM );

    /*
     *  Set icon type defaults
     */
    m_tmp_default_icon_data = QByteArray();
    m_tmp_default_icon_mime = QString();

    /*
     *  Signals and slots
     */
    connect( m_ui->chooseCustomDefaultIconButton, &QPushButton::clicked, this, &PreferencesDialog::slotDefaultFileSelect );
    connect( m_ui->chooseCustomButton, &QPushButton::clicked, this, &PreferencesDialog::slotFileSelect );
    connect( m_ui->buttonBox, &QDialogButtonBox::accepted, this, &PreferencesDialog::slotAccept );
    connect( m_ui->buttonBox, &QDialogButtonBox::rejected, this, &PreferencesDialog::slotReject );
    connect( m_ui->numberColorPushButton, &QPushButton::clicked, this, &PreferencesDialog::slotColorSelect );

    /*
     *  Set number color
     */
    setNumberColor( m_pref->getNumberColor() );

    /*
     *  Set number size
     */
    setNumberSize( m_pref->getNumberSize() );
}


/*
 *  Set the debug state
 */
void    PreferencesDialog::setDebug( bool state )
{
   m_ui->debugWindowCheckBox->setChecked( state );
}


/*
 *  Set the minimize type
 */
void    PreferencesDialog::setMinimizeType( Preferences::MinimizeType minimize_type )
{
   ( m_ui->minimizeTypeGroup->button( minimize_type ) )->setChecked( true );
}


/*
 *  Set the start minimized state
 */
void    PreferencesDialog::setStartMinimized( bool state )
{
   m_ui->startMinimizedCheckBox->setChecked( state );
}


/*
 *  Set the minimize on close state
 */
void    PreferencesDialog::setMinimizeOnClose( bool state )
{
   m_ui->minimizeOnCloseCheckBox->setChecked( state );
}


/*
 *  Set the icon type
 */
void    PreferencesDialog::setIconType( Preferences::IconType icon_type )
{
   ( m_ui->iconTypeGroup->button( icon_type ) )->setChecked( true );
}


/*
 *  Set the default icon type
 */
void    PreferencesDialog::setDefaultIconType( Preferences::DefaultIconType icon_type )
{
   ( m_ui->defaultIconTypeGroup->button( icon_type ) )->setChecked( true );
}


/*
 *  Set the icon
 */
void    PreferencesDialog::setIcon( const QString& icon_mime, const QByteArray& icon )
{
    /*
     *  Store the new icon
     */
    m_tmp_icon_mime = icon_mime;
    m_tmp_icon_data = icon;

    /*
     *  Display the new icon
     */
    setIcon();
}


/*
 *  Set the icon
 */
void    PreferencesDialog::setIcon()
{
    /*
     *  Convert data to pixmap
     */
    QPixmap  pixmap;
    pixmap.loadFromData( m_tmp_icon_data );

    /*
     *  Display the icon
     */
    m_ui->imageLabel->setPixmap( pixmap.scaledToHeight( m_ui->chooseCustomButton->size().height() ) );
}


/*
 *  Set the default icon
 */
void    PreferencesDialog::setDefaultIcon( const QString& icon_mime, const QByteArray& icon )
{
    /*
     *  Store the new icon
     */
    m_tmp_default_icon_mime = icon_mime;
    m_tmp_default_icon_data = icon;

    /*
     *  Display the new icon
     */
    setDefaultIcon();
}


/*
 *  Set the default icon
 */
void    PreferencesDialog::setDefaultIcon()
{
    /*
     *  Convert data to pixmap
     */
    QPixmap  pixmap;
    pixmap.loadFromData( m_tmp_default_icon_data );

    /*
     *  Display the icon
     */
    m_ui->defaultImageLabel->setPixmap( pixmap.scaledToHeight( m_ui->chooseCustomButton->size().height() ) );
}

/*
 *  Set the enable number state
 */
void    PreferencesDialog::setShowNumber( bool state )
{
   m_ui->showNumberCheckBox->setChecked( state );
}


/*
 *  Set the number color
 */
void    PreferencesDialog::setNumberColor( QString color )
{
    m_number_color = color;

    QPixmap pixmap( 256, 256 );
    pixmap.fill( QColor( color ) );

    m_ui->numberColorPushButton->setIcon( QIcon( pixmap ) );
}


/*
 *  Set the number size
 */
void    PreferencesDialog::setNumberSize( int size )
{
    m_ui->numberSizeSpinBox->setValue( size );
}


/*
 *  Set the count type
 */
void    PreferencesDialog::setCountType( Preferences::CountType count_type )
{
   ( m_ui->countTypeGroup->button( count_type ) )->setChecked( true );
}


/*
 *  Handle the accept signal
 */
void    PreferencesDialog::slotAccept()
{
    /*
     *  Settings changed by app
     */
    m_pref->setAppPrefChanged( true );

    /*
     *  Get all the selected values and store them in the preferences
     */
    m_pref->setDefaultIconType( static_cast< Preferences::DefaultIconType >( m_ui->defaultIconTypeGroup->checkedId() ) );
    m_pref->setDefaultIconMime( m_tmp_default_icon_mime );
    m_pref->setDefaultIconData( m_tmp_default_icon_data );

    m_pref->setIconType( static_cast< Preferences::IconType >( m_ui->iconTypeGroup->checkedId() ) );
    m_pref->setIconMime( m_tmp_icon_mime );
    m_pref->setIconData( m_tmp_icon_data );

    m_pref->setMinimizeType( static_cast< Preferences::MinimizeType >( m_ui->minimizeTypeGroup->checkedId() ) );
    m_pref->setStartMinimized( m_ui->startMinimizedCheckBox->isChecked() );
    m_pref->setMinimizeOnClose( m_ui->minimizeOnCloseCheckBox->isChecked() );

    m_pref->setShowNumber( m_ui->showNumberCheckBox->isChecked() );
    m_pref->setNumberColor( m_number_color );
    m_pref->setNumberSize( m_ui->numberSizeSpinBox->value() );
    m_pref->setCountType( static_cast< Preferences::CountType >( m_ui->countTypeGroup->checkedId() ) );

    m_pref->setDebug( m_ui->debugWindowCheckBox->isChecked() );

    /*
     *  Settings changed by app
     */
    m_pref->setAppPrefChanged( false );

    /*
     *  Close it
     */
//    QDialog::accept();
    hide();
}


/*
 *  Handle the accept signal
 */
void    PreferencesDialog::slotReject()
{
    /*
     *  Close it
     */
//    QDialog::reject();
    hide();
}


/*
 *  Handle the choose button
 */
void    PreferencesDialog::slotFileSelect()
{
    QFileDialog file_dialog( this, tr( "Open Image" ), "", tr( "Image Files (*.png *.jpg *.bmp)" ) );

    if( file_dialog.exec() )
    {
        QFile file( file_dialog.selectedFiles()[ 0 ] );
        file.open( QIODevice::ReadOnly );
        m_tmp_icon_data = file.readAll();
        file.close();

        QMimeType type = QMimeDatabase().mimeTypeForData( m_tmp_icon_data );
        m_tmp_icon_mime = type.name();

        /*
         *  Display the icon
         */
        setIcon();
    }
}

/*
 *  Handle the default choose button
 */
void    PreferencesDialog::slotDefaultFileSelect()
{
    QFileDialog file_dialog( this, tr( "Open Image" ), "", tr( "Image Files (*.png *.jpg *.bmp)" ) );

    if( file_dialog.exec() )
    {
        QFile file( file_dialog.selectedFiles()[ 0 ] );
        file.open( QIODevice::ReadOnly );
        m_tmp_default_icon_data = file.readAll();
        file.close();

        QMimeType type = QMimeDatabase().mimeTypeForData( m_tmp_default_icon_data );
        m_tmp_default_icon_mime = type.name();

        /*
         *  Display the default icon
         */
        setDefaultIcon();
    }
}


/*
 *  Handle the colro select button
 */
void    PreferencesDialog::slotColorSelect()
{
    QColor color( m_number_color );
    QColorDialog color_dialog( color );

    if( color_dialog.exec() )
    {
        setNumberColor( color_dialog.selectedColor().name() );
    }
}


/*
 *  Handle the debug change signal
 */
void    PreferencesDialog::slotDebugChange()
{
    setDebug( m_pref->getDebug() );
}


/*
 *  Handle the minimize type change signal
 */
void    PreferencesDialog::slotMinimizeTypeChange()
{
    setMinimizeType( m_pref->getMinimizeType() );
}


/*
 *  Handle the start minimized change signal
 */
void    PreferencesDialog::slotStartMinimizedChange()
{
    setStartMinimized( m_pref->getStartMinimized() );
}


/*
 *  Handle the minimize on close change signal
 */
void    PreferencesDialog::slotMinimizeOnCloseChange()
{
    setMinimizeOnClose( m_pref->getMinimizeOnClose() );
}


/*
 *  Handle the icon type change signal
 */
void    PreferencesDialog::slotIconTypeChange()
{
    setIconType( m_pref->getIconType() );
}


/*
 *  Handle the default icon type change signal
 */
void    PreferencesDialog::slotDefaultIconTypeChange()
{
    setDefaultIconType( m_pref->getDefaultIconType() );
}


/*
 *  Handle the icon data change signal
 */
void    PreferencesDialog::slotIconDataChange()
{
    m_tmp_icon_mime = m_pref->getIconMime();
    m_tmp_icon_data = m_pref->getIconData();

    /*
     *  Display the icon
     */
    setIcon();
}


/*
 *  Handle the default icon data change signal
 */
void    PreferencesDialog::slotDefaultIconDataChange()
{
    m_tmp_default_icon_mime = m_pref->getDefaultIconMime();
    m_tmp_default_icon_data = m_pref->getDefaultIconData();

    /*
     *  Display the icon
     */
    setDefaultIcon();
}


/*
 *  Handle the enable number state change
 */
void    PreferencesDialog::slotShowNumberChange()
{
    setShowNumber( m_pref->getShowNumber() );
}


/*
 *  Handle the number color change
 */
void    PreferencesDialog::slotNumberColorChange()
{
    setNumberColor( m_pref->getNumberColor() );
}


/*
 *  Handle the number size change
 */
void    PreferencesDialog::slotNumberSizeChange()
{
    setNumberSize( m_pref->getNumberSize() );
}


/*
 *  Handle the count type change signal
 */
void    PreferencesDialog::slotCountTypeChange()
{
    setCountType( m_pref->getCountType() );
}
