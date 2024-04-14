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
#include <QEvent>
#include <QKeyEvent>
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
     *  Store link and preferences
     */
    m_link = link;
    m_pref = pref;

    /*
     *  Always start with the first tab
     */
    m_ui->tabWidget->setCurrentIndex(0);

    /*
     *  Set close type button Ids
     */
    m_ui->closeTypeGroup->setId( m_ui->defaultCloseWindowsRadioButton, Preferences::PREF_DEFAULT_CLOSE_WINDOWS);
    m_ui->closeTypeGroup->setId( m_ui->minimizeMainTrayCloseChildrenWindowsRadioButton, Preferences::PREF_MINIMIZE_MAIN_TRAY_CLOSE_CHILDREN_WINDOWS );
    m_ui->closeTypeGroup->setId( m_ui->minimizeAllTrayWindowsRadioButton, Preferences::PREF_MINIMIZE_ALL_WINDOWS_TRAY );
    m_ui->closeTypeGroup->setId( m_ui->minimizeMainCloseChildrenWindowsRadioButton, Preferences::PREF_MINIMIZE_MAIN_CLOSE_CHILDREN_WINDOWS );
    m_ui->closeTypeGroup->setId( m_ui->minimizeAllWindowsRadioButton, Preferences::PREF_MINIMIZE_ALL_WINDOWS );

    /*
     *  Set minimize type button Ids
     */
    m_ui->minimizeTypeGroup->setId( m_ui->defaultMinimizeRadioButton, Preferences::PREF_DEFAULT_MINIMIZE);
    m_ui->minimizeTypeGroup->setId( m_ui->minimizeMethod1RadioButton, Preferences::PREF_MINIMIZE_METHOD_1 );
    m_ui->minimizeTypeGroup->setId( m_ui->minimizeMethod2RadioButton, Preferences::PREF_MINIMIZE_METHOD_2 );

    m_ui->minimizeMethod2RadioButton->hide();

    /*
     *  Set minimize icon type button Ids
     */
    m_ui->minimizeIconTypeGroup->setId( m_ui->defaultMinimizeIconRadioButton, Preferences::PREF_DEFAULT_MINIMIZE_ICON);
    m_ui->minimizeIconTypeGroup->setId( m_ui->minimizeTrayIconRadioButton, Preferences::PREF_MINIMIZE_TRAY_ICON );

    /*
     *  Set startup type button Ids
     */
    m_ui->startupTypeGroup->setId( m_ui->startDefaultRadioButton, Preferences::PREF_START_DEFAULT);
    m_ui->startupTypeGroup->setId( m_ui->startMinimizedRadioButton, Preferences::PREF_START_MINIMIZED);
    m_ui->startupTypeGroup->setId( m_ui->startDockedRadioButton, Preferences::PREF_START_DOCKED );

    /*
     *  Set position button Ids
     */
    m_ui->positionGroup->setId( m_ui->noTitlebarCorrectionRadioButton, Preferences::PREF_NO_CORRECTION);
    m_ui->positionGroup->setId( m_ui->addTitlebarCorrectionRadioButton, Preferences::PREF_ADD_CORRECTION );
    m_ui->positionGroup->setId( m_ui->subtractTitlebarCorrectionRadioButton, Preferences::PREF_SUBTRACT_CORRECTION );

#ifdef Q_OS_WIN

    m_ui->hideDefaultIconCheckBox->hide();
    m_ui->positionGroupBox->hide();
    m_ui->restorePositionscheckBox->hide();

#endif

#if defined( Q_OS_UNIX ) && defined( NO_KDE_INTEGRATION )

    m_ui->hideDefaultIconCheckBox->hide();

#endif

#if defined( Q_OS_UNIX ) && defined( NO_SHORTCUTS )

    hideShortcutsTab();

#else

    if( !m_pref->getShortcutsOption() )
    {
        hideShortcutsTab();
    }

