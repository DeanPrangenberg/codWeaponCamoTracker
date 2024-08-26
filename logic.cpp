#include "WeaponEditor.h"

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

void WeaponEditor::loadWeaponsForClass(const QString &weaponClass) {
    // Clear existing layout items
    while (QLayoutItem *item = weaponLayout->takeAt(0)) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    int widgetWidth = width();
    const int minTileSize = 316;
    int numTilesPerRow = widgetWidth / minTileSize;
    if (numTilesPerRow < 2) {
        numTilesPerRow = 2;
    }

    QStringList weaponNames = weaponMap.value(weaponClass);
    for (int i = 0; i < weaponNames.size(); ++i) {
        createWeaponTile(weaponNames[i], i, numTilesPerRow);
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
    loadWeaponsForClass(selectedClass);
    updateStatus();
}