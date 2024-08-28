#include "WeaponEditor.h"

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
            QJsonObject camosObject = weaponObject["Camos"].toObject();
        }
    }
}

void WeaponEditor::ensureFileExists() {
    QFile file(fileName);
    if (!file.exists()) {
        genJsonWeaponData();
    }
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
                        bool isMultiplayer = false;
                        bool isWarzone = false;
                        bool isZombies = false;

                        if (camoTypeComboBox->currentText() == "Multiplayer") {
                            isMultiplayer = true;
                        } else if (camoTypeComboBox->currentText() == "Warzone") {
                            isWarzone = true;
                        } else {
                            isZombies = true;
                        }

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
                        }

                        if (isWarzone) {
                            QJsonArray wCamosArray = weaponObject["W_CAMOS"].toArray();
                            for (int i = 0; i < wCamosArray.size(); ++i) {
                                QJsonObject camoObject = wCamosArray[i].toObject();
                                if (camoObject["Name"].toString() == key) {
                                    camoObject["Status"] = status;
                                    wCamosArray[i] = camoObject; // Update the array with the modified object
                                    break;
                                }
                            }
                            weaponObject["W_CAMOS"] = wCamosArray;
                        }

                        if (isZombies) {
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

void WeaponEditor::genJsonWeaponData() {
    QJsonObject json;
    QJsonArray classesArray;

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

            QJsonArray wCamosArray;
            for (const QString &camoName : wCamos) {
                QJsonObject camoObject;
                camoObject["Name"] = camoName;
                camoObject["Status"] = false;
                camoObject["Condition"] = "todo for Camo";
                wCamosArray.append(camoObject);
            }
            weaponObject["W_CAMOS"] = wCamosArray;

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