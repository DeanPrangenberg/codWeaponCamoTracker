#include "WeaponEditor.h"
#include <QDebug>

WeaponEditor::WeaponEditor(QWidget *parent) : QWidget(parent) {
    initializeWeaponData();
    ensureFileExists();
    setupUI();
    loadWeaponData();
    if (autoUnlockMasteryCamo) {
        polyatomicStatusLabel->hide();
    }
}

void WeaponEditor::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Add class selection ComboBox
    classComboBox = new QComboBox();

    for (const auto& key : weaponMap.keys()) {
        weaponClasses.append(key);
    }
    classComboBox->addItems(weaponClasses);
    classComboBox->setCurrentText("Assault Rifles");  // Set default selection
    comboBoxes.push_back(classComboBox);
    connect(classComboBox, &QComboBox::currentTextChanged, this, &WeaponEditor::updateWeaponClass);

    // Add camo type selection ComboBox
    camoTypeComboBox = new QComboBox();
    camoTypeComboBox->addItem("Multiplayer");
    camoTypeComboBox->addItem("Zombies");
    camoTypeComboBox->setCurrentText("Multiplayer");  // Set default selection
    comboBoxes.push_back(camoTypeComboBox);
    connect(camoTypeComboBox, &QComboBox::currentTextChanged, this, &WeaponEditor::updateWeaponClass);

    QHBoxLayout *dropdownLayout = new QHBoxLayout;
    dropdownLayout->addWidget(classComboBox);
    dropdownLayout->addWidget(camoTypeComboBox);

    mainLayout->addLayout(dropdownLayout);
    // Status labels
    QHBoxLayout *statusLayout = new QHBoxLayout();

    globalUnlockedWeaponsLabel = new QLabel();
    globalMaxedLevelWeaponsLabel = new QLabel();
    darkMatterStatusLabel = new QLabel();
    polyatomicStatusLabel = new QLabel();
    diamondStatusLabel = new QLabel();

    globalUnlockedWeaponsLabel->setObjectName("globalUnlockedWeaponsLabel");
    globalMaxedLevelWeaponsLabel->setObjectName("globalMaxedLevelWeaponsLabel");
    darkMatterStatusLabel->setObjectName("darkMatterStatusLabel");
    polyatomicStatusLabel->setObjectName("polyatomicStatusLabel");
    diamondStatusLabel->setObjectName("diamondStatusLabel");

    // Add labels to statusLabels vector
    statusLabels.push_back(globalUnlockedWeaponsLabel);
    statusLabels.push_back(globalMaxedLevelWeaponsLabel);
    statusLabels.push_back(darkMatterStatusLabel);
    statusLabels.push_back(polyatomicStatusLabel);
    statusLabels.push_back(diamondStatusLabel);

    // Add labels to status layout
    statusLayout->addWidget(globalUnlockedWeaponsLabel);
    statusLayout->addWidget(globalMaxedLevelWeaponsLabel);
    statusLayout->addWidget(darkMatterStatusLabel);
    statusLayout->addWidget(polyatomicStatusLabel);
    statusLayout->addWidget(diamondStatusLabel);

    // Add status layout to main layout
    mainLayout->addLayout(statusLayout);

    // Scroll area for displaying weapon tiles
    scrollArea = new QScrollArea();
    weaponContainer = new QWidget();
    weaponLayout = new QGridLayout(); // Changed to QGridLayout
    weaponLayout->setAlignment(Qt::AlignTop);
    weaponContainer->setLayout(weaponLayout);
    weaponContainer->setStyleSheet("background-color: #272626; border: 2px solid #444444; border-radius: 10px;");

    scrollArea->setWidget(weaponContainer);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // Initial update
    updateWeaponClass();
    updateStyles();
}

