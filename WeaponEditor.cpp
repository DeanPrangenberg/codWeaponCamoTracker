#include "WeaponEditor.h"
#include <QCoreApplication>
#include <QDebug>

WeaponEditor::WeaponEditor(QWidget *parent) : QWidget(parent) {
    initializeWeaponData();
    ensureFileExists();

    setupUI();
    loadWeaponData();
}

void WeaponEditor::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Add class selection ComboBox
    classComboBox = new QComboBox();
    weaponClasses = QStringList{
            "Assault Rifles", "SMGs", "Marksman Rifles", "Battle Rifles",
            "LMGs", "Sniper Rifles", "Shotguns", "Pistols", "Launchers", "Melee"
    };
    classComboBox->addItems(weaponClasses);
    connect(classComboBox, &QComboBox::currentTextChanged, this, &WeaponEditor::updateWeaponClass);
    mainLayout->addWidget(classComboBox);

    // Add camo type selection ComboBox
    camoTypeComboBox = new QComboBox();
    camoTypeComboBox->addItem("Multiplayer");
    camoTypeComboBox->addItem("Zombies");
    connect(camoTypeComboBox, &QComboBox::currentTextChanged, this, &WeaponEditor::updateWeaponClass);
    mainLayout->addWidget(camoTypeComboBox);

    // Scroll area for displaying weapon tiles
    QScrollArea *scrollArea = new QScrollArea();
    weaponContainer = new QWidget();
    weaponLayout = new QGridLayout(); // Changed to QGridLayout
    weaponContainer->setLayout(weaponLayout);
    scrollArea->setWidget(weaponContainer);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // Save button at the bottom center
    saveButton = new QPushButton("Save");
    connect(saveButton, &QPushButton::clicked, this, &WeaponEditor::saveChangedData);
    mainLayout->addWidget(saveButton);
}

void WeaponEditor::initializeWeaponData() {
    // Initialize weapon data for demo purposes
    weaponMap["Assault Rifles"] = QStringList{
            "FTAC Light", "Kastov 74", "Krig C", "M16", "M4-S", "R-2", "SA87"
    };
    weaponMap["SMGs"] = QStringList{
            "AMR9", "FMG9", "Kastov-M", "Lachmann Sub", "MD-97L", "Raptor-9", "VAP 2", "VAP 3", "VCAR"
    };
    weaponMap["Marksman Rifles"] = QStringList{
            "CAMRS", "Kastov 97", "Lienna 550", "MK-18", "Tempus Torrent", "VAP-M"
    };
    weaponMap["Battle Rifles"] = QStringList{
            "C58", "Kastov 73", "Lienna 57", "TAQ-F"
    };
    weaponMap["LMGs"] = QStringList{
            "IP 545", "LRC-308", "SAW-H", "SAW-L"
    };
    weaponMap["Sniper Rifles"] = QStringList{
            "Kastov-M", "LW3 – Tundra", "Ratio-H", "Ratio-P"
    };
    weaponMap["Shotguns"] = QStringList{
            "M500", "OCP500", "Olympia", "Roku 360", "Super Short"
    };
    weaponMap["Pistols"] = QStringList{
            "Dianolli", "GP13 Auto", "Lach-30", "Lach-45", "Lach-9", "Sykov", "TAQ Handheld"
    };
    weaponMap["Launchers"] = QStringList{
            "LAW", "Panzerfaust", "Stinger"
    };
    weaponMap["Melee"] = QStringList{
            "Breaching Tool", "Hunting Knife", "Scout Knife"
    };
}

void WeaponEditor::updateWeaponClass() {
    QString selectedClass = classComboBox->currentText();
    // Reload weapons for the selected class
    loadWeaponsForClass(selectedClass);
}

void WeaponEditor::loadWeaponsForClass(const QString &weaponClass) {
    // Clear existing tiles
    QLayoutItem *item;
    while ((item = weaponLayout->takeAt(0))) {
        delete item->widget();
        delete item;
    }

    // Load weapon names and create tiles
    QStringList weaponNames = weaponMap.value(weaponClass);
    for (int i = 0; i < weaponNames.size(); ++i) {
        createWeaponTile(weaponNames[i], i);
    }
}

