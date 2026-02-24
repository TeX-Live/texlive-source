# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'widgetPointEditor.ui'
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
from PySide6.QtWidgets import (QApplication, QHBoxLayout, QLabel, QLineEdit,
    QPushButton, QSizePolicy, QVBoxLayout, QWidget)
from xasyicons import icons_rc

class Ui_Form(object):
    def setupUi(self, Form):
        if not Form.objectName():
            Form.setObjectName(u"Form")
        Form.resize(324, 67)
        self.verticalLayout = QVBoxLayout(Form)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.nameLabel = QLabel(Form)
        self.nameLabel.setObjectName(u"nameLabel")
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.nameLabel.sizePolicy().hasHeightForWidth())
        self.nameLabel.setSizePolicy(sizePolicy)

        self.verticalLayout.addWidget(self.nameLabel)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.lineXorA = QLineEdit(Form)
        self.lineXorA.setObjectName(u"lineXorA")
        self.lineXorA.setEnabled(False)
        self.lineXorA.setReadOnly(False)

        self.horizontalLayout.addWidget(self.lineXorA)

        self.lineYorM = QLineEdit(Form)
        self.lineYorM.setObjectName(u"lineYorM")
        self.lineYorM.setEnabled(False)
        self.lineYorM.setAutoFillBackground(False)
        self.lineYorM.setReadOnly(False)

        self.horizontalLayout.addWidget(self.lineYorM)

        self.btnRelative = QPushButton(Form)
        self.btnRelative.setObjectName(u"btnRelative")
        self.btnRelative.setEnabled(False)
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.btnRelative.sizePolicy().hasHeightForWidth())
        self.btnRelative.setSizePolicy(sizePolicy1)
        icon = QIcon()
        icon.addFile(u":/icons/android-locate.svg", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.btnRelative.setIcon(icon)
        self.btnRelative.setCheckable(True)
        self.btnRelative.setFlat(False)

        self.horizontalLayout.addWidget(self.btnRelative)

        self.btnPolar = QPushButton(Form)
        self.btnPolar.setObjectName(u"btnPolar")
        self.btnPolar.setEnabled(False)
        sizePolicy1.setHeightForWidth(self.btnPolar.sizePolicy().hasHeightForWidth())
        self.btnPolar.setSizePolicy(sizePolicy1)
        icon1 = QIcon()
        icon1.addFile(u":/icons/android-radio-button-off.svg", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.btnPolar.setIcon(icon1)
        self.btnPolar.setCheckable(True)
        self.btnPolar.setFlat(False)

        self.horizontalLayout.addWidget(self.btnPolar)

        self.btnManualAdj = QPushButton(Form)
        self.btnManualAdj.setObjectName(u"btnManualAdj")
        sizePolicy1.setHeightForWidth(self.btnManualAdj.sizePolicy().hasHeightForWidth())
        self.btnManualAdj.setSizePolicy(sizePolicy1)
        icon2 = QIcon()
        icon2.addFile(u":/icons/edit.svg", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.btnManualAdj.setIcon(icon2)
        self.btnManualAdj.setCheckable(True)
        self.btnManualAdj.setFlat(False)

        self.horizontalLayout.addWidget(self.btnManualAdj)


        self.verticalLayout.addLayout(self.horizontalLayout)


        self.retranslateUi(Form)

        QMetaObject.connectSlotsByName(Form)
    # setupUi

    def retranslateUi(self, Form):
        Form.setWindowTitle(QCoreApplication.translate("Form", u"Form", None))
        self.nameLabel.setText(QCoreApplication.translate("Form", u"Left Control Point", None))
#if QT_CONFIG(tooltip)
        self.lineXorA.setToolTip(QCoreApplication.translate("Form", u"X", None))
#endif // QT_CONFIG(tooltip)
        self.lineXorA.setPlaceholderText(QCoreApplication.translate("Form", u"X", None))
#if QT_CONFIG(tooltip)
        self.lineYorM.setToolTip(QCoreApplication.translate("Form", u"X", None))
#endif // QT_CONFIG(tooltip)
        self.lineYorM.setPlaceholderText(QCoreApplication.translate("Form", u"Y", None))
        self.btnRelative.setText("")
        self.btnPolar.setText("")
        self.btnManualAdj.setText("")
    # retranslateUi