void WeaponEditor::updateStatus() {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file:" << fileName;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc(QJsonDocument::fromJson(data));
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Failed to parse JSON or not an object.";
        return;
    }

    QJsonObject weaponData = doc.object();
    QJsonArray weaponClassesArray = weaponData["WeaponClasses"].toArray();
    QString selectedCamoType = camoTypeComboBox->currentText() == "Multiplayer" ? "M_GOLD" : "Z_GOLD";
    QString selectedClass = classComboBox->currentText();

    int totalWeapons = 0;
    int unlockedWeapons = 0;
    int maxedWeapons = 0;
    int goldCamoWeapons = 0;
    int goldInSelectedClass = 0;
    int totalClassWeapons = 0;
    int polyatomicUnlockedCamos = 0;
    int polyatomicTotalCamos = 0;
    int darkMatterUnlockedCamos = 0;
    int darkMatterTotalCamos = 0;

    QMap<QString, int> weaponPerClass;
    int index = 0;
    for (auto it = weaponMap.constBegin(); it != weaponMap.constEnd(); ++it) {
        weaponPerClass.insert(it.key(), standardWeaponAmount[index]);
        ++index;
    }

    for (const QJsonValue &classValue : weaponClassesArray) {
        QJsonObject classObject = classValue.toObject();
        QString weaponClassName = classObject["Class"].toString();
        QJsonArray weaponsArray = classObject["Weapons"].toArray();

        for (const QJsonValue &weaponValue : weaponsArray) {
            QJsonObject weaponObject = weaponValue.toObject();
            totalWeapons++;

            bool isUnlocked = weaponObject["Unlocked"].toBool();
            if (isUnlocked) {
                unlockedWeapons++;
            }

            bool isMaxed = weaponObject["MaxedLevel"].toBool();
            if (isMaxed) {
                maxedWeapons++;
            }

            // Diamond Status (unverändert)
            QJsonArray camosArray;
            if (selectedCamoType == "M_GOLD") {
                camosArray = weaponObject["M_CAMOS"].toArray();
            } else if (selectedCamoType == "Z_GOLD") {
                camosArray = weaponObject["Z_CAMOS"].toArray();
            } else {
                continue;
            }

            bool hasSelectedGoldCamo = false;
            for (const QJsonValue &camoValue : camosArray) {
                QJsonObject camoObject = camoValue.toObject();
                QString camoName = camoObject["Name"].toString();
                bool camoStatus = camoObject["Status"].toBool();

                if (camoName == selectedCamoType && camoStatus) {
                    hasSelectedGoldCamo = true;
                    break;
                }
            }

            if (hasSelectedGoldCamo) {
                goldCamoWeapons++;
                if (weaponClassName == selectedClass) {
                    goldInSelectedClass++;
                }
            }

            if (weaponClassName == selectedClass) {
                totalClassWeapons++;
            }
            if (camosArray[polyatomicPos].toObject()["Status"].toBool()) {
                polyatomicUnlockedCamos++;
            }
            if (camosArray[darkMatterPos].toObject()["Status"].toBool()) {
                darkMatterUnlockedCamos++;
            }
            polyatomicTotalCamos++;
            darkMatterTotalCamos++;
        }
    }

    if (totalWeapons > 0) {
        double unlockedPercentage = (static_cast<double>(unlockedWeapons) / totalWeapons) * 100;
        double maxedPercentage = (static_cast<double>(maxedWeapons) / totalWeapons) * 100;
        double selectedClassGoldCamoPercentage = (totalClassWeapons > 0) ?
                                                 (static_cast<double>(goldInSelectedClass) / weaponPerClass[selectedClass]) * 100 : 0.0;  // Für den Diamond Status (unverändert)
        double polyatomicPercentage = (darkMatterTotalCamos > 0) ? (static_cast<double>(polyatomicUnlockedCamos) / totalStandardWeaponAmount) * 100 : 0.0;
        double darkMatterPercentage = (darkMatterTotalCamos > 0) ? (static_cast<double>(darkMatterUnlockedCamos) / totalStandardWeaponAmount) * 100 : 0.0;

        QString camoTypeText = (camoTypeComboBox->currentText() == "Multiplayer") ? "Multiplayer" : "Zombies";
        QString classNameText = selectedClass.isEmpty() ? "Not Selected" : selectedClass;

        if (statusLabels.size() == 5) {
            statusLabels[0]->setText(QString("Unlocked Weapons Status:\n%1% (%2 out of %3)")
            .arg(QString::number(unlockedPercentage,'f', 2))
            .arg(unlockedWeapons)
            .arg(totalWeapons));

            statusLabels[1]->setText(QString("Maxed Level Weapons Status:\n%1% (%2 out of %3)")
            .arg(QString::number(maxedPercentage, 'f', 2))
            .arg(maxedWeapons)
            .arg(totalWeapons));

            statusLabels[2]->setText(QString("%1 Dark-Matter Status:\n%2% (%3 %4 out of %5)")
            .arg(camoTypeComboBox->currentText())
            .arg(QString::number(darkMatterPercentage, 'f', 2))
            .arg(darkMatterUnlockedCamos)
            .arg((autoUnlockMasteryCamo) ? "diamonds" : "polyatomics")
            .arg(totalStandardWeaponAmount));

            statusLabels[3]->setText(QString("%1 Polyatomic Status:\n%2% (%3 diamonds out of %4)")
            .arg(camoTypeComboBox->currentText())
            .arg(QString::number(polyatomicPercentage, 'f', 2))
            .arg(polyatomicUnlockedCamos)
            .arg(totalStandardWeaponAmount));

            statusLabels[4]->setText(QString("%1 Diamond Status:\n%2% (%3 golds out of %4)")
            .arg(classComboBox->currentText())
            .arg(QString::number(selectedClassGoldCamoPercentage, 'f', 2))
            .arg(goldInSelectedClass)
            .arg(weaponPerClass[selectedClass]));
        }
    } else {
        if (statusLabels.size() == 5) {
            statusLabels[0]->setText("No weapons found.");
            statusLabels[1]->setText("No weapons found.");
            statusLabels[2]->setText("No weapons found.");
            statusLabels[3]->setText("No weapons found.");
            statusLabels[4]->setText("No weapons found.");
        }
    }
}

