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
    connect(classComboBox, &QComboBox::currentTextChanged, this, &WeaponEditor::updateWeaponClass);
    mainLayout->addWidget(classComboBox);

    // Add camo type selection ComboBox
    camoTypeComboBox = new QComboBox();
    camoTypeComboBox->addItem("Multiplayer");
    camoTypeComboBox->addItem("Zombies");
    camoTypeComboBox->setCurrentText("Multiplayer");  // Set default selection
    connect(camoTypeComboBox, &QComboBox::currentTextChanged, this, &WeaponEditor::updateWeaponClass);
    mainLayout->addWidget(camoTypeComboBox);

    // Status label
    statusLabel = new QLabel();
    mainLayout->addWidget(statusLabel);

    // Scroll area for displaying weapon tiles
    QScrollArea *scrollArea = new QScrollArea();
    weaponContainer = new QWidget();
    weaponLayout = new QGridLayout(); // Changed to QGridLayout
    weaponContainer->setLayout(weaponLayout);
    scrollArea->setWidget(weaponContainer);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // Initial update
    updateWeaponClass();
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
                    goldCamoSelectedClassWeapons++;
                }
            }

            if (weaponClassName == selectedClass) {
                totalClassWeapons++;
            }
        }
    }

    if (totalWeapons > 0) {
        double unlockedPercentage = (static_cast<double>(unlockedWeapons) / totalWeapons) * 100;
        double selectedGoldCamoPercentage = (static_cast<double>(goldCamoWeapons) / totalWeapons) * 100;
        double selectedClassGoldCamoPercentage = (totalClassWeapons > 0) ?
                                                 (static_cast<double>(goldCamoSelectedClassWeapons) / totalClassWeapons) * 100 : 0.0;

        QString camoTypeText = (camoTypeComboBox->currentText() == "Multiplayer") ? "Multiplayer" : "Zombies";
        QString classNameText = selectedClass.isEmpty() ? "Not Selected" : selectedClass;

        statusLabel->setText(QString("Global Unlocked Weapons: %1%\n"
                                     "%2 Dark-Matter Status: %3%\n"
                                     "%2 %4 Diamond Status: %5%")
                                     .arg(QString::number(unlockedPercentage, 'f', 2))
                                     .arg(camoTypeText)
                                     .arg(QString::number(selectedGoldCamoPercentage, 'f', 2))
                                     .arg(classNameText)
                                     .arg(QString::number(selectedClassGoldCamoPercentage, 'f', 2)));
    } else {
        statusLabel->setText("No weapons found.");
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
    tile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // Expanding für flexibles Layout

    // Erstelle ein Layout für den Tile
    QVBoxLayout *tileLayout = new QVBoxLayout(tile);
    tileLayout->setContentsMargins(0, 0, 0, 0); // Keine Margen
    tileLayout->setSpacing(0); // Kein Abstand zwischen Widgets

    // Erstelle ein Widget für den Inhalt des Tiles
    QWidget *contentWidget = new QWidget();
    QFormLayout *contentLayout = new QFormLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0); // Keine Margen

    // Display weapon name
    QLabel *nameLabel = new QLabel("Weapon Name:");
    QLineEdit *nameEdit = new QLineEdit(weaponName);
    nameEdit->setReadOnly(true);
    contentLayout->addRow(nameLabel, nameEdit);

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
    int currentLevel = 0;
    int maxLevel = 100;
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
                    currentLevel = weaponObject["CurrentLevel"].toInt();
                    maxLevel = weaponObject["MaxLevel"].toInt();
                    mCamosArray = weaponObject["M_CAMOS"].toArray();
                    zCamosArray = weaponObject["Z_CAMOS"].toArray();
                }
                break;
            }
        }
    }

    // Display unlocked status
    QLabel *unlockedLabel = new QLabel("Unlocked:");
    QCheckBox *unlockedCheckBox = new QCheckBox();
    unlockedCheckBox->setChecked(unlocked);
    contentLayout->addRow(unlockedLabel, unlockedCheckBox);

    // Display current level
    QLabel *levelLabel = new QLabel("Current Level:");
    QLineEdit *levelEdit = new QLineEdit(QString::number(currentLevel));
    contentLayout->addRow(levelLabel, levelEdit);

    // Display max level
    QLabel *maxLevelLabel = new QLabel("Max Level:");
    QLineEdit *maxLevelEdit = new QLineEdit(QString::number(maxLevel));
    contentLayout->addRow(maxLevelLabel, maxLevelEdit);

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
    for (int i = 0; i < totalCamos; ++i) {
        QJsonObject camoObject = camosToShow[i].toObject();
        QString camoName = camoObject["Name"].toString();
        bool camoStatus = camoObject["Status"].toBool();

        QLabel *camoLabel = new QLabel(camoName);
        QCheckBox *camoCheckBox = new QCheckBox();
        camoCheckBox->setChecked(camoStatus);
        contentLayout->addRow(camoLabel, camoCheckBox);

        connect(camoCheckBox, &QCheckBox::stateChanged, [this, weaponName, camoName](int state) {
            liveUpdateWeaponData(weaponName, camoName, (state == Qt::Checked));
        });
    }

    // Connect unlocked status change to liveUpdateWeaponData
    connect(unlockedCheckBox, &QCheckBox::stateChanged, [this, weaponName](int state) {
        liveUpdateWeaponData(weaponName, "Unlocked", (state == Qt::Checked));
    });

    // Connect current level change to liveUpdateWeaponData
    connect(levelEdit, &QLineEdit::textChanged, [this, weaponName](const QString &text) {
        bool ok;
        int level = text.toInt(&ok);
        if (ok) {
            liveUpdateWeaponData(weaponName, "CurrentLevel", level);
        }
    });

    // Connect max level change to liveUpdateWeaponData
    connect(maxLevelEdit, &QLineEdit::textChanged, [this, weaponName](const QString &text) {
        bool ok;
        int level = text.toInt(&ok);
        if (ok) {
            liveUpdateWeaponData(weaponName, "MaxLevel", level);
        }
    });

    // Füge das contentWidget zum Tile hinzu
    tileLayout->addWidget(contentWidget);

    QString backgroundColor = "transparent"; // Standardfarbe
    for (const QJsonValue &camoValue : camosToShow) {
        QJsonObject camoObject = camoValue.toObject();
        QString camoName = camoObject["Name"].toString();
        bool camoStatus = camoObject["Status"].toBool();

        if (camoStatus) {
            if (camoName.contains("GOLD")) {
                backgroundColor = "#8a8101"; // Gold-Farbe
            } else if (camoName.contains("DIAMOND")) {
                backgroundColor = "#001b47"; // Diamant-Farbe
            } else if (camoName.contains("DARK_MATTER")) {
                backgroundColor = "#2c0133"; // Dark Matter-Farbe
            }
        }
    }
    tile->setStyleSheet(QString("background-color: %1;").arg(backgroundColor)); // Hintergrundfarbe setzen

    // Füge das Tile zu einem Layout hinzu, z.B. einem GridLayout (angenommen, weaponLayout ist ein QGridLayout)
    weaponLayout->addWidget(tile, index / 3, index % 3); // Ändert sich auf 3 Tiles pro Reihe
}

