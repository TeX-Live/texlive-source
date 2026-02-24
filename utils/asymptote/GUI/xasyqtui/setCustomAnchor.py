# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'setCustomAnchor.ui'
##
## Created by: Qt User Interface Compiler version 6.9.0
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QAbstractButton, QApplication, QDialog, QDialogButtonBox,
    QFormLayout, QLabel, QLineEdit, QSizePolicy,
    QVBoxLayout, QWidget)

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        if not Dialog.objectName():
            Dialog.setObjectName(u"Dialog")
        Dialog.resize(245, 161)
        self.verticalLayoutWidget = QWidget(Dialog)
        self.verticalLayoutWidget.setObjectName(u"verticalLayoutWidget")
        self.verticalLayoutWidget.setGeometry(QRect(20, 20, 201, 121))
        self.verticalLayout = QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.formLayout = QFormLayout()
        self.formLayout.setObjectName(u"formLayout")
        self.formLayout.setFieldGrowthPolicy(QFormLayout.AllNonFixedFieldsGrow)
        self.label = QLabel(self.verticalLayoutWidget)
        self.label.setObjectName(u"label")

        self.formLayout.setWidget(1, QFormLayout.ItemRole.LabelRole, self.label)

        self.lineEditX = QLineEdit(self.verticalLayoutWidget)
        self.lineEditX.setObjectName(u"lineEditX")

        self.formLayout.setWidget(1, QFormLayout.ItemRole.FieldRole, self.lineEditX)

        self.label_2 = QLabel(self.verticalLayoutWidget)
        self.label_2.setObjectName(u"label_2")

        self.formLayout.setWidget(2, QFormLayout.ItemRole.LabelRole, self.label_2)

        self.lineEditY = QLineEdit(self.verticalLayoutWidget)
        self.lineEditY.setObjectName(u"lineEditY")

        self.formLayout.setWidget(2, QFormLayout.ItemRole.FieldRole, self.lineEditY)


        self.verticalLayout.addLayout(self.formLayout)

        self.buttonBox = QDialogButtonBox(self.verticalLayoutWidget)
        self.buttonBox.setObjectName(u"buttonBox")
        self.buttonBox.setOrientation(Qt.Horizontal)
        self.buttonBox.setStandardButtons(QDialogButtonBox.Cancel|QDialogButtonBox.Ok|QDialogButtonBox.Reset)

        self.verticalLayout.addWidget(self.buttonBox)


        self.retranslateUi(Dialog)
        self.buttonBox.accepted.connect(Dialog.accept)
        self.buttonBox.rejected.connect(Dialog.reject)

        QMetaObject.connectSlotsByName(Dialog)
    # setupUi

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QCoreApplication.translate("Dialog", u"Set Custom Anchor", None))
        self.label.setText(QCoreApplication.translate("Dialog", u"X:", None))
        self.lineEditX.setText(QCoreApplication.translate("Dialog", u"0", None))
        self.label_2.setText(QCoreApplication.translate("Dialog", u"Y:", None))
        self.lineEditY.setText(QCoreApplication.translate("Dialog", u"0", None))
    # retranslateUi

