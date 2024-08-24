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
    QLabel *globalUnlockedWeaponsLabel;
    QLabel *globalMaxedLevelWeaponsLabel;
    QLabel *darkMatterStatusLabel;
    QLabel *polyatomicStatusLabel;
    QLabel *diamondStatusLabel;
    QStringList weaponClasses;

    //global var
    QMap<QString, QStringList> weaponMap;
    QString fileName = "weaponData.json";
    QVector<int> standardWeaponAmount;
    int totalStandardWeaponAmount = 0;

    // camos var
    bool autoUnlockMasteryCamo = true;
    int standardCamoAmount = 4;
    int camoNameLableWidth = 95;

    static constexpr int goldPos = 4;
    static constexpr int diamondPos = 5;
    static constexpr int polyatomicPos = 6;
    static constexpr int darkMatterPos = 7;

    QString goldName = "GOLD";
    QString diamondName = "DIAMOND";
    QString polyatomicName = "POLYATOMIC";
    QString darkMatterName = "DARK_MATTER";

    // style
    void updateStyles();
    QVector<QComboBox*> comboBoxes;
    QVector<QLabel*> statusLabels;

    // functions
    void initializeWeaponData();
    void setupUI();
    void loadWeaponsForClass(const QString &weaponClass);
    void camoLogic();
};

#endif // WEAPONEDITOR_H