void WeaponEditor::initializeWeaponData() {
    standardWeaponAmount.push_back(5);
    weaponMap["Assault Rifles"] = QStringList{
            "AK-74", "AMES 85", "Colt Model 723", "SIG SG 550", "SR-3 Vikhr", "XM4"
    };

    standardWeaponAmount.push_back(1);
    weaponMap["Battle Rifles"] = QStringList{
            "PLACE_HOLDER"
    };

    standardWeaponAmount.push_back(2);
    weaponMap["LMGs"] = QStringList{
            "AR10", "KSP 58"
    };

    standardWeaponAmount.push_back(1);
    weaponMap["Launchers"] = QStringList{
            "PLACE_HOLDER"
    };

    standardWeaponAmount.push_back(1);
    weaponMap["Marksman Rifles"] = QStringList{
            "SR-25"
    };

    standardWeaponAmount.push_back(1);
    weaponMap["Melee"] = QStringList{
            "Combat Knife"
    };

    standardWeaponAmount.push_back(3);
    weaponMap["Pistols"] = QStringList{
            "Compact 92", "Makarov PM", "USP"
    };

    standardWeaponAmount.push_back(3);
    weaponMap["SMGs"] = QStringList{
            "FAMAE SAF", "Jackal PDW", "MP5A3"
    };

    standardWeaponAmount.push_back(1);
    weaponMap["Shotguns"] = QStringList{
            "Marine SP"
    };

    standardWeaponAmount.push_back(1);
    weaponMap["Sniper Rifles"] = QStringList{
            "LR 7.62"
    };

    for (int weaponsInClass : standardWeaponAmount) {
        totalStandardWeaponAmount += weaponsInClass;
    }
}

void WeaponEditor::updateWeaponClass() {
    QString selectedClass = classComboBox->currentText();
    QString selectedCamoType = camoTypeComboBox->currentText();
    loadWeaponsForClass(selectedClass, selectedCamoType);
    updateStatus();
}

void WeaponEditor::loadWeaponsForClass(const QString &weaponClass, const QString &camoType) {
    // Clear existing layout items
    while (QLayoutItem *item = weaponLayout->takeAt(0)) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // Load the weapons for the class
    QStringList weaponNames = weaponMap.value(weaponClass);
    for (int i = 0; i < weaponNames.size(); ++i) {
        createWeaponTile(weaponNames[i], i);
    }
}