void WeaponEditor::createWeaponTile(const QString &weaponName, int index) {
    QWidget *tile = new QWidget();
    QFormLayout *tileLayout = new QFormLayout(tile);

    // Display weapon name
    QLabel *nameLabel = new QLabel("Weapon Name:");
    QLineEdit *nameEdit = new QLineEdit(weaponName);
    nameEdit->setReadOnly(true);
    tileLayout->addRow(nameLabel, nameEdit);

    bool unlocked = false;
    int currentLevel = 0;
    int maxLevel = 100;
    QJsonObject camosObject;

    // Load weapon data
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Unable to open file.";
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));

    if (doc.isNull()) {
        qDebug() << "Error: Failed to parse JSON.";
        return;
    }

    if (!doc.isObject()) {
        qDebug() << "Error: JSON is not an object.";
        return;
    }

    QJsonObject weaponData = doc.object();

    // Check for WeaponClasses in weaponData
    if (!weaponData.contains("WeaponClasses") || !weaponData["WeaponClasses"].isArray()) {
        qDebug() << "Error: WeaponClasses not found or not an array.";
        return;
    }

    QJsonArray weaponClassesArray = weaponData["WeaponClasses"].toArray();
    QString selectedClass = classComboBox->currentText();
    QStringList weaponsInClass = weaponMap.value(selectedClass);

    int weaponIndex = weaponsInClass.indexOf(weaponName);
    if (weaponIndex != -1) {
        bool classFound = false;

        for (const QJsonValue &value : weaponClassesArray) {
            QJsonObject classObject = value.toObject();
            if (classObject["Class"].toString() == selectedClass) {
                QJsonArray weaponsArray = classObject["Weapons"].toArray();
                if (weaponIndex < weaponsArray.size()) {
                    QJsonObject weaponObject = weaponsArray[weaponIndex].toObject();
                    unlocked = weaponObject["Unlocked"].toBool();
                    currentLevel = weaponObject["CurrentLevel"].toInt();
                    maxLevel = weaponObject["MaxLevel"].toInt();
                    camosObject = weaponObject["Camos"].toObject();
                    classFound = true;
                }
                break;
            }
        }

        if (!classFound) {
            qDebug() << "Error: Class not found in WeaponClasses.";
            return;
        }
    } else {
        qDebug() << "Error: Weapon not found in selected class.";
        return;
    }

    // Display unlocked status
    QLabel *unlockedLabel = new QLabel("Unlocked:");
    QCheckBox *unlockedCheckBox = new QCheckBox();
    unlockedCheckBox->setChecked(unlocked);
    tileLayout->addRow(unlockedLabel, unlockedCheckBox);

    // Display current level
    QLabel *currentLevelLabel = new QLabel("Current Level:");
    QLineEdit *currentLevelEdit = new QLineEdit(QString::number(currentLevel));
    tileLayout->addRow(currentLevelLabel, currentLevelEdit);

    // Display max level
    QLabel *maxLevelLabel = new QLabel("Max Level:");
    QLineEdit *maxLevelEdit = new QLineEdit(QString::number(maxLevel));
    tileLayout->addRow(maxLevelLabel, maxLevelEdit);

    // Create widget for displaying camos
    QWidget *camoWidget = new QWidget();
    QFormLayout *camoLayout = new QFormLayout(camoWidget);

    // Function to update camo display
    auto updateCamos = [this, &camosObject, camoLayout]() {
        // Clear previous camo widgets
        QLayoutItem *item;
        while ((item = camoLayout->takeAt(0))) {
            delete item->widget();
            delete item;
        }

        QString selectedType = camoTypeComboBox->currentText();
        for (int i = 1; i <= 7; ++i) {
            QString camoNameKey;
            QString camoStatusKey;

            if (selectedType == "Multiplayer") {
                camoNameKey = QString("M_Camo%1Name").arg(i);
                camoStatusKey = QString("M_Camo%1Status").arg(i);
            } else {
                camoNameKey = QString("Z_Camo%1Name").arg(i);
                camoStatusKey = QString("Z_Camo%1Status").arg(i);
            }

            if (camosObject.contains(camoNameKey)) {
                QString camoName = camosObject.value(camoNameKey).toString();
                bool camoStatus = camosObject.value(camoStatusKey).toBool();

                if (!camoName.isEmpty()) { // Only add if there's a name
                    QLabel *camoNameLabel = new QLabel(camoName);
                    QCheckBox *camoCheckBox = new QCheckBox();
                    camoCheckBox->setChecked(camoStatus);
                    camoLayout->addRow(camoNameLabel, camoCheckBox);
                }
            }
        }
    };

    // Update camos initially
    updateCamos();

    tileLayout->addRow(camoWidget);

    // Calculate grid position
    int row = index / 3; // Each row has 3 tiles
    int column = index % 3; // Position within the row

    // Add the tile widget to the layout
    weaponLayout->addWidget(tile, row, column);
}

