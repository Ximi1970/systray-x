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

    private:

        /**
         * @brief setDebug. Set the debug state.
         *
         *  @param state    The state.
         */
        void    setDebug( bool state );

        /**
         * @brief setPollStartupDelay. Set the poll startup delay.
         *
         *  @param val    The new value.
         */
        void    setPollStartupDelay( int val );

        /**
         * @brief setPollInterval. Set the poll interval.
         *
         *  @param val    The new value.
         */
        void    setPollInterval( int val );

        /**
         * @brief setHideOnMinimize. Set the hide on minimize state.
         *
         *  @param state    The state.
         */
        void    setHideOnMinimize( bool state );

        /**
         * @brief setStartMinimized. Set the start minimized state.
         *
         *  @param state    The state.
         */
        void    setStartMinimized( bool state );

        /**
         * @brief setIconType. Set the icon type.
         *
         *  @param icon_type    The icon type.
         */
        void    setIconType( Preferences::IconType icon_type );

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
         * @brief setShowNumber. Set the show number state.
         *
         *  @param state    The state.
         */
        void    setShowNumber( bool state );

        /**
         * @brief setNumberColor. Set the number color.
         *
         *  @param state    The state.
         */
        void    setNumberColor( QString color );

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
         * @brief slotPollStartupDelayChange. Slot for handling poll startup delay signals.
         */
        void    slotPollStartupDelayChange();

        /**
         * @brief slotPollIntervalChange. Slot for handling poll interval signals.
         */
        void    slotPollIntervalChange();

        /**
         * @brief slotHideOnMinimizeChange. Slot for handling hide on minimize change signals.
         */
        void    slotHideOnMinimizeChange();

        /**
         * @brief slotStartMinimizedChange. Slot for handling start minimized change signals.
         */
        void    slotStartMinimizedChange();

        /**
         * @brief slotIconTypeChange. Slot for handling icon type change signals.
         */
        void    slotIconTypeChange();

        /**
         * @brief slotIconDataChange. Slot for handling icon data change signals.
         */
        void    slotIconDataChange();

        /**
         * @brief slotShowNumberChange. Slot for handling show number state change.
         */
        void    slotShowNumberChange();

        /**
         * @brief slotNumberColorChange. Slot for handling number color change.
         */
        void    slotNumberColorChange();

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
         * @brief m_number_color. Temporary storage for the number color.
         */
        QString m_number_color;
};

#endif // PREFERENCESDIALOG_H
