# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'widg_addLabel.ui'
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
from PySide6.QtWidgets import (QApplication, QComboBox, QHBoxLayout, QLabel,
    QLineEdit, QPushButton, QSizePolicy, QSpacerItem,
    QWidget)
from xasyicons import icons_rc

class Ui_Form(object):
    def setupUi(self, Form):
        if not Form.objectName():
            Form.setObjectName(u"Form")
        Form.setWindowModality(Qt.NonModal)
        Form.resize(599, 35)
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(Form.sizePolicy().hasHeightForWidth())
        Form.setSizePolicy(sizePolicy)
        Form.setMinimumSize(QSize(0, 35))
        Form.setMaximumSize(QSize(16777215, 35))
        self.horizontalLayout_2 = QHBoxLayout(Form)
        self.horizontalLayout_2.setSpacing(0)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.txtLabelText = QLineEdit(Form)
        self.txtLabelText.setObjectName(u"txtLabelText")

        self.horizontalLayout.addWidget(self.txtLabelText)

        self.btnAdvancedEdit = QPushButton(Form)
        self.btnAdvancedEdit.setObjectName(u"btnAdvancedEdit")
        self.btnAdvancedEdit.setMaximumSize(QSize(25, 25))
        icon = QIcon()
        icon.addFile(u":/icons/edit.svg", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.btnAdvancedEdit.setIcon(icon)
        self.btnAdvancedEdit.setFlat(True)

        self.horizontalLayout.addWidget(self.btnAdvancedEdit)

        self.label = QLabel(Form)
        self.label.setObjectName(u"label")

        self.horizontalLayout.addWidget(self.label)

        self.cmbAlign = QComboBox(Form)
        self.cmbAlign.addItem("")
        self.cmbAlign.addItem("")
        self.cmbAlign.addItem("")
        self.cmbAlign.addItem("")
        self.cmbAlign.addItem("")
        self.cmbAlign.addItem("")
        self.cmbAlign.addItem("")
        self.cmbAlign.addItem("")
        self.cmbAlign.addItem("")
        self.cmbAlign.addItem("")
        self.cmbAlign.setObjectName(u"cmbAlign")

        self.horizontalLayout.addWidget(self.cmbAlign)

        self.label_3 = QLabel(Form)
        self.label_3.setObjectName(u"label_3")

        self.horizontalLayout.addWidget(self.label_3)

        self.cmbFontSize = QComboBox(Form)
        self.cmbFontSize.addItem("")
        self.cmbFontSize.addItem("")
        self.cmbFontSize.addItem("")
        self.cmbFontSize.addItem("")
        self.cmbFontSize.addItem("")
        self.cmbFontSize.addItem("")
        self.cmbFontSize.addItem("")
        self.cmbFontSize.addItem("")
        self.cmbFontSize.addItem("")
        self.cmbFontSize.addItem("")
        self.cmbFontSize.addItem("")
        self.cmbFontSize.setObjectName(u"cmbFontSize")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.cmbFontSize.sizePolicy().hasHeightForWidth())
        self.cmbFontSize.setSizePolicy(sizePolicy1)
        self.cmbFontSize.setEditable(True)

        self.horizontalLayout.addWidget(self.cmbFontSize)

        self.label_2 = QLabel(Form)
        self.label_2.setObjectName(u"label_2")

        self.horizontalLayout.addWidget(self.label_2)

        self.txtShiftX = QLineEdit(Form)
        self.txtShiftX.setObjectName(u"txtShiftX")
        sizePolicy2 = QSizePolicy(QSizePolicy.Policy.Maximum, QSizePolicy.Policy.Fixed)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.txtShiftX.sizePolicy().hasHeightForWidth())
        self.txtShiftX.setSizePolicy(sizePolicy2)
        self.txtShiftX.setMaximumSize(QSize(50, 16777215))

        self.horizontalLayout.addWidget(self.txtShiftX)

        self.txtShiftY = QLineEdit(Form)
        self.txtShiftY.setObjectName(u"txtShiftY")
        sizePolicy2.setHeightForWidth(self.txtShiftY.sizePolicy().hasHeightForWidth())
        self.txtShiftY.setSizePolicy(sizePolicy2)
        self.txtShiftY.setMaximumSize(QSize(50, 16777215))

        self.horizontalLayout.addWidget(self.txtShiftY)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)


        self.horizontalLayout_2.addLayout(self.horizontalLayout)


        self.retranslateUi(Form)

        QMetaObject.connectSlotsByName(Form)
    # setupUi

    def retranslateUi(self, Form):
        Form.setWindowTitle(QCoreApplication.translate("Form", u"Form", None))
#if QT_CONFIG(tooltip)
        self.txtLabelText.setToolTip(QCoreApplication.translate("Form", u"Number of Sides", None))
#endif // QT_CONFIG(tooltip)
        self.txtLabelText.setPlaceholderText(QCoreApplication.translate("Form", u"Text", None))
        self.btnAdvancedEdit.setText("")
        self.label.setText(QCoreApplication.translate("Form", u"Align", None))
        self.cmbAlign.setItemText(0, QCoreApplication.translate("Form", u"Center", None))
        self.cmbAlign.setItemText(1, QCoreApplication.translate("Form", u"N", None))
        self.cmbAlign.setItemText(2, QCoreApplication.translate("Form", u"E", None))
        self.cmbAlign.setItemText(3, QCoreApplication.translate("Form", u"W", None))
        self.cmbAlign.setItemText(4, QCoreApplication.translate("Form", u"S", None))
        self.cmbAlign.setItemText(5, QCoreApplication.translate("Form", u"NW", None))
        self.cmbAlign.setItemText(6, QCoreApplication.translate("Form", u"NE", None))
        self.cmbAlign.setItemText(7, QCoreApplication.translate("Form", u"SW", None))
        self.cmbAlign.setItemText(8, QCoreApplication.translate("Form", u"SE", None))
        self.cmbAlign.setItemText(9, QCoreApplication.translate("Form", u"Custom", None))

        self.label_3.setText(QCoreApplication.translate("Form", u"Font Size", None))
        self.cmbFontSize.setItemText(0, QCoreApplication.translate("Form", u"-", None))
        self.cmbFontSize.setItemText(1, QCoreApplication.translate("Form", u"8", None))
        self.cmbFontSize.setItemText(2, QCoreApplication.translate("Form", u"9", None))
        self.cmbFontSize.setItemText(3, QCoreApplication.translate("Form", u"10", None))
        self.cmbFontSize.setItemText(4, QCoreApplication.translate("Form", u"11", None))
        self.cmbFontSize.setItemText(5, QCoreApplication.translate("Form", u"12", None))
        self.cmbFontSize.setItemText(6, QCoreApplication.translate("Form", u"14", None))
        self.cmbFontSize.setItemText(7, QCoreApplication.translate("Form", u"18", None))
        self.cmbFontSize.setItemText(8, QCoreApplication.translate("Form", u"24", None))
        self.cmbFontSize.setItemText(9, QCoreApplication.translate("Form", u"48", None))
        self.cmbFontSize.setItemText(10, QCoreApplication.translate("Form", u"72", None))

        self.label_2.setText(QCoreApplication.translate("Form", u"Custom Align", None))
        self.txtShiftX.setPlaceholderText(QCoreApplication.translate("Form", u"Shift X", None))
        self.txtShiftY.setPlaceholderText(QCoreApplication.translate("Form", u"Shift Y", None))
    # retranslateUi

