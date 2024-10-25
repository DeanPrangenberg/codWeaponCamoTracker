#include "WeaponEditor.h"

void WeaponEditor::camoLogic() {
  QVector<QVector<QVector<QVector<bool>>>> camoStatus;

  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, "Error", "Failed to open file:" + fileName);
    return;
  }

  QByteArray data = file.readAll();
  file.close();

  QJsonDocument doc(QJsonDocument::fromJson(data));
  if (doc.isNull() || !doc.isObject()) {
    QMessageBox::critical(this, "Error", "Failed to parse JSON or not an object.");
    return;
  }

  QJsonObject weaponData = doc.object();
  QJsonArray weaponClassesArray = weaponData["WeaponClasses"].toArray();
  camoStatus.clear();

  for (const auto &classValue: weaponClassesArray) {
    QJsonObject classObject = classValue.toObject();
    QJsonArray weaponsArray = classObject["Weapons"].toArray();

    QVector<QVector<QVector<bool>>> classCamos;

    for (const auto &weaponValue: weaponsArray) {
      QJsonObject weaponObject = weaponValue.toObject();
      QJsonArray mCamosArray = weaponObject["M_CAMOS"].toArray();
      QJsonArray wCamosArray = weaponObject["W_CAMOS"].toArray();
      QJsonArray zCamosArray = weaponObject["Z_CAMOS"].toArray();

      QVector<QVector<bool>> weaponCamos(3);

      for (auto &&i: mCamosArray) {
        weaponCamos[0].append(i.toObject()["Status"].toBool());
      }

      for (auto &&i: wCamosArray) {
        weaponCamos[1].append(i.toObject()["Status"].toBool());
      }

      for (auto &&i: zCamosArray) {
        weaponCamos[2].append(i.toObject()["Status"].toBool());
      }

      classCamos.append(weaponCamos);
    }

    camoStatus.append(classCamos);
  }

  // logic
  if (autoUnlockMasteryCamo) {
    // old cod logic
    for (int camoMode = 0; camoMode < 3; ++camoMode) {
      // gold
      for (QVector<QVector<QVector<bool>>> &classVector: camoStatus) {
        for (QVector<QVector<bool>> &weaponVector: classVector) {
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
      for (QVector<QVector<QVector<bool>>> &classVector: camoStatus) {
        int goldForDiamond = 0;
        for (QVector<QVector<bool>> &weaponVector: classVector) {
          if (weaponVector[camoMode][goldPos]) {
            ++goldForDiamond;
          }
        }

        if (goldForDiamond >= standardWeaponAmount[index]) {
          for (QVector<QVector<bool>> &weaponVector: classVector) {
            weaponVector[camoMode][diamondPos] = weaponVector[camoMode][goldPos];
          }
        }
        ++index;
      }

      // dark matter
      int diamondClassesUnlocked = 0;
      for (QVector<QVector<QVector<bool>>> &classVector: camoStatus) {
        for (QVector<QVector<bool>> &weaponVector: classVector) {
          if (weaponVector[camoMode][diamondPos]) {
            ++diamondClassesUnlocked;
            break;
          }
        }
      }

      if (diamondClassesUnlocked >= standardWeaponAmount.size()) {
        for (QVector<QVector<QVector<bool>>> &classVector: camoStatus) {
          for (QVector<QVector<bool>> &weaponVector: classVector) {
            weaponVector[camoMode][darkMatterPos] = weaponVector[camoMode][diamondPos];
          }
        }
      }
    }
  } else {
    // new cod logic

    for (int camoMode = 0; camoMode < 3; ++camoMode) {
      // gold
      for (QVector<QVector<QVector<bool>>> &classVector: camoStatus) {
        for (QVector<QVector<bool>> &weaponVector: classVector) {
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
      for (QVector<QVector<QVector<bool>>> &classVector: camoStatus) {
        goldInClass = 0;
        for (QVector<QVector<bool>> &weaponVector: classVector) {
          if (weaponVector[camoMode][goldPos]) {
            ++goldInClass;
          }
        }

        for (QVector<QVector<bool>> &weaponVector: classVector) {
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
      for (QVector<QVector<QVector<bool>>> &classVector: camoStatus) {
        for (QVector<QVector<bool>> &weaponVector: classVector) {
          if (weaponVector[camoMode][diamondPos]) {
            ++diamondInAllClasses;
          }
        }
      }
      for (QVector<QVector<QVector<bool>>> &classVector: camoStatus) {
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
      for (QVector<QVector<QVector<bool>>> &classVector: camoStatus) {
        for (QVector<QVector<bool>> &weaponVector: classVector) {
          if (weaponVector[camoMode][polyatomicPos]) {
            ++polyatomicInAllClasses;
          }
        }
      }


      for (QVector<QVector<QVector<bool>>> &classVector: camoStatus) {
        for (QVector<QVector<bool>> &weaponVector: classVector) {
          if (polyatomicInAllClasses >= totalStandardWeaponAmount) {
            if (!weaponVector[camoMode][polyatomicPos]) {
              weaponVector[camoMode][darkMatterPos] = false;
            }
          } else {
            weaponVector[camoMode][darkMatterPos] = false;
          }
        }
      }
    }
  }

  QFile writeFile(fileName);
  if (!writeFile.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this, "Error", "Unable to open file for writing.");
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
      QJsonArray wCamosArray;
      QJsonArray zCamosArray;

      for (int i = 0; i < camoStatus[classIndex][weaponIndex][0].size(); ++i) {
        QJsonObject camoObject;
        camoObject["Condition"] = QString("todo for Camo");  // Maintain the condition as placeholder
        camoObject["Name"] = mCamos[i];
        camoObject["Status"] = camoStatus[classIndex][weaponIndex][0][i];
        mCamosArray.append(camoObject);
      }

      for (int i = 0; i < camoStatus[classIndex][weaponIndex][1].size(); ++i) {
        QJsonObject camoObject;
        camoObject["Condition"] = QString("todo for Camo");  // Maintain the condition as placeholder
        camoObject["Name"] = wCamos[i];
        camoObject["Status"] = camoStatus[classIndex][weaponIndex][1][i];
        wCamosArray.append(camoObject);
      }

      for (int i = 0; i < camoStatus[classIndex][weaponIndex][2].size(); ++i) {
        QJsonObject camoObject;
        camoObject["Condition"] = QString("todo for Camo");  // Maintain the condition as placeholder
        camoObject["Name"] = zCamos[i];
        camoObject["Status"] = camoStatus[classIndex][weaponIndex][2][i];
        zCamosArray.append(camoObject);
      }

      weaponObject["M_CAMOS"] = mCamosArray;
      weaponObject["W_CAMOS"] = wCamosArray;
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

  weaponMap["Assault Rifles"] = QStringList{
      "AK-74", "AMES 85", "XM4", "GPR 91", "MODEL L", "GOBLIN MK 2", "AS VAL"
  };
  standardWeaponAmount.push_back(weaponMap["Assault Rifles"].size());

  weaponMap["LMGs"] = QStringList{
      "XMG", "PU-21", "GPMG-7"
  };
  standardWeaponAmount.push_back(weaponMap["LMGs"].size());


  weaponMap["Launchers"] = QStringList{
      "Cigma 2B", "HE-1"
  };
  standardWeaponAmount.push_back(weaponMap["Launchers"].size());


  weaponMap["Marksman Rifles"] = QStringList{
      "Swat 5.56", "DM-10", "Tsarkov 7.62", "AEK-973"
  };
  standardWeaponAmount.push_back(weaponMap["Marksman Rifles"].size());


  weaponMap["Melee"] = QStringList{
      "Knife", "Baseball Bat"
  };
  standardWeaponAmount.push_back(weaponMap["Melee"].size());


  weaponMap["Pistols"] = QStringList{
      "GS45", "9mm PM", "Grekhova", "Stryder .22"
  };
  standardWeaponAmount.push_back(weaponMap["Pistols"].size());

  weaponMap["SMGs"] = QStringList{
      "C9", "Tanto .22", "Jackal PDW", "KSV", "PP-919", "KOMPAKT 92"
  };
  standardWeaponAmount.push_back(weaponMap["SMGs"].size());


  weaponMap["Shotguns"] = QStringList{
      "Marine SP", "ASG-89"
  };
  standardWeaponAmount.push_back(weaponMap["Shotguns"].size());


  weaponMap["Sniper Rifles"] = QStringList{
      "SVD", "LR 7.62", "LW3A1 Frostline"
  };
  standardWeaponAmount.push_back(weaponMap["Sniper Rifles"].size());

  for (int weaponsInClass: standardWeaponAmount) {
    totalStandardWeaponAmount += weaponsInClass;
  }

  mCamos = {
      "Military_1", "Military_2", "Military_3", "Military_4", "Military_5", "Military_6",
      "Military_7", "Military_8", "Military_9",
      "Unique_1", "Unique_2",
      "Gold", "Diamond", "Dark Spine", "Dark Matter"
  };

  zCamos = {
      "Military_1", "Military_2", "Military_3", "Military_4", "Military_5", "Military_6",
      "Military_7", "Military_8", "Military_9",
      "Unique_1", "Unique_2",
      "Mystic Gold", "Opal", "Afterlife", "Nebula"
  };

  wCamos = {
      "Military_1", "Military_2", "Military_3", "Military_4", "Military_5", "Military_6",
      "Military_7", "Military_8", "Military_9",
      "Unique_1", "Unique_2",
      "Gold Tiger", "Kings's Ransom", "Catalyst", "Abyss"
  };

  goldPos = static_cast<int>(mCamos.size() - 4);
  diamondPos = static_cast<int>(mCamos.size() - 3);
  polyatomicPos = static_cast<int>(mCamos.size() - 2);
  darkMatterPos = static_cast<int>(mCamos.size() - 1);
}

void WeaponEditor::updateWeaponClass() {
  QString selectedClass = classComboBox->currentText();
  loadWeaponsForClass(selectedClass);
  updateStatus();
}