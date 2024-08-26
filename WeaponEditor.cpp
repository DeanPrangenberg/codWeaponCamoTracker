#include "WeaponEditor.h"

WeaponEditor::WeaponEditor(QWidget *parent) : QWidget(parent) {
    initializeWeaponData();
    ensureFileExists();
    setupUI();
    loadWeaponData();
    if (autoUnlockMasteryCamo) {
        polyatomicStatusLabel->hide();
    }
}