void WeaponEditor::createWeaponTile(const QString &weaponName, int index) {
    // Erstelle ein Widget für den Tile
    QWidget *tile = new QWidget();
    if (autoUnlockMasteryCamo) {
        tile->setFixedHeight(315 - 20); // polyatomic gibts nicht im standard system
    } else {
        tile->setFixedHeight(315);
    }
    tile->setFixedWidth(316);

    tile->setStyleSheet(R"(
        QWidget {
            background-color: #333333;
            }
        )"
    );

    QVBoxLayout *tileLayout = new QVBoxLayout(tile);
    tileLayout->setContentsMargins(10, 10, 10, 10);

    // Erstelle ein Widget für die Waffen Camo status oben im Tile
    QWidget *colorHeaderWidget = new QWidget();
    QHBoxLayout *colorHeaderLayout = new QHBoxLayout(colorHeaderWidget);
    colorHeaderLayout->setContentsMargins(0, 0, 0, 0);
    colorHeaderWidget->setStyleSheet("border: none;");

    QLabel *statusLabel = new QLabel("Camo Status:");
    QLabel *colorLabel = new QLabel();
    colorLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    colorHeaderLayout->addWidget(statusLabel);
    colorHeaderLayout->addWidget(colorLabel);

    tileLayout->addWidget(colorHeaderWidget);

    // Erstelle ein Widget für den Inhalt des Tiles
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentWidget->setStyleSheet("border: none;");

    // Display weapon name
    QHBoxLayout *nameLayout = new QHBoxLayout();
    QLabel *nameLabel = new QLabel("Weapon Name:");
    QLineEdit *nameEdit = new QLineEdit(weaponName);
    nameEdit->setAlignment(Qt::AlignCenter);
    nameEdit->setReadOnly(true);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    nameEdit->setStyleSheet(
            "QLineEdit {"
            "   border: 2px solid #444444;"
            "   border-radius: 10px;"
            "   background-color: #2e2e2e;"
            "}"
    );

    contentLayout->addLayout(nameLayout);

    // Lade Waffendaten
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Unable to open file.";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc(QJsonDocument::fromJson(data));
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Error: Failed to parse JSON.";
        return;
    }

    QJsonObject weaponData = doc.object();
    QJsonArray weaponClassesArray = weaponData["WeaponClasses"].toArray();
    QString selectedClass = classComboBox->currentText();
    QStringList weaponsInClass = weaponMap.value(selectedClass);
    int weaponIndex = weaponsInClass.indexOf(weaponName);

    bool unlocked = false;
    bool maxedLevel = false;
    QJsonArray mCamosArray;
    QJsonArray zCamosArray;

    if (weaponIndex != -1) {
        for (const QJsonValue &value : weaponClassesArray) {
            QJsonObject classObject = value.toObject();
            if (classObject["Class"].toString() == selectedClass) {
                QJsonArray weaponsArray = classObject["Weapons"].toArray();
                if (weaponIndex < weaponsArray.size()) {
                    QJsonObject weaponObject = weaponsArray[weaponIndex].toObject();
                    unlocked = weaponObject["Unlocked"].toBool();
                    maxedLevel = weaponObject["MaxedLevel"].toBool();
                    mCamosArray = weaponObject["M_CAMOS"].toArray();
                    zCamosArray = weaponObject["Z_CAMOS"].toArray();
                }
                break;
            }
        }
    }

    // Display unlocked status
    QHBoxLayout *unlockedLayout = new QHBoxLayout();
    QLabel *unlockedLabel = new QLabel("Unlocked:");
    unlockedLabel->setFixedWidth(camoNameLableWidth + 10);

    QCheckBox *unlockedCheckBox = new QCheckBox();
    unlockedCheckBox->setChecked(unlocked);
    unlockedLayout->addWidget(unlockedLabel);
    unlockedLayout->addWidget(unlockedCheckBox);

    contentLayout->addLayout(unlockedLayout);

    unlockedCheckBox->setStyleSheet(
            "QCheckBox {"
            "   color: #333333;"
            "}"
            "QCheckBox::indicator {"
            "   border: 2px solid rgba(0, 0, 0, 0.0);"
            "   border-radius: 5px;"
            "}"
            "QCheckBox::indicator:unchecked {"
            "   background-color: #8f0e00;"
            "}"
            "QCheckBox::indicator:checked {"
            "   background-color: #328f00;"
            "}"
    );

    // Display maxed level
    QHBoxLayout *maxedLevelLayout = new QHBoxLayout();
    QLabel *maxedLevelLabel = new QLabel("Maxed Level:");
    maxedLevelLabel->setFixedWidth(camoNameLableWidth + 10);
    QCheckBox *maxedLevelCheckBox = new QCheckBox();
    maxedLevelCheckBox->setChecked(maxedLevel);
    maxedLevelLayout->addWidget(maxedLevelLabel);
    maxedLevelLayout->addWidget(maxedLevelCheckBox);

    contentLayout->addLayout(maxedLevelLayout);

    maxedLevelCheckBox->setStyleSheet(
            "QCheckBox {"
            "   color: #333333;"
            "}"
            "QCheckBox::indicator {"
            "   border: 2px solid rgba(0, 0, 0, 0.0);"
            "   border-radius: 5px;"
            "}"
            "QCheckBox::indicator:unchecked {"
            "   background-color: #8f0e00;"
            "}"
            "QCheckBox::indicator:checked {"
            "   background-color: #328f00;"
            "}"
    );

    // Get selected camo type
    QString selectedCamoType = camoTypeComboBox->currentText();

    // Create camo UI elements
    QJsonArray camosToShow;
    if (selectedCamoType == "Multiplayer") {
        camosToShow = mCamosArray;
    } else if (selectedCamoType == "Zombies") {
        camosToShow = zCamosArray;
    }

    QVBoxLayout *camoVBoxLayout = new QVBoxLayout();

    for (int i = 0; i < camosToShow.size(); ++i) {
        if (i == camosToShow.size() - 2 && autoUnlockMasteryCamo) {
            continue;
        }
        QJsonObject camoObject = camosToShow[i].toObject();
        QString camoName = camoObject["Name"].toString();
        bool camoStatus = camoObject["Status"].toBool();
        QString camoCondition = camoObject["Condition"].toString();

        QHBoxLayout *camoLayout = new QHBoxLayout();
        camoLayout->setContentsMargins(10, 0, 0, 0);

        QLabel *camoLabel = new QLabel(camoName + ":");
        camoLabel->setFixedWidth(camoNameLableWidth);
        camoLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

        QLineEdit *camoLineEdit = new QLineEdit(camoCondition);
        camoLineEdit->setAlignment(Qt::AlignCenter);
        camoLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        camoLineEdit->setStyleSheet(
                "QLineEdit {"
                "   border: 2px solid #444444;"
                "   border-radius: 10px;"
                "   background-color: #2e2e2e;"
                "}"
        );

        QCheckBox *camoCheckBox = new QCheckBox();
        camoCheckBox->setStyleSheet(
                "QCheckBox {"
                "   color: #333333;"
                "}"
                "QCheckBox::indicator {"
                "   border: 2px solid rgba(0, 0, 0, 0.0);"
                "   border-radius: 5px;"
                "}"
                "QCheckBox::indicator:unchecked {"
                "   background-color: #8f0e00;"
                "}"
                "QCheckBox::indicator:checked {"
                "   background-color: #328f00;"
                "}"
        );

        camoCheckBox->setChecked(camoStatus);

        camoLayout->addWidget(camoLabel);
        camoLayout->addWidget(camoCheckBox);
        camoLayout->addWidget(camoLineEdit);

        QWidget *camoWidget = new QWidget();
        camoWidget->setLayout(camoLayout);

        camoVBoxLayout->addWidget(camoWidget);
        camoVBoxLayout->setSpacing(5); // Abstand zwischen den Camos
        camoVBoxLayout->setContentsMargins(0, 0, 0, 0);

        connect(camoCheckBox, &QCheckBox::stateChanged, [this, weaponName, camoName](int state) {
            liveUpdateWeaponData(weaponName, camoName, (state == Qt::Checked));
        });
    }

    // Erstelle ein Widget für die Tarnungen und setze das VBoxLayout darauf
    QWidget *camoContainerWidget = new QWidget();
    camoContainerWidget->setLayout(camoVBoxLayout);
    camoContainerWidget->setContentsMargins(0, 0, 0, 0); // Keine Margen
    camoContainerWidget->setStyleSheet("border: none;");
    contentLayout->addWidget(camoContainerWidget);

    connect(unlockedCheckBox, &QCheckBox::stateChanged, [this, weaponName](int state) {
        liveUpdateWeaponData(weaponName, "Unlocked", (state == Qt::Checked));
    });

    connect(maxedLevelCheckBox, &QCheckBox::stateChanged, [this, weaponName](int state) {
        liveUpdateWeaponData(weaponName, "MaxedLevel", (state == Qt::Checked));
    });

    // Füge das contentWidget zum Tile hinzu
    tileLayout->addWidget(contentWidget);

    // Erstelle die Farbe für das colorHeaderLabel
    QString colorHeaderBackgroundColor = "#4d4d4d"; // Standardfarbe
    for (const QJsonValue &camoValue : camosToShow) {
        QJsonObject camoObject = camoValue.toObject();
        QString camoName = camoObject["Name"].toString();
        bool camoStatus = camoObject["Status"].toBool();

        if (camoStatus) {
            if (camoName.contains(goldName)) {
                colorHeaderBackgroundColor = "#ffc400"; // Gold-Farbe
            } else if (camoName.contains(diamondName)) {
                colorHeaderBackgroundColor = "#4a7fe8"; // Diamant-Farbe
            } else if (camoName.contains(polyatomicName)) {
                colorHeaderBackgroundColor = "#6b005f"; // Polyatomic-Farbe
            } else if (camoName.contains(darkMatterName)) {
                colorHeaderBackgroundColor = "#eb4034"; // Dark Matter-Farbe
            }
        }
    }
    colorLabel->setStyleSheet(QString("QLabel {"
                                      "   border: 2px solid #444444;"
                                      "   border-radius: 10px;"
                                      "   color: #ffffff;"
                                      "   background-color: %1;"
                                      "}"
    ).arg(colorHeaderBackgroundColor));

    // Füge das Tile zum Layout hinzu
    weaponLayout->addWidget(tile, index / 3, index % 3); // Bleibe bei fester Positionierung der Tiles
}

