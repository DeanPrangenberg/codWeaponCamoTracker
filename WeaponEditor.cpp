#include "WeaponEditor.h"
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
    classComboBox->setCurrentText("Assault Rifles");  // Set default selection
    comboBoxes.push_back(classComboBox);
    connect(classComboBox, &QComboBox::currentTextChanged, this, &WeaponEditor::updateWeaponClass);
    mainLayout->addWidget(classComboBox);

    // Add camo type selection ComboBox
    camoTypeComboBox = new QComboBox();
    camoTypeComboBox->addItem("Multiplayer");
    camoTypeComboBox->addItem("Zombies");
    camoTypeComboBox->setCurrentText("Multiplayer");  // Set default selection
    comboBoxes.push_back(camoTypeComboBox);
    connect(camoTypeComboBox, &QComboBox::currentTextChanged, this, &WeaponEditor::updateWeaponClass);
    mainLayout->addWidget(camoTypeComboBox);

    // Status labels
    QHBoxLayout *statusLayout = new QHBoxLayout();

    globalUnlockedWeaponsLabel = new QLabel();
    globalMaxedLevelWeaponsLabel = new QLabel();
    darkMatterStatusLabel = new QLabel();
    diamondStatusLabel = new QLabel();

    // Add labels to statusLabels vector
    statusLabels.push_back(globalUnlockedWeaponsLabel);
    statusLabels.push_back(globalMaxedLevelWeaponsLabel);
    statusLabels.push_back(darkMatterStatusLabel);
    statusLabels.push_back(diamondStatusLabel);

    // Set object names for labels
    globalUnlockedWeaponsLabel->setObjectName("globalUnlockedWeaponsLabel");
    globalMaxedLevelWeaponsLabel->setObjectName("globalMaxedLevelWeaponsLabel");
    darkMatterStatusLabel->setObjectName("darkMatterStatusLabel");
    diamondStatusLabel->setObjectName("diamondStatusLabel");

    // Add labels to status layout
    statusLayout->addWidget(globalUnlockedWeaponsLabel);
    statusLayout->addWidget(globalMaxedLevelWeaponsLabel);
    statusLayout->addWidget(darkMatterStatusLabel);
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
    int goldCamoWeapons = 0;
    int goldCamoSelectedClassWeapons = 0;
    int totalClassWeapons = 0;
    int maxedLevelWeapons = 0;

    for (const QJsonValue &classValue : weaponClassesArray) {
        QJsonObject classObject = classValue.toObject();
        QString weaponClassName = classObject["Class"].toString();
        QJsonArray weaponsArray = classObject["Weapons"].toArray();

        for (const QJsonValue &weaponValue : weaponsArray) {
            QJsonObject weaponObject = weaponValue.toObject();
            totalWeapons++;

            bool isUnlocked = weaponObject["Unlocked"].toBool();
            bool maxLevel = weaponObject["MaxedLevel"].toBool();

            if (isUnlocked) unlockedWeapons++;
            if (maxLevel) maxedLevelWeapons++;

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
                if (weaponClassName == selectedClass) goldCamoSelectedClassWeapons++;
            }

            if (weaponClassName == selectedClass) totalClassWeapons++;
        }
    }

    if (totalWeapons > 0) {
        double unlockedPercentage = (static_cast<double>(unlockedWeapons) / totalWeapons) * 100;
        double maxedLevelPercentage = (static_cast<double>(maxedLevelWeapons) / totalWeapons) * 100;
        double selectedGoldCamoPercentage = (static_cast<double>(goldCamoWeapons) / totalWeapons) * 100;
        double selectedClassGoldCamoPercentage = (totalClassWeapons > 0) ?
                                                 (static_cast<double>(goldCamoSelectedClassWeapons) / totalClassWeapons) * 100 : 0.0;

        QString camoTypeText = (camoTypeComboBox->currentText() == "Multiplayer") ? "Multiplayer" : "Zombies";
        QString classNameText = selectedClass.isEmpty() ? "Not Selected" : selectedClass;

        if (statusLabels.size() == 4) {
            statusLabels[0]->setText(QString("Global Unlocked Weapons:\n%1%").arg(QString::number(unlockedPercentage, 'f', 2)));
            statusLabels[1]->setText(QString("Global Maxed Level Weapons:\n%1%").arg(QString::number(maxedLevelPercentage, 'f', 2)));
            statusLabels[2]->setText(QString("%1 Dark-Matter Status:\n%2%").arg(camoTypeText).arg(QString::number(selectedGoldCamoPercentage, 'f', 2)));
            statusLabels[3]->setText(QString("%1 %2 Diamond Status:\n%3%").arg(classNameText).arg(camoTypeText).arg(QString::number(selectedClassGoldCamoPercentage, 'f', 2)));
        }
    } else {
        if (statusLabels.size() == 4) {
            statusLabels[0]->setText("No weapons found.");
            statusLabels[1]->setText("No weapons found.");
            statusLabels[2]->setText("No weapons found.");
            statusLabels[3]->setText("No weapons found.");
        }
    }
}

