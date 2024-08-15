#include <QApplication>
#include "WeaponEditor.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WeaponEditor editor;
    editor.setWindowTitle("Weapon Editor");
    editor.setFixedSize(1000, 860);
    editor.show();

    return app.exec();
}