void WeaponEditor::liveUpdateWeaponData(const QString &weaponName, const QString &key, bool status) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Unable to open file for reading.";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc(QJsonDocument::fromJson(data));
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Error: Failed to parse JSON.";
        return;
    }

    QJsonObject json = doc.object();
    QJsonArray weaponClassesArray = json["WeaponClasses"].toArray();
    QString selectedClass = classComboBox->currentText();
    QStringList weaponsInClass = weaponMap.value(selectedClass);

    QJsonArray updatedWeaponClassesArray;

    for (const QJsonValue &classValue : weaponClassesArray) {
        QJsonObject classObject = classValue.toObject();
        if (classObject["Class"].toString() == selectedClass) {
            QJsonArray weaponsArray = classObject["Weapons"].toArray();
            QJsonArray updatedWeaponsArray;

            for (const QJsonValue &weaponValue : weaponsArray) {
                QJsonObject weaponObject = weaponValue.toObject();
                if (weaponObject["Name"].toString() == weaponName) {
                    if (key == "Unlocked") {
                        weaponObject["Unlocked"] = status;
                    } else if (key == "MaxedLevel") {
                        weaponObject["MaxedLevel"] = status;
                    } else {
                        // Handle camo updates
                        bool isMultiplayer = key.startsWith("M_"); // Use a heuristic to determine if it's Multiplayer
                        if (isMultiplayer) {
                            QJsonArray camosArray = weaponObject["M_CAMOS"].toArray();
                            for (int i = 0; i < camosArray.size(); ++i) {
                                QJsonObject camoObject = camosArray[i].toObject();
                                if (camoObject["Name"].toString() == key) {
                                    camoObject["Status"] = status;
                                    camosArray[i] = camoObject; // Update the array with the modified object
                                    break;
                                }
                            }
                            weaponObject["M_CAMOS"] = camosArray;
                        } else {
                            QJsonArray zCamosArray = weaponObject["Z_CAMOS"].toArray();
                            for (int i = 0; i < zCamosArray.size(); ++i) {
                                QJsonObject camoObject = zCamosArray[i].toObject();
                                if (camoObject["Name"].toString() == key) {
                                    camoObject["Status"] = status;
                                    zCamosArray[i] = camoObject; // Update the array with the modified object
                                    break;
                                }
                            }
                            weaponObject["Z_CAMOS"] = zCamosArray;
                        }
                    }
                }
                updatedWeaponsArray.append(weaponObject);
            }
            classObject["Weapons"] = updatedWeaponsArray;
        }
        updatedWeaponClassesArray.append(classObject);
    }

    json["WeaponClasses"] = updatedWeaponClassesArray;

    QJsonDocument updatedDoc(json);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Unable to open file for writing.";
        return;
    }
    file.write(updatedDoc.toJson());
    file.close();

    camoLogic();
    updateStatus();
    updateWeaponClass();
}

void WeaponEditor::ensureFileExists() {
    QFile file(fileName);
    if (!file.exists()) {
        genJsonWeaponData();
    }
}