void WeaponEditor::genJsonWeaponData() {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Error", "Cannot open file for writing.");
        qDebug() << "Failed to open file for writing:" << fileName;
        return;
    }

    QJsonArray weaponsArray;

    for (auto it = weaponMap.constBegin(); it != weaponMap.constEnd(); ++it) {
        const QString &classType = it.key();
        const QStringList &weapons = it.value();

        QJsonObject classObject;
        classObject["Class"] = classType;

        QJsonArray weaponArray;
        for (const QString &weaponName : weapons) {
            QJsonObject weaponObject;
            weaponObject["Name"] = weaponName;
            weaponObject["Unlocked"] = false; // default value
            weaponObject["CurrentLevel"] = 0; // default value
            weaponObject["MaxLevel"] = 100; // default value

            QJsonObject camoObject;
            for (int i = 1; i <= 7; ++i) {
                camoObject[QString("M_Camo%1Name").arg(i)] = QString("M_Camo%1").arg(i);
                camoObject[QString("M_Camo%1Status").arg(i)] = false; // default value

                camoObject[QString("Z_Camo%1Name").arg(i)] = QString("Z_Camo%1").arg(i);
                camoObject[QString("Z_Camo%1Status").arg(i)] = false; // default value
            }
            camoObject["M_Camo5Name"] = "M_GOLD";
            camoObject["M_Camo5Status"] = false;
            camoObject["M_Camo6Name"] = "M_DIAMOND";
            camoObject["M_Camo6Status"] = false;
            camoObject["M_Camo7Name"] = "M_DARK_MATTER";
            camoObject["M_Camo7Status"] = false;
            camoObject["Z_Camo5Name"] = "Z_GOLD";
            camoObject["Z_Camo5Status"] = false;
            camoObject["Z_Camo6Name"] = "Z_DIAMOND";
            camoObject["Z_Camo6Status"] = false;
            camoObject["Z_Camo7Name"] = "Z_DARK_MATTER";
            camoObject["Z_Camo7Status"] = false;

            weaponObject["Camos"] = camoObject;
            weaponArray.append(weaponObject);
        }
        classObject["Weapons"] = weaponArray;
        weaponsArray.append(classObject);
    }

    QJsonObject json;
    json["WeaponClasses"] = weaponsArray;

    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    qDebug() << "Weapon data saved to" << fileName;
}

void WeaponEditor::loadWeaponData() {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Cannot open file for reading.");
        qDebug() << "Failed to open file for reading:" << fileName;
        return;
    }

    QByteArray fileContent = file.readAll();
    file.close();

    // Debug output to verify file content
    // qDebug() << "File content:" << fileContent;

    QJsonDocument doc = QJsonDocument::fromJson(fileContent);

    if (doc.isNull()) {
        QMessageBox::warning(this, "Error", "Invalid JSON data in file.");
        qDebug() << "Failed to parse JSON document.";
        return;
    }

    QJsonObject json = doc.object();
    if (!json.contains("WeaponClasses") || !json["WeaponClasses"].isArray()) {
        QMessageBox::warning(this, "Error", "Invalid JSON format: 'WeaponClasses' missing or not an array.");
        qDebug() << "Invalid JSON format.";
        return;
    }

    QJsonArray weaponClassesArray = json["WeaponClasses"].toArray();
    weaponMap.clear();

    for (const QJsonValue &value : weaponClassesArray) {
        QJsonObject classObject = value.toObject();
        QString classType = classObject["Class"].toString();
        QJsonArray weaponsArray = classObject["Weapons"].toArray();

        QStringList weaponsList;
        for (const QJsonValue &weaponValue : weaponsArray) {
            QJsonObject weaponObject = weaponValue.toObject();
            weaponsList.append(weaponObject["Name"].toString());
        }
        weaponMap[classType] = weaponsList;
    }

    // Debug output to verify the contents of weaponMap
    // qDebug() << "Loaded weaponMap:" << weaponMap;

    // Reload the data for the currently selected class
    updateWeaponClass();
}

void WeaponEditor::ensureFileExists() {
    QFile file(fileName);
    if (!file.exists()) {
        genJsonWeaponData(); // Create file with default values
        qDebug() << "File did not exist. Created new file:" << fileName;
    } else {
        qDebug() << "File exists:" << fileName;
    }
}

void WeaponEditor::saveChangedData() {

}