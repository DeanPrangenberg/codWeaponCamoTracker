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
#include <QApplication>
#include <QCoreApplication>
#include <QTimer>
#include <QResizeEvent>
#include <QMessageBox>

class WeaponEditor : public QWidget {
Q_OBJECT

public:
    explicit WeaponEditor(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override {
        updateWeaponClass();
    }

private slots:

    void updateWeaponClass();

    void loadWeaponData();

    void ensureFileExists();

    void createWeaponTile(const QString &weaponName, int index, int rowSize);

    void genJsonWeaponData();

    void liveUpdateWeaponData(const QString &weaponName, const QString &key, bool status);

    void updateStatus();

private:
    // global hud elements
    QComboBox *classComboBox;
    QWidget *weaponContainer;
    QScrollArea *scrollArea;
    QComboBox *camoTypeComboBox;
    QGridLayout *weaponLayout;
    QLabel *unlockedWeaponsLabel;
    QLabel *maxedLevelWeaponsLabel;
    QLabel *darkMatterStatusLabel;
    QLabel *polyatomicWeaponsStatusLabel;
    QLabel *diamondWeaponsStatusLabel;
    QLabel *goldInClassStatusLabel;
    QStringList weaponClasses;

    //global var
    QMap<QString, QStringList> weaponMap;
    QString fileName = "weaponData.json";
    QVector<int> standardWeaponAmount;
    int totalStandardWeaponAmount = 0;

    // camos var
    QStringList mCamos;
    QStringList zCamos;
    QStringList wCamos;

    bool autoUnlockMasteryCamo = false;
    int standardCamoAmount = 11;
    int camoNameLableWidth = 95;

    int goldPos;
    int diamondPos;
    int polyatomicPos;
    int darkMatterPos;

    // style
    void updateStyles();

    QVector<QComboBox *> comboBoxes;
    QVector<QLabel *> statusLabels;

    // functions
    void initializeWeaponData();

    void setupUI();

    void loadWeaponsForClass(const QString &weaponClass);

    void camoLogic();
};

#endif // WEAPONEDITOR_H