void WeaponEditor::genJsonWeaponData() {
    QJsonObject json;
    QJsonArray classesArray;

    QStringList mCamos = {
            "M_Camo1", "M_Camo2", "M_Camo3", "M_Camo4", "M_GOLD", "M_DIAMOND", "M_POLYATOMIC", "M_DARK_MATTER"
    };
    QStringList zCamos = {
            "Z_Camo1", "Z_Camo2", "Z_Camo3", "Z_Camo4", "Z_GOLD", "Z_DIAMOND", "Z_POLYATOMIC", "Z_DARK_MATTER"
    };

    for (const QString &className : weaponMap.keys()) {
        QJsonObject classObject;
        classObject["Class"] = className;

        QJsonArray weaponsArray;
        QStringList weapons = weaponMap.value(className);

        for (const QString &weaponName : weapons) {
            QJsonObject weaponObject;
            weaponObject["Name"] = weaponName;
            weaponObject["Unlocked"] = false;
            weaponObject["MaxedLevel"] = false;  // Neues Feld

            // Fügen Sie Multiplayer Camos hinzu
            QJsonArray mCamosArray;
            for (const QString &camoName : mCamos) {
                QJsonObject camoObject;
                camoObject["Name"] = camoName;
                camoObject["Status"] = false;
                camoObject["Condition"] = "todo for Camo";
                mCamosArray.append(camoObject);
            }
            weaponObject["M_CAMOS"] = mCamosArray;

            // Fügen Sie Zombies Camos hinzu
            QJsonArray zCamosArray;
            for (const QString &camoName : zCamos) {
                QJsonObject camoObject;
                camoObject["Name"] = camoName;
                camoObject["Status"] = false;
                camoObject["Condition"] = "todo for Camo";
                zCamosArray.append(camoObject);
            }
            weaponObject["Z_CAMOS"] = zCamosArray;

            weaponsArray.append(weaponObject);
        }

        classObject["Weapons"] = weaponsArray;
        classesArray.append(classObject);
    }

    json["WeaponClasses"] = classesArray;

    QJsonDocument doc(json);
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Unable to open file for writing.";
        return;
    }
    file.write(doc.toJson());
    file.close();
}

void WeaponEditor::loadWeaponData() {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Unable to open file.";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc(QJsonDocument::fromJson(data));
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Error: Failed to parse JSON.";
        return;
    }

    QJsonObject weaponData = doc.object();
    QJsonArray weaponClassesArray = weaponData["WeaponClasses"].toArray();

    // Optionally populate the UI with data
    for (const QJsonValue &value : weaponClassesArray) {
        QJsonObject classObject = value.toObject();
        QString className = classObject["Class"].toString();
        QJsonArray weaponsArray = classObject["Weapons"].toArray();

        for (const QJsonValue &weaponValue : weaponsArray) {
            QJsonObject weaponObject = weaponValue.toObject();
            QString weaponName = weaponObject["Name"].toString();
            bool unlocked = weaponObject["Unlocked"].toBool();
            int currentLevel = weaponObject["CurrentLevel"].toInt();
            int maxLevel = weaponObject["MaxLevel"].toInt();
            QJsonObject camosObject = weaponObject["Camos"].toObject();
        }
    }
}

