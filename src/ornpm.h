#pragma once

#include <QObject>

#include <Transaction>

#include <utility>

class QQmlEngine;
class QJSEngine;
class OrnPackageVersion;
class OrnInstalledPackage;
class OrnRepo;

class OrnPmPrivate;

class OrnPm : public QObject
{
    friend class OrnBackup;

    Q_OBJECT

    Q_PROPERTY(bool initialised READ initialised NOTIFY initialisedChanged)
    Q_PROPERTY(QVariantList operations READ operations NOTIFY operationsChanged)
    Q_PROPERTY(bool updatesAvailable READ updatesAvailable NOTIFY updatablePackagesChanged)

public:

    enum Operation
    {
        NoOperations,
        AddingRepo,
        RemovingRepo,
        EnablingRepo,
        DisablingRepo,
        RefreshingRepo,
        InstallingPackage,
        RemovingPackage,
        UpdatingPackage,
        RefreshingCache
    };
    Q_ENUM(Operation)

    // ssu actions
    enum RepoAction
    {
        RemoveRepo  = 0,
        AddRepo     = 1,
        DisableRepo = 2,
        EnableRepo  = 3
    };
    Q_ENUM(RepoAction)

    enum RepoStatus
    {
        RepoUnknownStatus,
        RepoNotInstalled,
        RepoDisabled,
        RepoEnabled
    };
    Q_ENUM(RepoStatus)

    enum PackageStatus
    {
        PackageUnknownStatus,
        PackageNotInstalled,
        PackageAvailable,
        PackageInstalled,
        PackageUpdateAvailable,
        PackageInstalling,
        PackageRemoving,
        PackageUpdating
    };
    Q_ENUM(PackageStatus)

    enum ErrorCode
    {
        ErrorUnknown = PackageKit::Transaction::ErrorUnknown,
        ErrorPackageNotFound = PackageKit::Transaction::ErrorPackageNotFound,
        ErrorDepResolutionFailed = PackageKit::Transaction::ErrorDepResolutionFailed
    };
    Q_ENUM(ErrorCode)

    static const QLatin1String repoNamePrefix;

    static QString repoUrl(const QString &author);

    static OrnPm *instance();
    static inline QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
    {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return OrnPm::instance();
    }

    bool initialised() const;
    QVariantList operations() const;

    bool updatesAvailable() const;
    Q_INVOKABLE QStringList updatablePackages() const;
    QString updateVersion(const QString &packageName) const;

    RepoStatus repoStatus(const QString &alias) const;
    PackageStatus packageStatus(const QString &packageName) const;

signals:
    void initialisedChanged();
    void operationsChanged();
    void packageStatusChanged(const QString &packageName, OrnPm::PackageStatus status);
    void error(quint32 code, const QString &details);

#ifdef QT_DEBUG
private slots:
    void onTransactionFinished(quint32 exit, quint32 runtime);
    void emitError(quint32 code, const QString& details);
#endif

    // Check for updates
signals:
    void updatablePackagesChanged();

    // Package versions
signals:
    void packageVersions(const QString &packageName, const QList<OrnPackageVersion> &versions);
public slots:
    void getPackageVersions(const QString &packageName);

    // Install package
signals:
    void packageInstalled(const QString &packageName);
public slots:
    void installPackage(const QString &packageId);
    void installFile(const QString &packageFile);

    // Remove package
signals:
    void packageRemoved(const QString &packageName);
public slots:
    void removePackage(const QString &packageId, bool autoremove = false);

    // Update package
signals:
    void packageUpdated(const QString &packageName);
public slots:
    void updatePackage(const QString &packageName);

    // SSU repo actions
signals:
    void repoModified(const QString &alias, OrnPm::RepoAction action);
    void enableReposFinished();
    void removeAllReposFinished();
public slots:
    void addRepo(const QString &author);
    void modifyRepo(const QString &alias, OrnPm::RepoAction action);
    void enableRepos(bool enable);
    void removeAllRepos();

    // Refresh repos
public slots:
    void refreshRepo(const QString &alias, bool force = false);
    void refreshRepos(bool force = false);
    void refreshCache(bool force = false);

    // Get ORN repositories
public:
    QList<OrnRepo> repoList() const;

    // Get installed packages
signals:
    void installedPackages(const QList<OrnInstalledPackage> &packages);
public:
    void getInstalledPackages(const QString &packageName = QString());

private:
    explicit OrnPm(QObject *parent = nullptr);

    Q_DECLARE_PRIVATE(OrnPm)

    template<
        typename Sender,
        typename ...Args,
        typename Signal = void (Sender::*)(Args...),
        typename Slot   = void (OrnPmPrivate::*)(Args...)
    >
    friend void connect_priv(Sender *sender, Signal signal, OrnPm *ptr, Slot slot)
    {
        QObject::connect(sender, signal, ptr, [ptr, slot](auto && ...args) {
            (ptr->d_func()->*slot)(std::forward<decltype(args)>(args)...);
        });
    }
};
