#ifndef PATHCONFIG_H
#define PATHCONFIG_H

#include <QString>
#include <QProcess>
#include <QFile>
#include <QDebug>

class PathConfig
{
public:
    static QString qpdfPath;
    static QString pdfjamPath;
    
    static void initialize()
    {
        // Find qpdf
        qpdfPath = findExecutable("qpdf");
        qDebug() << "Using qpdf path:" << qpdfPath;
        
        // Find pdfjam
        pdfjamPath = findExecutable("pdfjam");
        qDebug() << "Using pdfjam path:" << pdfjamPath;
    }
    
    static bool checkDependencies(QString &missingDeps)
    {
        bool allPresent = true;
        
        // Check qpdf
        if (qpdfPath.isEmpty()) {
            allPresent = false;
            missingDeps += "- qpdf\n";
        } else {
            // Test if it works
            QProcess testProcess;
            testProcess.start(qpdfPath, QStringList() << "--version");
            if (!testProcess.waitForFinished() || testProcess.exitCode() != 0) {
                allPresent = false;
                missingDeps += "- qpdf (installed but not working)\n";
            }
        }
        
        // Check pdfjam
        if (pdfjamPath.isEmpty()) {
            allPresent = false;
            missingDeps += "- pdfjam (part of texlive)\n";
        } else {
            // Test if it works
            QProcess testProcess;
            testProcess.start(pdfjamPath, QStringList() << "--version");
            if (!testProcess.waitForFinished() || testProcess.exitCode() != 0) {
                allPresent = false;
                missingDeps += "- pdfjam (installed but not working)\n";
            }
        }
        
        return allPresent;
    }
    
private:
    static QString findExecutable(const QString &name)
    {
        // Try using 'which' command
        QProcess whichProcess;
        whichProcess.start("which", QStringList() << name);
        if (whichProcess.waitForFinished() && whichProcess.exitCode() == 0) {
            return QString(whichProcess.readAllStandardOutput()).trimmed();
        }
        
        // Try common locations
        QStringList commonPaths = {
            "/opt/homebrew/bin/" + name,
            "/usr/local/bin/" + name,
            "/usr/bin/" + name
        };
        
        for (const QString &path : commonPaths) {
            if (QFile::exists(path)) {
                return path;
            }
        }
        
        // Last resort, return just the name and hope it's in PATH
        return name;
    }
};

// Initialize static members
//extern QString PathConfig::qpdfPath;
//extern QString PathConfig::pdfjamPath;

#endif // PATHCONFIG_H