void WeaponEditor::camoLogic() {
    QVector<QVector<QVector<QVector<bool>>>> camoStatus;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Unable to open file.";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc(QJsonDocument::fromJson(data));
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Error: Failed to parse JSON.";
        return;
    }

    QJsonObject weaponData = doc.object();
    QJsonArray weaponClassesArray = weaponData["WeaponClasses"].toArray();
    camoStatus.clear();

    for (const QJsonValue& classValue : weaponClassesArray) {
        QJsonObject classObject = classValue.toObject();
        QJsonArray weaponsArray = classObject["Weapons"].toArray();

        QVector<QVector<QVector<bool>>> classCamos;

        for (const QJsonValue& weaponValue : weaponsArray) {
            QJsonObject weaponObject = weaponValue.toObject();
            QJsonArray mCamosArray = weaponObject["M_CAMOS"].toArray();
            QJsonArray zCamosArray = weaponObject["Z_CAMOS"].toArray();

            QVector<QVector<bool>> weaponCamos(2);

            for (int i = 0; i < mCamosArray.size(); ++i) {
                weaponCamos[0].append(mCamosArray[i].toObject()["Status"].toBool());
            }

            for (int i = 0; i < zCamosArray.size(); ++i) {
                weaponCamos[1].append(zCamosArray[i].toObject()["Status"].toBool());
            }

            classCamos.append(weaponCamos);
        }

        camoStatus.append(classCamos);
    }

    // logic
    if (autoUnlockMasteryCamo) {
        // old cod logic
        for (int camoMode = 0; camoMode < 2; ++camoMode) {
            // gold
            for (QVector<QVector<QVector<bool>>> &classVector : camoStatus) {
                for (QVector<QVector<bool>> &weaponVector : classVector) {
                    bool goldAllowedForWeapon = true;
                    for (int i = 0; i < standardCamoAmount; ++i) {
                        if (!weaponVector[camoMode][i]) {
                            goldAllowedForWeapon = false;
                        }
                    }
                    weaponVector[camoMode][goldPos] = goldAllowedForWeapon;
                }
            }

            // diamond
            int index = 0;
            for (QVector<QVector<QVector<bool>>> &classVector : camoStatus) {
                int goldForDiamond = 0;
                for (QVector<QVector<bool>> &weaponVector : classVector) {
                    if (weaponVector[camoMode][goldPos]) {
                        ++goldForDiamond;
                    }
                }

                if (goldForDiamond >= standardWeaponAmount[index]) {
                    for (QVector<QVector<bool>> &weaponVector : classVector) {
                        weaponVector[camoMode][diamondPos] = weaponVector[camoMode][goldPos];
                    }
                }
                ++index;
            }

            // dark matter
            int diamondClassesUnlocked = 0;
            for (QVector<QVector<QVector<bool>>> &classVector : camoStatus) {
                for (QVector<QVector<bool>> &weaponVector : classVector) {
                    if (weaponVector[camoMode][diamondPos]) {
                        ++diamondClassesUnlocked;
                        break;
                    }
                }
            }

            if (diamondClassesUnlocked >= standardWeaponAmount.size()) {
                for (QVector<QVector<QVector<bool>>> &classVector : camoStatus) {
                    for (QVector<QVector<bool>> &weaponVector : classVector) {
                        weaponVector[camoMode][darkMatterPos] = weaponVector[camoMode][diamondPos];
                    }
                }
            }
        }
    } else {
        // new cod logic

        for (int camoMode = 0; camoMode < 2; ++camoMode) {
            // gold
            for (QVector<QVector<QVector<bool>>> &classVector : camoStatus) {
                for (QVector<QVector<bool>> &weaponVector : classVector) {
                    bool goldAllowedForWeapon = true;
                    for (int i = 0; i < standardCamoAmount; ++i) {
                        if (!weaponVector[camoMode][i]) {
                            goldAllowedForWeapon = false;
                        }
                    }
                    if (!goldAllowedForWeapon) {
                        weaponVector[camoMode][goldPos] = false;
                    }
                }
            }

            // diamond
            int diamondIndex = 0;
            int goldInClass = 0;
            for (QVector<QVector<QVector<bool>>> &classVector : camoStatus) {
                goldInClass = 0;
                for (QVector<QVector<bool>> &weaponVector : classVector) {
                    if (weaponVector[camoMode][goldPos]) {
                        ++goldInClass;
                    }
                }

                for (QVector<QVector<bool>> &weaponVector : classVector) {
                    if (goldInClass >= standardWeaponAmount[diamondIndex]) {
                        if (!weaponVector[camoMode][goldPos]) {
                            weaponVector[camoMode][diamondPos] = false;
                        }
                    } else {
                        weaponVector[camoMode][diamondPos] = false;
                    }
                }
                ++diamondIndex;
            }

            // polyatomic
            int diamondInAllClasses = 0;
            for (QVector<QVector<QVector<bool>>> &classVector : camoStatus) {
                for (QVector<QVector<bool>> &weaponVector: classVector) {
                    if (weaponVector[camoMode][diamondPos]) {
                        ++diamondInAllClasses;
                    }
                }
            }
            for (QVector<QVector<QVector<bool>>> &classVector : camoStatus) {
                for (QVector<QVector<bool>> &weaponVector: classVector) {
                    if (diamondInAllClasses >= totalStandardWeaponAmount) {
                        if (!weaponVector[camoMode][diamondPos]) {
                            weaponVector[camoMode][polyatomicPos] = false;
                        }
                    } else {
                        weaponVector[camoMode][polyatomicPos] = false;
                    }
                }
            }

            // dark Matter
            int polyatomicInAllClasses = 0;
            for (QVector<QVector<QVector<bool>>> &classVector : camoStatus) {
                for (QVector<QVector<bool>> &weaponVector: classVector) {
                    if (weaponVector[camoMode][polyatomicPos]) {
                        ++polyatomicInAllClasses;
                    }
                }
            }

            if (polyatomicInAllClasses >= totalStandardWeaponAmount) {
                for (QVector<QVector<QVector<bool>>> &classVector : camoStatus) {
                    for (QVector<QVector<bool>> &weaponVector : classVector) {
                        weaponVector[camoMode][darkMatterPos] = weaponVector[camoMode][polyatomicPos];
                    }
                }
            }
        }
    }

    QFile writeFile(fileName);
    if (!writeFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Unable to open file for writing.";
        return;
    }

    QJsonObject updatedWeaponData;
    QJsonArray updatedWeaponClassesArray;

    for (int classIndex = 0; classIndex < camoStatus.size(); ++classIndex) {
        QJsonObject classObject;
        QJsonArray weaponsArray;

        for (int weaponIndex = 0; weaponIndex < camoStatus[classIndex].size(); ++weaponIndex) {
            QJsonObject weaponObject;
            QJsonArray mCamosArray;
            QJsonArray zCamosArray;

            for (int i = 0; i < camoStatus[classIndex][weaponIndex][0].size(); ++i) {
                QJsonObject camoObject;
                camoObject["Condition"] = QString("todo for Camo");  // Maintain the condition as placeholder
                switch (i) {
                    case goldPos:
                        camoObject["Name"] = "M_" + goldName;
                        break;
                    case diamondPos:
                        camoObject["Name"] = "M_" + diamondName;
                        break;
                    case polyatomicPos:
                        camoObject["Name"] = "M_" + polyatomicName;
                        break;
                    case darkMatterPos:
                        camoObject["Name"] = "M_" + darkMatterName;
                        break;
                    default:
                        camoObject["Name"] = QString("M_Camo%1").arg(i + 1); // Maintain the names as placeholders
                        break;
                }
                camoObject["Status"] = camoStatus[classIndex][weaponIndex][0][i];
                mCamosArray.append(camoObject);
            }

            for (int i = 0; i < camoStatus[classIndex][weaponIndex][1].size(); ++i) {
                QJsonObject camoObject;
                camoObject["Condition"] = QString("todo for Camo");  // Maintain the condition as placeholder
                switch (i) {
                    case goldPos:
                        camoObject["Name"] = "Z_" + goldName;
                        break;
                    case diamondPos:
                        camoObject["Name"] = "Z_" + diamondName;
                        break;
                    case polyatomicPos:
                        camoObject["Name"] = "Z_" + polyatomicName;
                        break;
                    case darkMatterPos:
                        camoObject["Name"] = "Z_" + darkMatterName;
                        break;
                    default:
                        camoObject["Name"] = QString("Z_Camo%1").arg(i + 1); // Maintain the names as placeholders
                        break;
                }
                camoObject["Status"] = camoStatus[classIndex][weaponIndex][1][i];
                zCamosArray.append(camoObject);
            }

            weaponObject["M_CAMOS"] = mCamosArray;
            weaponObject["Z_CAMOS"] = zCamosArray;

            // Ensure other weapon details are preserved
            QJsonObject originalWeaponObject = weaponData["WeaponClasses"].toArray()[classIndex].toObject()["Weapons"].toArray()[weaponIndex].toObject();
            weaponObject["Name"] = originalWeaponObject["Name"].toString();
            weaponObject["MaxedLevel"] = originalWeaponObject["MaxedLevel"].toBool();
            weaponObject["Unlocked"] = originalWeaponObject["Unlocked"].toBool();

            weaponsArray.append(weaponObject);
        }

        classObject["Class"] = weaponData["WeaponClasses"].toArray()[classIndex].toObject()["Class"].toString();
        classObject["Weapons"] = weaponsArray;
        updatedWeaponClassesArray.append(classObject);
    }

    updatedWeaponData["WeaponClasses"] = updatedWeaponClassesArray;
    QJsonDocument updatedDoc(updatedWeaponData);
    writeFile.write(updatedDoc.toJson());
    writeFile.close();
}

