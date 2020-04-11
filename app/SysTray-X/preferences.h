#ifndef PREFERENCES_H
#define PREFERENCES_H

/*
 *	Local includes
 */

/*
 *	Qt includes
 */
#include <QObject>
#include <QString>
#include <QByteArray>

/**
 * @brief The Preferences class. Class to hold the preferences.
 */
class Preferences : public QObject
{
    Q_OBJECT

    public:

        /*
         *  Minimize types
         */
        enum MinimizeType {
            PREF_DEFAULT_MINIMIZE = 0,
            PREF_MINIMIZE_METHOD_1,
            PREF_MINIMIZE_METHOD_2
        };

        /*
         *  Icon types
         */
        enum IconType {
            PREF_BLANK_ICON = 0,
            PREF_NEWMAIL_ICON,
            PREF_CUSTOM_ICON
        };

    public:

        /**
         * @brief Preferences.  Constructor. Destructor.
         */
        Preferences( QObject *parent = nullptr );

        /**
         * @brief getAppPrefChanged. Control for sending changes to the add-on.
         *
         * @return      The state
         */
        bool getAppPrefChanged() const;

        /**
         * @brief setAppPrefChanged. Control for sending changes to the add-on.
         *
         *  @param state    The state
         */
        void setAppPrefChanged( bool state );

        /**
         * @brief getIconType. Get the icon type.
         *
         * @return      The icon type.
         */
        IconType getIconType() const;

        /**
         * @brief setIconType. Set the icon type.
         *
         * @param      The icon type.
         */
        void setIconType( IconType icon_type );

        /**
         * @brief getIconMime. Get the icon mime.
         *
         * @return      The icon mime.
         */
        const QString& getIconMime() const;

        /**
         * @brief setIconMime. Set the icon mime.
         *
         * @param      The icon mime.
         */
        void setIconMime( const QString& icon_mime );

        /**
         * @brief getIconData. Get the icon data.
         *
         * @return      The icon data.
         */
        const QByteArray& getIconData() const;

        /**
         * @brief setIconData. Set the icon data.
         *
         * @param      The icon data.
         */
        void setIconData( const QByteArray& icon_data );

        /**
         * @brief getShowNumber. Get the show number state.
         *
         * @return      The state.
         */
        bool getShowNumber() const;

        /**
         * @brief setShowNumber. Set the show number state.
         *
         * @param      The state.
         */
        void setShowNumber( bool state );

        /**
         * @brief getNumberColor. Get the number color.
         *
         * @return      The color.
         */
        QString getNumberColor() const;

        /**
         * @brief setNumberColor. Set the number color.
         *
         * @param      The color.
         */
        void setNumberColor( QString color );

        /**
         * @brief getMinimizeType. Get the minimize type.
         *
         * @return      The minimize type.
         */
        MinimizeType getMinimizeType() const;

        /**
         * @brief setMinimizeType. Set the minimize type.
         *
         * @param      The minimize type.
         */
        void setMinimizeType( MinimizeType icon_type );

        /**
         * @brief getStartMinimized. Get the start minimized state.
         *
         * @return      The state.
         */
        bool getStartMinimized() const;

        /**
         * @brief setStartMinimized. Set the start minimized state.
         *
         * @param      The state.
         */
        void setStartMinimized( bool state );

        /**
         * @brief getPollStartupDelay. Get the poll startup delay.
         *
         * @return      The poll startup delay.
         */
        int getPollStartupDelay() const;

        /**
         * @brief setPollStartupDelay. Set the poll startup delay.
         *
         * @param       The poll startup delay.
         */
        void setPollStartupDelay( int val );

        /**
         * @brief getPollInterval. Get the poll interval.
         *
         * @return      The poll interval.
         */
        int getPollInterval() const;

        /**
         * @brief setPollInterval. Set the poll interval.
         *
         * @param       The poll interval.
         */
        void setPollInterval( int val );

        /**
         * @brief getDebug. Get the debug windows state.
         *
         * @return      The state.
         */
        bool getDebug() const;

        /**
         * @brief setDebug. Set the debug windows state.
         *
         * @param      The state.
         */
        void setDebug( bool state );

        /**
         * @brief getVersion. Get the software version.
         *
         * @return      The version.
         */
        QString getVersion() const;

        /**
         * @brief getBuild. Get the git number of commits.
         *
         * @return      The number of commits.
         */
        QString getBuild() const;

        /**
         * @brief getHash. Get the git hash.
         *
         * @return      The hash.
         */
        QString getHash() const;

        /**
         * @brief getBuild. Get the git branch.
         *
         * @return      The branch.
         */
        QString getBranch() const;

    signals:

        /**
         * @brief signalConsole. Send a console message.
         *
         *  @param message      The message.
         */
        void    signalConsole( QString message );

        /**
         * @brief signalIconTypeChange. Signal a icon type change.
         */
        void signalIconTypeChange();

        /**
         * @brief signalIconDataChange. Signal a icon data change.
         */
        void signalIconDataChange();

        /**
         * @brief signalShowNumberChange. Signal a show number state change.
         */
        void signalShowNumberChange();

        /**
         * @brief signalNumberColorChange. Signal a number color change.
         */
        void signalNumberColorChange();

        /**
         * @brief signalMinimizeTypeChange. Signal a minimize type change.
         */
        void signalMinimizeTypeChange();

        /**
         * @brief signalStartMinimizedChange. Signal a start minimized state change.
         */
        void signalStartMinimizedChange();

        /**
         * @brief signalPollStartupDelayChange. Signal a poll startup delay change.
         */
        void signalPollStartupDelayChange();

        /**
         * @brief signalPollIntervalChange. Signal a poll interval change.
         */
        void signalPollIntervalChange();

        /**
         * @brief signalDebugChange. Signal a debug state change.
         */
        void signalDebugChange();

    private:

        /**
         * @brief m_app_pref_changed. Control for sending changes to the add-on.
         */
        bool m_app_pref_changed;

        /**
         * @brief m_icon_type. Selected icon type.
         */
        IconType m_icon_type;

        /**
         * @brief m_icon_mime. Selected icon mime.
         */
        QString m_icon_mime;

        /**
         * @brief m_icon_data. Binary data icon image.
         */
        QByteArray m_icon_data;

        /**
         * @brief m_show_number. Show number in systray icon.
         */
        bool m_show_number;

        /**
         * @brief m_number_color. The color of the number in systray icon.
         */
        QString m_number_color;

        /**
         * @brief m_minimize_type. Selected minimize type.
         */
        MinimizeType m_minimize_type;

        /**
         * @brief m_start_minimized. Start TB minimized.
         */
        bool m_start_minimized;

        /**
         * @brief m_poll_startup_delay. The startup poll delay.
         */
        int m_poll_startup_delay;

        /**
         * @brief m_poll_interval. The poll interval.
         */
        int m_poll_interval;

        /**
         * @brief m_debug. Display debug window.
         */
        bool m_debug;

        /**
         * @brief m_version_major. Major version number.
         */
        QString m_version_major;

        /**
         * @brief m_version_minor. Minor version number.
         */
        QString m_version_minor;

        /**
         * @brief m_version_patch. patch version number.
         */
        QString m_version_patch;

        /**
         * @brief m_version_build. Git commits count.
         */
        QString m_version_build;

        /**
         * @brief m_version_build. Git hash.
         */
        QString m_version_hash;

        /**
         * @brief m_version_build. Git branch.
         */
        QString m_version_branch;

};

#endif // PREFERENCES_H