void WeaponEditor::liveUpdateWeaponData(const QString &weaponName, const QString &key, const QVariant &value) {
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

    // Bearbeiten der Waffendaten
    for (int i = 0; i < weaponClassesArray.size(); ++i) {
        QJsonObject classObject = weaponClassesArray[i].toObject();
        QJsonArray weaponsArray = classObject["Weapons"].toArray();

        for (int j = 0; j < weaponsArray.size(); ++j) {
            QJsonObject weaponObject = weaponsArray[j].toObject();
            if (weaponObject["Name"].toString() == weaponName) {
                // Prüfen, ob der Schlüssel zu M_CAMOS oder Z_CAMOS gehört
                if (key.startsWith("M_")) {
                    QJsonArray mCamosArray = weaponObject["M_CAMOS"].toArray();
                    for (int k = 0; k < mCamosArray.size(); ++k) {
                        QJsonObject camoObject = mCamosArray[k].toObject();
                        if (camoObject["Name"].toString() == key) {
                            camoObject["Status"] = QJsonValue::fromVariant(value);
                            mCamosArray[k] = camoObject;
                            break;
                        }
                    }
                    weaponObject["M_CAMOS"] = mCamosArray;
                } else if (key.startsWith("Z_")) {
                    QJsonArray zCamosArray = weaponObject["Z_CAMOS"].toArray();
                    for (int k = 0; k < zCamosArray.size(); ++k) {
                        QJsonObject camoObject = zCamosArray[k].toObject();
                        if (camoObject["Name"].toString() == key) {
                            camoObject["Status"] = QJsonValue::fromVariant(value);
                            zCamosArray[k] = camoObject;
                            break;
                        }
                    }
                    weaponObject["Z_CAMOS"] = zCamosArray;
                } else {
                    weaponObject[key] = QJsonValue::fromVariant(value);
                }

                weaponsArray[j] = weaponObject;
                break;
            }
        }
        classObject["Weapons"] = weaponsArray;
        weaponClassesArray[i] = classObject;
    }
    weaponData["WeaponClasses"] = weaponClassesArray;

    QJsonDocument updatedDoc(weaponData);
    QFile writeFile(fileName);
    if (!writeFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Unable to open file for writing.";
        return;
    }
    writeFile.write(updatedDoc.toJson());
    writeFile.close();
    camoLogic();
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

    // Definieren Sie die Camos für Multiplayer und Zombies
    QStringList mCamos = {
            "M_Camo1", "M_Camo2", "M_Camo3", "M_Camo4", "M_GOLD", "M_DIAMOND", "M_DARK_MATTER"
    };
    QStringList zCamos = {
            "Z_Camo1", "Z_Camo2", "Z_Camo3", "Z_Camo4", "Z_GOLD", "Z_DIAMOND", "Z_DARK_MATTER"
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
            weaponObject["CurrentLevel"] = 0;
            weaponObject["MaxLevel"] = 100;

            // Fügen Sie Multiplayer Camos hinzu
            QJsonArray mCamosArray;
            for (const QString &camoName : mCamos) {
                QJsonObject camoObject;
                camoObject["Name"] = camoName;
                camoObject["Status"] = false;
                mCamosArray.append(camoObject);
            }
            weaponObject["M_CAMOS"] = mCamosArray;

            // Fügen Sie Zombies Camos hinzu
            QJsonArray zCamosArray;
            for (const QString &camoName : zCamos) {
                QJsonObject camoObject;
                camoObject["Name"] = camoName;
                camoObject["Status"] = false;
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

            bool allMultiplayerCamosUnlocked = true;
            bool allZombiesCamosUnlocked = true;

            // Überprüfe alle Multiplayer-Tarnungen (außer Gold, Diamond, Dark Matter)
            for (int k = 0; k < 4; ++k) { // Die ersten 4 Tarnungen sind die regulären Tarnungen
                QJsonObject camoObject = mCamosArray[k].toObject();
                if (!camoObject["Status"].toBool()) {
                    allMultiplayerCamosUnlocked = false;
                }
            }

            // Überprüfe alle Zombies-Tarnungen (außer Gold, Diamond, Dark Matter)
            for (int k = 0; k < 4; ++k) {
                QJsonObject camoObject = zCamosArray[k].toObject();
                if (!camoObject["Status"].toBool()) {
                    allZombiesCamosUnlocked = false;
                }
            }

            // Setze Gold, wenn alle 4 Tarnungen freigeschaltet sind
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