void WeaponEditor::updateStyles() {
    // Stylesheet für die ComboBoxen
    QString comboBoxStyle =
            "QComboBox {"
            "   border: 2px solid #444444;"
            "   border-radius: 10px;"
            "   padding: 5px 10px;"
            "   color: #ffffff;"
            "   font-size: 14px;"
            "   min-width: 6em;"
            "}"
            "QComboBox::drop-down {"
            "   subcontrol-origin: padding;"
            "   subcontrol-position: top right;"
            "   width: 20px;"
            "   border-left-width: 1px;"
            "   border-left-color: #444444;"
            "   border-left-style: solid;"
            "   border-top-right-radius: 10px;"
            "   border-bottom-right-radius: 10px;"
            "}"
            "QComboBox::down-arrow {"
            "   image: url(C:/Qt/Tools/QtDesignStudio/share/doc/qtcreator/qtdesignstudio/images/arrowdown.png);"
            "   width: 10px;"
            "   height: 10px;"
            "}"
            "QComboBox QAbstractItemView {"
            "   border: 2px solid #444444;"
            "   border-radius: 10px;"
            "   background-color: #333333;"
            "   color: #ffffff;"
            "   selection-background-color: #555555;"
            "   selection-color: #ffffff;"
            "   padding: 5px;"
            "   outline: none;"
            "   margin: 0;"
            "}"
            "QComboBox QAbstractItemView::item {"
            "   border-radius: 5px;"
            "   padding: 5px;"
            "   margin: 2px;"
            "}";

    // Anwenden des Stylesheets auf alle ComboBoxen
    for (auto comboBox : comboBoxes) {
        comboBox->setStyleSheet(comboBoxStyle);
    }

    // Stylesheet für die Statuslabels
    QString statusLabelStyle =
            "QLabel {"
            "   border: 2px solid #444444;"
            "   border-radius: 10px;"
            "   background-color: #333333;"
            "   color: #ffffff;"
            "   padding: 10px;"
            "   text-align: center;"
            "}";

    // Anwenden des Stylesheets auf alle Statuslabels
    for (auto statusLabel : statusLabels) {
        statusLabel->setStyleSheet(statusLabelStyle);
    }

    // Stylesheet für das weaponContainer-Widget und die Scrollbars
    QString weaponContainerStyle = R"(
        QWidget {
            background-color: #272626;
            border: 2px solid #444444;
            border-radius: 10px;
        }
    )";

    weaponContainer->setStyleSheet(weaponContainerStyle);

    QString scrollbarStyle = R"(
    QScrollArea {
        border: none;
        background: transparent;
    }

    QScrollBar:vertical, QScrollBar:horizontal {
        border: none;
        background: none;
        width: 0px;
        height: 0px;
        margin: 0px;
        padding: 0px;
    }

    QScrollBar::handle:vertical, QScrollBar::handle:horizontal {
        background: none;
        border-radius: 0px;
    }

    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical,
    QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
        background: none;
        width: 0px;
        height: 0px;
    }

    QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical,
    QScrollBar::left-arrow:horizontal, QScrollBar::right-arrow:horizontal {
        background: none;
        width: 0px;
        height: 0px;
    }
)";

    scrollArea->setStyleSheet(scrollbarStyle);
}