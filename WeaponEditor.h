#ifndef WEAPONEDITOR_H
#define WEAPONEDITOR_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QMessageBox>
#include <QFormLayout>
#include <QLabel>
#include <QGridLayout>
#include <QMap>
#include <QPair>
#include <QCoreApplication>

class WeaponEditor : public QWidget {
Q_OBJECT

public:
    explicit WeaponEditor(QWidget *parent = nullptr);

private slots:
    void updateWeaponClass();
    void loadWeaponData();
    void ensureFileExists();
    void createWeaponTile(const QString &weaponName, int index);
    void genJsonWeaponData();
    void liveUpdateWeaponData(const QString &weaponName, const QString &key, bool status);
    void updateStatus();

private:
    QComboBox *classComboBox;
    QWidget *weaponContainer;
    QScrollArea *scrollArea;
    QComboBox *camoTypeComboBox;
    QGridLayout *weaponLayout; // Changed to QGridLayout
    QLabel *globalUnlockedWeaponsLabel;
    QLabel *globalMaxedLevelWeaponsLabel;
    QLabel *darkMatterStatusLabel;
    QLabel *diamondStatusLabel;
    QStringList weaponClasses;
    QMap<QString, QStringList> weaponMap;
    QString fileName = "D:\\MyRepo\\c++ QT\\codWeaponCamoTracker\\weaponData.json";


    // style
    void updateStyles();
    QVector<QComboBox*> comboBoxes;
    QVector<QLabel*> statusLabels;

    void initializeWeaponData();
    void setupUI();
    void loadWeaponsForClass(const QString &weaponClass, const QString &camoType);
    void camoLogic();
};

#endif // WEAPONEDITOR_H
