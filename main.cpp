#include <QApplication>
#include "WeaponEditor.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WeaponEditor editor;
    editor.setWindowTitle("Weapon Tracker");
    editor.setGeometry(0,0,1000,978);
    editor.setWindowIcon(QIcon("windowIcon.png"));
    editor.show();

    return app.exec();
}
