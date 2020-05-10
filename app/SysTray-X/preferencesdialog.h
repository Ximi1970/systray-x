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

        /**
         * @brief setCountType. Set the count type.
         *
         *  @param count_type    The count type.
         */
        void    setCountType( Preferences::CountType count_type );

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
         * @brief slotMinimizeTypeChange. Slot for handling minimize type change signals.
         */
        void    slotMinimizeTypeChange();

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

        /**
         * @brief slotCountTypeChange. Slot for handling count type change.
         */
        void    slotCountTypeChange();

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