#endif

    /*
     *  Set icon type button Ids
     */
    m_ui->iconTypeGroup->setId( m_ui->blankIconRadioButton, Preferences::PREF_BLANK_ICON );
    m_ui->iconTypeGroup->setId( m_ui->newMailIconRadioButton, Preferences::PREF_NEWMAIL_ICON );
    m_ui->iconTypeGroup->setId( m_ui->customIconRadioButton, Preferences::PREF_CUSTOM_ICON );
    m_ui->iconTypeGroup->setId( m_ui->noIconRadioButton, Preferences::PREF_NO_ICON );
    m_ui->iconTypeGroup->setId( m_ui->tbIconRadioButton, Preferences::PREF_TB_ICON );

    /*
     *  Set count type button Ids
     */
    m_ui->countTypeGroup->setId( m_ui->unreadRadioButton, Preferences::PREF_COUNT_UNREAD );
    m_ui->countTypeGroup->setId( m_ui->newRadioButton, Preferences::PREF_COUNT_NEW );

    /*
     *  Set icon type defaults
     */
    m_tmp_icon_data = QByteArray();
    m_tmp_icon_mime = QString();

    /*
     *  Set default icon type button Ids
     */
    m_ui->defaultIconTypeGroup->setId( m_ui->defaultIconRadioButton, Preferences::PREF_DEFAULT_ICON_DEFAULT );
    m_ui->defaultIconTypeGroup->setId( m_ui->lookThroughDefaultIconRadioButton, Preferences::PREF_DEFAULT_ICON_HIDE );
    m_ui->defaultIconTypeGroup->setId( m_ui->customDefaultIconRadioButton, Preferences::PREF_DEFAULT_ICON_CUSTOM );

    /*
     *  Set icon type defaults
     */
    m_tmp_default_icon_data = QByteArray();
    m_tmp_default_icon_mime = QString();

    /*
     *  Set new indicator button Ids
     */
    m_ui->newIndicatorTypeGroup->setId( m_ui->newIconRoundRadioButton, Preferences::PREF_NEW_INDICATOR_ROUND );
    m_ui->newIndicatorTypeGroup->setId( m_ui->newIconStarRadioButton, Preferences::PREF_NEW_INDICATOR_STAR );
    m_ui->newIndicatorTypeGroup->setId( m_ui->newShadeRadioButton, Preferences::PREF_NEW_INDICATOR_SHADE);

    /*
     *  Signals and slots
     */
    connect( m_ui->chooseCustomDefaultIconButton, &QPushButton::clicked, this, &PreferencesDialog::slotDefaultFileSelect );
    connect( m_ui->chooseCustomButton, &QPushButton::clicked, this, &PreferencesDialog::slotFileSelect );
    connect( m_ui->numberColorPushButton, &QPushButton::clicked, this, &PreferencesDialog::slotColorSelect );

    connect( m_ui->newShadeColorPushButton, &QPushButton::clicked, this, &PreferencesDialog::slotNewShadeColorSelect );

    connect( m_ui->savePushButton, &QPushButton::clicked, this, &PreferencesDialog::slotAccept);
    connect( m_ui->cancelPushButton, &QPushButton::clicked, this, &PreferencesDialog::slotReject);

    connect( m_ui->startBrowseButton, &QPushButton::clicked, this, &PreferencesDialog::slotStartAppSelect );
    connect( m_ui->closeBrowseButton, &QPushButton::clicked, this, &PreferencesDialog::slotCloseAppSelect );

    connect( m_ui->showHideKeySequenceEdit, &QKeySequenceEdit::editingFinished, this, &PreferencesDialog::slotTruncateShowHideShortcut );
    connect( m_ui->clearShowHideShortcutPushButton, &QPushButton::clicked, this, &PreferencesDialog::slotClearShowHideShortcut );

    /*
     *  Set number color
     */
    setNumberColor( m_pref->getNumberColor() );

    /*
     *  Set number size
     */
    setNumberSize( m_pref->getNumberSize() );

    /*
     *  Set startup delay
     */
    setStartupDelay( m_pref->getStartupDelay() );

    /*
     *  Set API count method
     */
    setApiCountMethod( m_pref->getApiCountMethod() );

    /*
     *  Set number alignment
     */
    setNumberAlignment( m_pref->getNumberAlignment() );

    /*
     *  Set number margins
     */
    setNumberMargins( m_pref->getNumberMargins() );

    /*
     *  Set new shade color
     */
    setNewShadeColor( m_pref->getNewShadeColor() );

    /*
     *  Set the start and close application parameters
     */
    setStartApp( m_pref->getStartApp() );
    setStartAppArgs( m_pref->getStartAppArgs() );
    setCloseApp( m_pref->getCloseApp() );
    setCloseAppArgs( m_pref->getCloseAppArgs() );

    /*
     *  Set the show / hide shortcut
     */
    setShowHideShortcut( m_pref->getShowHideShortcut() );

    /*
     *  Dialog on top
     */
/*
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
#ifdef Q_OS_UNIX
    flags |= Qt::X11BypassWindowManagerHint;
#endif
    setWindowFlags( flags );
*/
}


