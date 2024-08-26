#include "WeaponEditor.h"

void WeaponEditor::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *logoWidget = new QWidget(this);
    logoWidget->setStyleSheet(QString("QWidget {"
                                      "   border: 2px solid #444444;"
                                      "   border-radius: 10px;"
                                      "   background-color: #2d2d2d;"
                                      "   color: #ffffff;"
                                      "}"));
    logoWidget->setFixedHeight(45);

    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->setContentsMargins(0, 0, 0, 0); // Entfernen Sie die Standard-Ränder

    QLabel *logoLabel = new QLabel(logoWidget);
    logoLabel->setFixedSize(200, 40);
    logoLabel->setScaledContents(true);
    QPixmap logoPixmap("logoBig.png");
    logoLabel->setPixmap(logoPixmap);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setStyleSheet("QLabel { border: none; }");

    logoLayout->addStretch();
    logoLayout->addWidget(logoLabel);
    logoLayout->addStretch();

    logoWidget->setLayout(logoLayout);

    mainLayout->addWidget(logoWidget);


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
            "   background-color: #2d2d2d;"
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
            background-color: #2a2a2a;
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

void WeaponEditor::createWeaponTile(const QString &weaponName, int index, int rowSize) {
    // Erstelle ein Widget für den Tile
    QWidget *tile = new QWidget();
    if (autoUnlockMasteryCamo) {
        tile->setFixedHeight(315 - 20);
    } else {
        tile->setFixedHeight(315);
    }

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
            "   background-color: #2d2d2d;"
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
        camoLineEdit->setToolTip(camoCondition);
        camoLineEdit->setAlignment(Qt::AlignCenter);
        camoLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        camoLineEdit->setStyleSheet(
                "QLineEdit {"
                "   border: 2px solid #444444;"
                "   border-radius: 10px;"
                "   background-color: #2d2d2d;"
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
        if (autoUnlockMasteryCamo) {
            if (camoStatus) {
                if (camoName.contains(goldName)) {
                    colorHeaderBackgroundColor = "#ffc400"; // Gold-Farbe
                } else if (camoName.contains(diamondName)) {
                    colorHeaderBackgroundColor = "#4a7fe8"; // Diamant-Farbe
                } else if (camoName.contains(darkMatterName)) {
                    colorHeaderBackgroundColor = "#eb4034"; // Dark Matter-Farbe
                }
            }
        } else {
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
    }
    colorLabel->setStyleSheet(QString("QLabel {"
                                      "   border: 2px solid #444444;"
                                      "   border-radius: 10px;"
                                      "   color: #ffffff;"
                                      "   background-color: %1;"
                                      "}"
    ).arg(colorHeaderBackgroundColor));

    weaponLayout->addWidget(tile, index / rowSize, index % rowSize);
}