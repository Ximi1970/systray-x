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
         * @brief setHideOnMinimize. Set the hide on minimize state.
         *
         *  @param state    The state.
         */
        void    setHideOnMinimize( bool state );

        /**
         * @brief setMinimizeOnClose. Set the minimize on close state.
         *
         *  @param state    The state.
         */
        void    setMinimizeOnClose( bool state );

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

    signals:

        /**
         * @brief signalDebugMessage. Signal a debug message.
         *
         * @param message   The message.
         */
        void    signalDebugMessage( QString message );

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
         * @brief slotHideOnMinimizeChange. Slot for handling hide on minimize change signals.
         */
        void    slotHideOnMinimizeChange();

        /**
         * @brief slotMinimizeOnCloseChange. Slot for handling minimize on close change signals.
         */
        void    slotMinimizeOnCloseChange();

        /**
         * @brief slotIconTypeChange. Slot for handling icon type change signals.
         */
        void    slotIconTypeChange();

        /**
         * @brief slotIconDataChange. Slot for handling icon data change signals.
         */
        void    slotIconDataChange();

    private slots:

        /**
         * @brief slotAccept. Store the preferences on the accept signal.
         */
        void    slotAccept();

        /**
         * @brief slotFileSelect. Handle the choose custom button click.
         */
        void    slotFileSelect();

    private:

        /**
         * @brief m_ui. Pointer to the dialog.
         */
        Ui::PreferencesDialog*  m_ui;

        /**
         * @brief m_link. Pointer to the link.
         */
        SysTrayXLink*   m_link;

        /**
         * @brief m_pref. Pointer to the preferences storage.
         */
        Preferences*    m_pref;

        /**
         * @brief m_tmp_icon_mime. Temporary storage for icon mime.
         */
        QString     m_tmp_icon_mime;

        /**
         * @brief m_tmp_icon_data. Temporary storage for icon data.
         */
        QByteArray  m_tmp_icon_data;

};

#endif // PREFERENCESDIALOG_H