void WeaponEditor::initializeWeaponData() {
    // Initialize weapon data for demo purposes
    weaponMap["Assault Rifles"] = QStringList{
            "FTAC Light", "Kastov 74", "Krig C", "M16", "M4-S", "R-2", "SA87"
    };
    weaponMap["SMGs"] = QStringList{
            "AMR9", "FMG9", "Lachmann Sub", "MD-97L", "Raptor-9", "VAP 2", "VAP 3", "VCAR"
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
    tile->setFixedHeight(295);
    tile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    tile->setStyleSheet(R"(
        QWidget {
            background-color: #333333;
            }
        )"
    );

    QVBoxLayout *tileLayout = new QVBoxLayout(tile);
    tileLayout->setContentsMargins(10, 10, 10, 10); // Margen nach Wunsch anpassen

    // Erstelle ein Widget für die Hintergrundfarbe oben im Tile
    QWidget *colorHeaderWidget = new QWidget();
    QHBoxLayout *colorHeaderLayout = new QHBoxLayout(colorHeaderWidget);
    colorHeaderLayout->setContentsMargins(0, 0, 0, 0); // Keine Margen
    colorHeaderWidget->setStyleSheet("border: none;");

    QLabel *statusLabel = new QLabel("Camo Status:");
    QLabel *colorLabel = new QLabel();
    colorLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    colorHeaderLayout->addWidget(statusLabel);
    colorHeaderLayout->addWidget(colorLabel);

    tileLayout->addWidget(colorHeaderWidget); // Füge das Farb-Widget zum Tile hinzu

    // Erstelle ein Widget für den Inhalt des Tiles
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0); // Keine Margen
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
                    maxedLevel = weaponObject["MaxedLevel"].toBool(); // Neues Feld
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
            "QCheckBox::indicator:unchecked {"
            "   border: 2px solid #555555;"
            "   border-radius: 5px;"
            "   background-color: #8f0e00;"
            "}"
            "QCheckBox::indicator:checked {"
            "   border: 2px solid #555555;"
            "   border-radius: 5px;"
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
            "QCheckBox::indicator:unchecked {"
            "   border: 2px solid #555555;"
            "   border-radius: 5px;"
            "   background-color: #8f0e00;"
            "}"
            "QCheckBox::indicator:checked {"
            "   border: 2px solid #555555;"
            "   border-radius: 5px;"
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

    int totalCamos = camosToShow.size();
    QVBoxLayout *camoVBoxLayout = new QVBoxLayout();

    int displayedCamos = std::max(0, totalCamos - autoUnlockCamoAmount); // Display all except last 3

    for (int i = 0; i < displayedCamos; ++i) {
        QJsonObject camoObject = camosToShow[i].toObject();
        QString camoName = camoObject["Name"].toString();
        bool camoStatus = camoObject["Status"].toBool();
        QString camoCondition = camoObject["Condition"].toString();

        QHBoxLayout *camoLayout = new QHBoxLayout();
        camoLayout->setContentsMargins(10, 0, 0, 0); // Margen (oben, links, unten, rechts) erhöhen

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
                "QCheckBox::indicator:unchecked {"
                "   border: 2px solid #555555;"
                "   border-radius: 5px;"
                "   background-color: #8f0e00;"
                "}"
                "QCheckBox::indicator:checked {"
                "   border: 2px solid #555555;"
                "   border-radius: 5px;"
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
        camoVBoxLayout->setSpacing(5); // Verhindert zusätzlichen Abstand zwischen den Camo-Widgets
        camoVBoxLayout->setContentsMargins(0, 0, 0, 0); // Keine Margen

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
            if (camoName.contains("GOLD")) {
                colorHeaderBackgroundColor = "#ffc400"; // Gold-Farbe
            } else if (camoName.contains("DIAMOND")) {
                colorHeaderBackgroundColor = "#4a7fe8"; // Diamant-Farbe
            } else if (camoName.contains("DARK_MATTER")) {
                colorHeaderBackgroundColor = "#6b005f"; // Dark Matter-Farbe
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

    bool allClassesGoldMultiplayer = true;
    bool allClassesGoldZombies = true;

    // Schleife durch alle Klassen (z.B. Assault Rifles, SMGs, etc.)
    for (int i = 0; i < weaponClassesArray.size(); ++i) {
        QJsonObject classObject = weaponClassesArray[i].toObject();
        QJsonArray weaponsArray = classObject["Weapons"].toArray();

        bool classGoldMultiplayer = true;
        bool classGoldZombies = true;

        // Schleife durch alle Waffen in dieser Klasse
        for (int j = 0; j < weaponsArray.size(); ++j) {
            QJsonObject weaponObject = weaponsArray[j].toObject();
            QJsonArray mCamosArray = weaponObject["M_CAMOS"].toArray();
            QJsonArray zCamosArray = weaponObject["Z_CAMOS"].toArray();

            // Setze Mastery-Camos (Gold, Diamond, Dark Matter) auf false
            for (int k = 4; k < 7; ++k) {
                QJsonObject camoMObject = mCamosArray[k].toObject();
                QJsonObject camoZObject = zCamosArray[k].toObject();
                camoMObject["Status"] = false;
                camoZObject["Status"] = false;
                mCamosArray[k] = camoMObject;
                zCamosArray[k] = camoZObject;
            }

            weaponObject["M_CAMOS"] = mCamosArray;
            weaponObject["Z_CAMOS"] = zCamosArray;
            weaponsArray[j] = weaponObject;

            bool allMultiplayerCamosUnlocked = true;
            bool allZombiesCamosUnlocked = true;

            // Überprüfe alle regulären Multiplayer-Tarnungen (1-4)
            for (int k = 0; k < 4; ++k) {
                QJsonObject camoObject = mCamosArray[k].toObject();
                if (!camoObject["Status"].toBool()) {
                    allMultiplayerCamosUnlocked = false;
                }
            }

            // Überprüfe alle regulären Zombies-Tarnungen (1-4)
            for (int k = 0; k < 4; ++k) {
                QJsonObject camoObject = zCamosArray[k].toObject();
                if (!camoObject["Status"].toBool()) {
                    allZombiesCamosUnlocked = false;
                }
            }

            // Setze Gold, wenn alle regulären Tarnungen freigeschaltet sind
            if (allMultiplayerCamosUnlocked) {
                QJsonObject goldCamo = mCamosArray[4].toObject(); // Gold Tarnung
                goldCamo["Status"] = true;
                mCamosArray[4] = goldCamo;
            } else {
                classGoldMultiplayer = false;
            }

            if (allZombiesCamosUnlocked) {
                QJsonObject goldCamo = zCamosArray[4].toObject();
                goldCamo["Status"] = true;
                zCamosArray[4] = goldCamo;
            } else {
                classGoldZombies = false;
            }

            weaponObject["M_CAMOS"] = mCamosArray;
            weaponObject["Z_CAMOS"] = zCamosArray;
            weaponsArray[j] = weaponObject;
        }

        // Setze Diamond für die Klasse, wenn alle Waffen in dieser Klasse Gold haben
        if (classGoldMultiplayer) {
            for (int j = 0; j < weaponsArray.size(); ++j) {
                QJsonObject weaponObject = weaponsArray[j].toObject();
                QJsonArray mCamosArray = weaponObject["M_CAMOS"].toArray();

                QJsonObject diamondCamo = mCamosArray[5].toObject(); // Diamond Tarnung
                diamondCamo["Status"] = true;
                mCamosArray[5] = diamondCamo;

                weaponObject["M_CAMOS"] = mCamosArray;
                weaponsArray[j] = weaponObject;
            }
        } else {
            allClassesGoldMultiplayer = false;
        }

        if (classGoldZombies) {
            for (int j = 0; j < weaponsArray.size(); ++j) {
                QJsonObject weaponObject = weaponsArray[j].toObject();
                QJsonArray zCamosArray = weaponObject["Z_CAMOS"].toArray();

                QJsonObject diamondCamo = zCamosArray[5].toObject();
                diamondCamo["Status"] = true;
                zCamosArray[5] = diamondCamo;

                weaponObject["Z_CAMOS"] = zCamosArray;
                weaponsArray[j] = weaponObject;
            }
        } else {
            allClassesGoldZombies = false;
        }

        classObject["Weapons"] = weaponsArray;
        weaponClassesArray[i] = classObject;
    }

    // Setze Dark Matter, wenn alle Klassen Diamond haben
    if (allClassesGoldMultiplayer) {
        for (int i = 0; i < weaponClassesArray.size(); ++i) {
            QJsonObject classObject = weaponClassesArray[i].toObject();
            QJsonArray weaponsArray = classObject["Weapons"].toArray();

            for (int j = 0; j < weaponsArray.size(); ++j) {
                QJsonObject weaponObject = weaponsArray[j].toObject();
                QJsonArray mCamosArray = weaponObject["M_CAMOS"].toArray();

                QJsonObject darkMatterCamo = mCamosArray[6].toObject(); // Dark Matter Tarnung
                darkMatterCamo["Status"] = true;
                mCamosArray[6] = darkMatterCamo;

                weaponObject["M_CAMOS"] = mCamosArray;
                weaponsArray[j] = weaponObject;
            }

            classObject["Weapons"] = weaponsArray;
            weaponClassesArray[i] = classObject;
        }
    }

    if (allClassesGoldZombies) {
        for (int i = 0; i < weaponClassesArray.size(); ++i) {
            QJsonObject classObject = weaponClassesArray[i].toObject();
            QJsonArray weaponsArray = classObject["Weapons"].toArray();

            for (int j = 0; j < weaponsArray.size(); ++j) {
                QJsonObject weaponObject = weaponsArray[j].toObject();
                QJsonArray zCamosArray = weaponObject["Z_CAMOS"].toArray();

                QJsonObject darkMatterCamo = zCamosArray[6].toObject();
                darkMatterCamo["Status"] = true;
                zCamosArray[6] = darkMatterCamo;

                weaponObject["Z_CAMOS"] = zCamosArray;
                weaponsArray[j] = weaponObject;
            }

            classObject["Weapons"] = weaponsArray;
            weaponClassesArray[i] = classObject;
        }
    }

    // Aktualisiere das JSON-Dokument
    weaponData["WeaponClasses"] = weaponClassesArray;
    QJsonDocument updatedDoc(weaponData);

    QFile writeFile(fileName);
    if (!writeFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Unable to open file for writing.";
        return;
    }
    writeFile.write(updatedDoc.toJson());
    writeFile.close();
    updateStatus();
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
            "   min-width: 200px;"
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
