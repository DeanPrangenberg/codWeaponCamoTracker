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
    void saveChangedData();

private:
    QComboBox *classComboBox;
    QWidget *weaponContainer;
    QComboBox *camoTypeComboBox;
    QGridLayout *weaponLayout; // Changed to QGridLayout
    QPushButton *saveButton;

    QStringList weaponClasses;
    QMap<QString, QStringList> weaponMap;
    QString fileName = "D:\\MyRepo\\c++ QT\\codWeaponCamoTracker\\weaponData.json"; // Updated to be relative to the application directory

    QMap<QString, QList<QWidget*>> weaponWidgets; // To store widgets for each weapon
    QJsonObject weaponData; // Add this member variable to store JSON data

    void initializeWeaponData();
    void loadWeaponsForClass(const QString &classType);
    void setupUI();
};

#endif // WEAPONEDITOR_H