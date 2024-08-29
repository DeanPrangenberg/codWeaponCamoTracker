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
    bool autoUnlockMasteryCamo = false;
    int standardCamoAmount = 4;
    int camoNameLableWidth = 95;

    static constexpr int goldPos = 4;
    static constexpr int diamondPos = 5;
    static constexpr int polyatomicPos = 6;
    static constexpr int darkMatterPos = 7;

    QStringList mCamos = {
            "M_Camo1", "M_Camo2", "M_Camo3", "M_Camo4", "Gold", "Diamond", "Dark Spine", "Dark Matter"
    };
    QStringList zCamos = {
            "Z_Camo1", "Z_Camo2", "Z_Camo3", "Z_Camo4", "Mystic Gold", "Opal", "Afterlife", "Nebula"
    };
    QStringList wCamos = {
            "W_Camo1", "W_Camo2", "W_Camo3", "W_Camo4", "Gold Tiger", "Kings's Ransom", "Catalyst", "Abyss"
    };

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