/*
 *  Hide the shortcuts tab
 */
void PreferencesDialog::hideShortcutsTab()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)

    int index = m_ui->tabWidget->indexOf( m_ui->tabShortcuts );
    m_ui->tabWidget->setTabVisible( index, false );

#else

    int index = m_ui->tabWidget->indexOf( m_ui->tabShortcuts );
    m_ui->tabWidget->setTabEnabled( index, false );
    m_ui->tabWidget->setStyleSheet( "QTabBar::tab::disabled { width: 0; height: 0; margin: 0; padding: 0; border: none; }" );

#endif
}


/*
 *  Handle the language change event
 */
void PreferencesDialog::changeEvent( QEvent *event )
{
    QDialog::changeEvent( event );

    if( QEvent::LanguageChange == event->type() )
    {
        m_ui->retranslateUi( this );
    }
}


/*
 *  Handle key event
 */
void PreferencesDialog::keyPressEvent( QKeyEvent *event )
{
    if( event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return )
    {
        slotAccept();

        return;
    }

    if( event->key() == Qt::Key_Escape )
    {
        slotReject();

        return;
    }

    QDialog::keyPressEvent( event );
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
 *  Set the minimize icon type
 */
void    PreferencesDialog::setMinimizeIconType( Preferences::MinimizeIconType minimize_icon_type )
{
   ( m_ui->minimizeIconTypeGroup->button( minimize_icon_type ) )->setChecked( true );
}


/*
 *  Set the positions correction state
 */
void    PreferencesDialog::setWindowPositionsCorrection( bool state )
{
    m_ui->correctWinPosCheckBox->setChecked( state );
}


/*
 *  Set the position correction
 */
void    PreferencesDialog::setWindowPositionsCorrectionType( Preferences::WindowPositionsCorrectionType position_correction )
{
    ( m_ui->positionGroup->button( position_correction ) )->setChecked( true );
}


/*
 *  Set the startup type
 */
void    PreferencesDialog::setStartupType(  Preferences::StartupType startup_type )
{
    ( m_ui->startupTypeGroup->button( startup_type ) )->setChecked( true );
}


/*
 *  Set the restore window positions state
 */
void    PreferencesDialog::setRestoreWindowPositions( bool state )
{
   m_ui->restorePositionscheckBox->setChecked( state );
}


/*
 *  Set the minimize on close state
 */
void    PreferencesDialog::setCloseType( Preferences::CloseType close_type )
{
    ( m_ui->closeTypeGroup->button( close_type ) )->setChecked( true );
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
 *  Set the hide default icon
 */
void    PreferencesDialog::setHideDefaultIcon( bool hide )
{
    m_ui->hideDefaultIconCheckBox->setChecked( hide );
}


/*
 *  Set the icon invert state
 */
void    PreferencesDialog::setInvertIcon( bool state )
{
   m_ui->invertIconCheckBox->setChecked( state );
}


/*
 *  Set the show number state
 */
void    PreferencesDialog::setShowNumber( bool state )
{
   m_ui->showNumberCheckBox->setChecked( state );
}


/*
 *  Set the show new indicator state
 */
void    PreferencesDialog::setShowNewIndicator( bool state )
{
   m_ui->showNewCheckBox->setChecked( state );
}


/*
 *  Set the startup delay
 */
void    PreferencesDialog::setStartupDelay( int delay )
{
    m_ui->startupDelaySpinBox->setValue( delay );
}


/*
 *  Set the count type
 */
void    PreferencesDialog::setCountType( Preferences::CountType count_type )
{
   ( m_ui->countTypeGroup->button( count_type ) )->setChecked( true );
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
 *  Set the number alignment
 */
void    PreferencesDialog::setNumberAlignment( int alignment )
{
   m_ui->numberAlignmentComboBox->setCurrentIndex( alignment );
}


/*
 *  Set the number margns
 */
void    PreferencesDialog::setNumberMargins( QMargins margins )
{
   m_ui->numberMarginLeftSpinBox->setValue( margins.left() );
   m_ui->numberMarginTopSpinBox->setValue( margins.top() );
   m_ui->numberMarginRightSpinBox->setValue( margins.right() );
   m_ui->numberMarginBottomSpinBox->setValue( margins.bottom() );
}


/*
 *  Set the new indicator type
 */
void    PreferencesDialog::setNewIndicatorType( Preferences::NewIndicatorType new_indicator_type )
{
   ( m_ui->newIndicatorTypeGroup->button( new_indicator_type ) )->setChecked( true );
}


/*
 *  Set the new shade color
 */
void    PreferencesDialog::setNewShadeColor( QString color )
{
    m_new_shade_color = color;

    QPixmap pixmap( 256, 256 );
    pixmap.fill( QColor( color ) );

    m_ui->newShadeColorPushButton->setIcon( QIcon( pixmap ) );
}


/*
 *  Set start application
 */
void    PreferencesDialog::setStartApp( QString app )
{
   m_ui->startAppLineEdit->setText( app );
}


/*
 *  Set start application arguments
 */
void    PreferencesDialog::setStartAppArgs( QString args )
{
   m_ui->startAppArgsLineEdit->setText( args );
}

/*
 *  Set close application
 */
void    PreferencesDialog::setCloseApp( QString app )
{
   m_ui->closeAppLineEdit->setText( app );
}


/*
 *  Set close application arguments
 */
void    PreferencesDialog::setCloseAppArgs( QString args )
{
   m_ui->closeAppArgsLineEdit->setText( args );
}


/*
 *  Set the API count method state
 */
void    PreferencesDialog::setApiCountMethod( bool state )
{
   m_ui->apiCountMethod->setChecked( state );
}


/*
 *  Set the show / hide shortcut
 */
void    PreferencesDialog::setShowHideShortcut( QKeySequence key_seq )
{
    m_ui->currentShowHideShortcut->setText( key_seq.toString() );

    m_ui->showHideKeySequenceEdit->setKeySequence( key_seq );
}


/*
 *  Handle show dialog signal
 */
void PreferencesDialog::slotShowDialog()
{
    QString version = m_pref->getBrowserVersion().split(".")[0];
    if( version.toInt() < 115 )
    {
        m_ui->specialOptionsGroupBox->setVisible( false );
    }
    else
    {
        m_ui->specialOptionsGroupBox->setVisible( true );
    }

    showNormal();
    activateWindow();
}


/*
 *  Handle the accept signal
 */
void    PreferencesDialog::slotAccept()
{
    /*
     *  Get all the selected values and store them in the preferences
     */
    m_pref->setDefaultIconType( static_cast< Preferences::DefaultIconType >( m_ui->defaultIconTypeGroup->checkedId() ) );
    m_pref->setDefaultIconMime( m_tmp_default_icon_mime );
    m_pref->setDefaultIconData( m_tmp_default_icon_data );
    m_pref->setHideDefaultIcon( m_ui->hideDefaultIconCheckBox->isChecked() );

    m_pref->setIconType( static_cast< Preferences::IconType >( m_ui->iconTypeGroup->checkedId() ) );
    m_pref->setIconMime( m_tmp_icon_mime );
    m_pref->setIconData( m_tmp_icon_data );

    m_pref->setMinimizeType( static_cast< Preferences::MinimizeType >( m_ui->minimizeTypeGroup->checkedId() ) );
    m_pref->setMinimizeIconType( static_cast< Preferences::MinimizeIconType >( m_ui->minimizeIconTypeGroup->checkedId() ) );

    m_pref->setWindowPositionsCorrection( m_ui->correctWinPosCheckBox->isChecked() );
    m_pref->setWindowPositionsCorrectionType( static_cast< Preferences::WindowPositionsCorrectionType >( m_ui->positionGroup->checkedId() ) );

    m_pref->setStartupType( static_cast< Preferences::StartupType >( m_ui->startupTypeGroup->checkedId() ) );
    m_pref->setRestoreWindowPositions( m_ui->restorePositionscheckBox->isChecked() );
    m_pref->setCloseType( static_cast< Preferences::CloseType >( m_ui->closeTypeGroup->checkedId() ) );
    m_pref->setInvertIcon( m_ui->invertIconCheckBox->isChecked() );

    m_pref->setShowNumber( m_ui->showNumberCheckBox->isChecked() );
    m_pref->setShowNewIndicator( m_ui->showNewCheckBox->isChecked() );
    m_pref->setStartupDelay( m_ui->startupDelaySpinBox->value() );
    m_pref->setApiCountMethod( m_ui->apiCountMethod->isChecked() );
    m_pref->setCountType( static_cast< Preferences::CountType >( m_ui->countTypeGroup->checkedId() ) );
    m_pref->setNumberSize( m_ui->numberSizeSpinBox->value() );

    m_pref->setNumberAlignment( m_ui->numberAlignmentComboBox->currentIndex() );
    m_pref->setNumberMargins( QMargins( m_ui->numberMarginLeftSpinBox->value(),  m_ui->numberMarginTopSpinBox->value(),
                   m_ui->numberMarginRightSpinBox->value(),  m_ui->numberMarginBottomSpinBox->value() ) );
    m_pref->setNewIndicatorType( static_cast< Preferences::NewIndicatorType >( m_ui->newIndicatorTypeGroup->checkedId() ) );
    m_pref->setNewShadeColor( m_new_shade_color );

    QString startApp = m_ui->startAppLineEdit->text();
    m_pref->setStartApp( startApp );
    QString startAppArgs = m_ui->startAppArgsLineEdit->text();
    m_pref->setStartAppArgs( startAppArgs );
    QString closeApp = m_ui->closeAppLineEdit->text();
    m_pref->setCloseApp( closeApp );
    QString closeAppArgs = m_ui->closeAppArgsLineEdit->text();
    m_pref->setCloseAppArgs( closeAppArgs );
    m_pref->setNumberColor( m_number_color );

    m_pref->setShowHideShortcut( m_ui->showHideKeySequenceEdit->keySequence() );

    m_pref->setDebug( m_ui->debugWindowCheckBox->isChecked() );

    /*
     *  Settings changed by app
     */
    emit signalPreferencesChanged();

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

    /*
     *  Reset all parameters
     */
    setDefaultIconType( m_pref->getDefaultIconType() );
    slotDefaultIconDataChange();
    setHideDefaultIcon( m_pref->getHideDefaultIcon() );

    setIconType( m_pref->getIconType() );
    slotIconDataChange();

    setMinimizeType( m_pref->getMinimizeType() );
    setMinimizeIconType( m_pref->getMinimizeIconType() );
    setWindowPositionsCorrection( m_pref->getWindowPositionsCorrection() );
    setWindowPositionsCorrectionType( m_pref->getWindowPositionsCorrectionType() );
    setStartupType( m_pref->getStartupType() );
    setRestoreWindowPositions( m_pref->getRestoreWindowPositions() );
    setCloseType( m_pref->getCloseType() );
    setInvertIcon( m_pref->getInvertIcon() );

    setShowNumber( m_pref->getShowNumber() );
    setShowNewIndicator( m_pref->getShowNewIndicator() );
    setStartupDelay( m_pref->getStartupDelay() );
    setApiCountMethod( m_pref->getApiCountMethod() );
    setCountType( m_pref->getCountType() );
    setNumberColor( m_pref->getNumberColor() );
    setNumberSize( m_pref->getNumberSize());
    setNumberAlignment( m_pref->getNumberAlignment() );
    setNumberMargins( m_pref->getNumberMargins() );
    setNewIndicatorType( m_pref->getNewIndicatorType() );
    setNewShadeColor( m_pref->getNewShadeColor() );

    setStartApp( m_pref->getStartApp() );
    setStartAppArgs( m_pref->getStartAppArgs() );
    setCloseApp( m_pref->getCloseApp() );
    setCloseAppArgs( m_pref->getCloseAppArgs() );

    setShowHideShortcut( m_pref->getShowHideShortcut() );

    setDebug( m_pref->getDebug());
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
 *  Handle the color select button
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
 *  Handle the new shade color select button
 */
void    PreferencesDialog::slotNewShadeColorSelect()
{
    QColor color( m_new_shade_color );
    QColorDialog color_dialog( color );

    if( color_dialog.exec() )
    {
        setNewShadeColor( color_dialog.selectedColor().name() );
    }
}


/*
 *  Handle the start application button
 */
void    PreferencesDialog::slotStartAppSelect()
{
    QFileDialog file_dialog( this, tr( "Select application" ), "", "" );

    if( file_dialog.exec() )
    {
        setStartApp( file_dialog.selectedFiles()[ 0 ] );
    }
}


/*
 *  Handle the close application button
 */
void    PreferencesDialog::slotCloseAppSelect()
{
    QFileDialog file_dialog( this, tr( "Select application" ), "", "" );

    if( file_dialog.exec() )
    {
        setCloseApp( file_dialog.selectedFiles()[ 0 ] );
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
 *  Handle the startup type change signal
 */
void    PreferencesDialog::slotStartupTypeChange()
{
    setStartupType( m_pref->getStartupType() );
}


/*
 *  Handle the restore window positions change signal
 */
void    PreferencesDialog::slotRestoreWindowPositionsChange()
{
    setRestoreWindowPositions( m_pref->getRestoreWindowPositions() );
}


/*
 *  Handle the minimize type change signal
 */
void    PreferencesDialog::slotMinimizeTypeChange()
{
    setMinimizeType( m_pref->getMinimizeType() );
}


/*
 *  Handle the minimize icon type change signal
 */
void    PreferencesDialog::slotMinimizeIconTypeChange()
{
    setMinimizeIconType( m_pref->getMinimizeIconType() );
}


/*
 *  Handle the minimize on close change signal
 */
void    PreferencesDialog::slotCloseTypeChange()
{
    setCloseType( m_pref->getCloseType() );
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
 *  Handle the hide default icon change signal
 */
void    PreferencesDialog::slotHideDefaultIconChange()
{
    setHideDefaultIcon( m_pref->getHideDefaultIcon() );
}


/*
 *  Handle the invert icon change signal
 */
void    PreferencesDialog::slotInvertIconChange()
{
    setInvertIcon( m_pref->getInvertIcon() );
}


/*
 *  Handle the show number state change
 */
void    PreferencesDialog::slotShowNumberChange()
{
    setShowNumber( m_pref->getShowNumber() );
}


/*
 *  Handle the show new indicator state change
 */
void    PreferencesDialog::slotShowNewIndicatorChange()
{
    setShowNewIndicator( m_pref->getShowNewIndicator() );
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


/*
 *  Handle the startup delay change
 */
void    PreferencesDialog::slotStartupDelayChange()
{
    setStartupDelay( m_pref->getStartupDelay() );
}


/*
 *  Handle the API count method change signal
 */
void    PreferencesDialog::slotApiCountMethodChange()
{
    setApiCountMethod( m_pref->getApiCountMethod() );
}


/*
 *  Handle the number alignment change
 */
void    PreferencesDialog::slotNumberAlignmentChange()
{
    setNumberAlignment( m_pref->getNumberAlignment() );
}


/*
 *  Handle the number margins change
 */
void    PreferencesDialog::slotNumberMarginsChange()
{
    setNumberMargins( m_pref->getNumberMargins() );
}


/*
 *  Handle the new indicator type change
 */
void    PreferencesDialog::slotNewIndicatorTypeChange()
{
    setNewIndicatorType( m_pref->getNewIndicatorType() );
}


/*
 *  Handle the new shade color change
 */
void    PreferencesDialog::slotNewShadeColorChange()
{
    setNewShadeColor( m_pref->getNewShadeColor() );
}


/*
 *  Handle the start application change signal
 */
void    PreferencesDialog::slotStartAppChange()
{
    setStartApp( m_pref->getStartApp() );
}


/*
 *  Handle the start application arguments change signal
 */
void    PreferencesDialog::slotStartAppArgsChange()
{
    setStartAppArgs( m_pref->getStartAppArgs() );
}


/*
 *  Handle the close application change signal
 */
void    PreferencesDialog::slotCloseAppChange()
{
    setCloseApp( m_pref->getCloseApp() );
}


/*
 *  Handle the close application arguments change signal
 */
void    PreferencesDialog::slotCloseAppArgsChange()
{
    setCloseAppArgs( m_pref->getCloseAppArgs() );
}


/*
 *  Handle the show / hide shortcut change signal
 */
void    PreferencesDialog::slotShowHideShortcutChange()
{
    setShowHideShortcut( m_pref->getShowHideShortcut() );
}


/*
 *  Handle the edit show / hide shortcut key field
 */
void    PreferencesDialog::slotTruncateShowHideShortcut()
{
    QKeySequence key_seq = m_ui->showHideKeySequenceEdit->keySequence();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    /*
     *  Workaround for a Qt5 bug in QKeySequenceEdit object, Meta/Win key is not handled correctly.
     */
    int value = 0;
    if( key_seq[ 0 ] == 16777299 && key_seq.count() > 1 )
    {
        value = key_seq[ 1 ];
    }
    else if( !(key_seq[ 0 ] == 16777299 && key_seq.count() == 1) )
    {
        value = key_seq[ 0 ];
    }

    QKeySequence shortcut( value );

#else

    QKeySequence shortcut( key_seq[ 0 ] );

#endif

    m_ui->showHideKeySequenceEdit->setKeySequence( shortcut );
}


/*
 *  Handle the clear show / hide shortcut push button
 */
void    PreferencesDialog::slotClearShowHideShortcut()
{
    m_ui->showHideKeySequenceEdit->setKeySequence( QKeySequence() );
}
