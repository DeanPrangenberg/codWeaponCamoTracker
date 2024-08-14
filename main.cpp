#include <QApplication>
#include "WeaponEditor.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WeaponEditor editor;
    editor.setWindowTitle("Weapon Editor");
    editor.resize(1000, 800);
    editor.show();

    return app.exec();
}
