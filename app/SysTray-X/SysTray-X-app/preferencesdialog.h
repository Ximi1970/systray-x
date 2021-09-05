#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

/*
 *	Local includes
 */
#include "preferences.h"

/*
 *	Qt includes
 */
#include <QDialog>
#include <QByteArray>

/*
 *	Predefines
 */
class SysTrayXLink;

/*
 *  Namespace
 */
namespace Ui {
    class PreferencesDialog;
}


/**
 * @brief The PreferencesDialog class. Handles the preferences.
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT

    public:

        /**
         * @brief PreferencesDialog
         */
        PreferencesDialog( SysTrayXLink *link, Preferences *pref, QWidget *parent = nullptr );

        /**
         * @brief changeEvent. Catch the event.
         *
         *  @param event    The event
         */
        void    changeEvent(QEvent* event);

    private:

        /**
         * @brief setDebug. Set the debug state.
         *
         *  @param state    The state.
         */
        void    setDebug( bool state );

        /**
         * @brief setMinimizeType. Set the minimize type.
         *
         *  @param minimize_type    The minimize type.
         */
        void    setMinimizeType( Preferences::MinimizeType minimize_type );

        /**
         * @brief setStartMinimized. Set the start minimized state.
         *
         *  @param state    The state.
         */
        void    setStartMinimized( bool state );

        /**
         * @brief setRestoreWindowPositions. Set the restore window positions state.
         *
         *  @param state    The state.
         */
        void    setRestoreWindowPositions( bool state );

        /**
         * @brief setCloseType. Set the close type.
         *
         *  @param state    The close type.
         */
        void    setCloseType( Preferences::CloseType close_type );

        /**
         * @brief setIconType. Set the icon type.
         *
         *  @param icon_type    The icon type.
         */
        void    setIconType( Preferences::IconType icon_type );

        /**
         * @brief setDefaultIconType. Set the default icon type.
         *
         *  @param icon_type    The icon type.
         */
        void    setDefaultIconType( Preferences::DefaultIconType icon_type );

        /**
         * @brief setIcon. Set the icon.
         *
         *  @param icon     The icon mime.
         *  @param icon     The icon data.
         */
        void    setIcon( const QString& icon_mime, const QByteArray& icon );

        /**
         * @brief setIcon. Set the icon.
         */
        void    setIcon();

        /**
         * @brief setDefaultIcon. Set the default icon.
         *
         *  @param icon     The icon mime.
         *  @param icon     The icon data.
         */
        void    setDefaultIcon( const QString& icon_mime, const QByteArray& icon );

        /**
         * @brief setDefaultIcon. Set the default icon.
         */
        void    setDefaultIcon();

        /**
         * @brief setHideDefaultIcon. Set the hide default icon.
         *
         *  @param hide     The state.
         */
        void    setHideDefaultIcon( bool hide );

        /**
         * @brief setShowNumber. Set the show number state.
         *
         *  @param state    The state.
         */
        void    setShowNumber( bool state );

        /**
         * @brief setNumberColor. Set the number color.
         *
         *  @param color    The color.
         */
        void    setNumberColor( QString color );

        /**
         * @brief setNumberSize. Set the number size.
         *
         *  @param size    The size.
         */
        void    setNumberSize( int size );

        /**
         * @brief setNumberAlignment. Set the number alignment.
         *
         *  @param alignment    The alignment.
         */
        void    setNumberAlignment( int alignment );

        /**
         * @brief setNumberMargins. Set the number margins.
         *
         *  @param margins    The margins.
         */
        void    setNumberMargins( QMargins margins );

        /**
         * @brief getNumberMargins. Get the number margins.
         *
         * @return The margins.
         */
        QMargins    getNumberMargins() const;

        /**
         * @brief setCountType. Set the count type.
         *
         *  @param count_type    The count type.
         */
        void    setCountType( Preferences::CountType count_type );

        /**
         * @brief setStartupDelay. Set the startup delay.
         *
         *  @param delay    The delay.
         */
        void    setStartupDelay( int size );

        /**
         * @brief setTheme. Set the theme.
         *
         * @param theme     The theme.
         */
        void    setTheme( Preferences::Theme theme );

    signals:

        /**
         * @brief signalConsole. Send a console message.
         *
         *  @param message      The message.
         */
        void    signalConsole( QString message );

        /**
         * @brief signalUpdateSysTray. Signal to update the system tray icon.
         */
        void    signalUpdateSysTrayIcon();

    public slots:

        /**
         * @brief slotDebugChange. Slot for handling debug change signals.
         */
        void    slotDebugChange();

        /**
         * @brief slotBrowserVersion. Slot for handling the browser version signals.
         */
        void    slotBrowserVersion();

        /**
         * @brief slotMinimizeTypeChange. Slot for handling minimize type change signals.
         */
        void    slotMinimizeTypeChange();

        /**
         * @brief slotStartMinimizedChange. Slot for handling start minimized change signals.
         */
        void    slotStartMinimizedChange();

        /**
         * @brief slotRestoreWindowPositionsChange. Slot for handling restore window positions change signals.
         */
        void    slotRestoreWindowPositionsChange();

        /**
         * @brief slotCloseTypeChange. Slot for handling close type change signals.
         */
        void    slotCloseTypeChange();

        /**
         * @brief slotIconTypeChange. Slot for handling icon type change signals.
         */
        void    slotIconTypeChange();

        /**
         * @brief slotDefaultIconTypeChange. Slot for handling default icon type change signals.
         */
        void    slotDefaultIconTypeChange();

        /**
         * @brief slotHideDefaultIconChange. Slot for handling hide default icon change signals.
         */
        void    slotHideDefaultIconChange();

        /**
         * @brief slotIconDataChange. Slot for handling icon data change signals.
         */
        void    slotIconDataChange();

        /**
         * @brief slotDefaultIconDataChange. Slot for handling default icon data change signals.
         */
        void    slotDefaultIconDataChange();

        /**
         * @brief slotShowNumberChange. Slot for handling show number state change.
         */
        void    slotShowNumberChange();

        /**
         * @brief slotNumberColorChange. Slot for handling number color change.
         */
        void    slotNumberColorChange();

        /**
         * @brief slotNumberSizeChange. Slot for handling number size change.
         */
        void    slotNumberSizeChange();

        /**
         * @brief slotNumberAlignmentChange. Slot for handling number alignment change.
         */
        void    slotNumberAlignmentChange();

        /**
         * @brief slotNumberMarginsChange. Slot for handling number margins change.
         */
        void    slotNumberMarginsChange();

        /**
         * @brief slotCountTypeChange. Slot for handling count type change.
         */
        void    slotCountTypeChange();

        /**
         * @brief slotStartupDelayChange. Slot for handling startup delay change.
         */
        void    slotStartupDelayChange();

        /**
         * @brief slotThemeChange. Slot for handling theme change.
         */
        void    slotThemeChange();

    private slots:

        /**
         * @brief slotAccept. Store the preferences on the accept signal.
         */
        void    slotAccept();

        /**
         * @brief slotReject. Store the preferences on the accept signal.
         */
        void    slotReject();

        /**
         * @brief slotFileSelect. Handle the choose custom button click.
         */
        void    slotFileSelect();

        /**
         * @brief slotDefaultFileSelect. Handle the choose default custom button click.
         */
        void    slotDefaultFileSelect();

        /**
         * @brief slotColorSelect. Handle the choose color button click.
         */
        void    slotColorSelect();

    private:

        /**
         * @brief m_ui. Pointer to the dialog.
         */
        Ui::PreferencesDialog *m_ui;

        /**
         * @brief m_link. Pointer to the link.
         */
        SysTrayXLink *m_link;

        /**
         * @brief m_pref. Pointer to the preferences storage.
         */
        Preferences *m_pref;

        /**
         * @brief m_tmp_icon_mime. Temporary storage for icon mime.
         */
        QString m_tmp_icon_mime;

        /**
         * @brief m_tmp_icon_data. Temporary storage for icon data.
         */
        QByteArray m_tmp_icon_data;

        /**
         * @brief m_tmp_default_icon_mime. Temporary storage for default icon mime.
         */
        QString m_tmp_default_icon_mime;

        /**
         * @brief m_tmp_default_icon_data. Temporary storage for default icon data.
         */
        QByteArray m_tmp_default_icon_data;

        /**
         * @brief m_number_color. Temporary storage for the number color.
         */
        QString m_number_color;
};

#endif // PREFERENCESDIALOG_H
