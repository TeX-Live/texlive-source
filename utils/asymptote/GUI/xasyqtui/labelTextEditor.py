# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'labelTextEditor.ui'
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
from PySide6.QtWidgets import (QApplication, QCheckBox, QComboBox, QDialog,
    QFrame, QGridLayout, QHBoxLayout, QLabel,
    QPlainTextEdit, QPushButton, QSizePolicy, QSpacerItem,
    QVBoxLayout, QWidget)
from xasyicons import icons_rc

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        if not Dialog.objectName():
            Dialog.setObjectName(u"Dialog")
        Dialog.resize(473, 424)
        self.verticalLayout = QVBoxLayout(Dialog)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.frame = QFrame(Dialog)
        self.frame.setObjectName(u"frame")
        self.frame.setFrameShape(QFrame.StyledPanel)
        self.frame.setFrameShadow(QFrame.Raised)
        self.verticalLayout_3 = QVBoxLayout(self.frame)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.gridLayout = QGridLayout()
        self.gridLayout.setObjectName(u"gridLayout")
        self.gridLayout.setContentsMargins(-1, 0, -1, -1)
        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.gridLayout.addItem(self.horizontalSpacer_2, 0, 2, 1, 1)

        self.chkMathMode = QCheckBox(self.frame)
        self.chkMathMode.setObjectName(u"chkMathMode")

        self.gridLayout.addWidget(self.chkMathMode, 0, 0, 1, 1)

        self.cmbMathStyle = QComboBox(self.frame)
        self.cmbMathStyle.addItem("")
        self.cmbMathStyle.addItem("")
        self.cmbMathStyle.addItem("")
        self.cmbMathStyle.setObjectName(u"cmbMathStyle")
        self.cmbMathStyle.setEnabled(False)
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Maximum, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.cmbMathStyle.sizePolicy().hasHeightForWidth())
        self.cmbMathStyle.setSizePolicy(sizePolicy)
        self.cmbMathStyle.setMinimumSize(QSize(100, 0))

        self.gridLayout.addWidget(self.cmbMathStyle, 0, 1, 1, 1)


        self.verticalLayout_3.addLayout(self.gridLayout)

        self.verticalLayout_2 = QVBoxLayout()
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.txtLabelEdit = QPlainTextEdit(self.frame)
        self.txtLabelEdit.setObjectName(u"txtLabelEdit")

        self.verticalLayout_2.addWidget(self.txtLabelEdit)


        self.verticalLayout_3.addLayout(self.verticalLayout_2)

        self.verticalLayout_4 = QVBoxLayout()
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.verticalLayout_4.setContentsMargins(-1, 0, -1, -1)
        self.label = QLabel(self.frame)
        self.label.setObjectName(u"label")

        self.verticalLayout_4.addWidget(self.label)

        self.lblLabelPreview = QLabel(self.frame)
        self.lblLabelPreview.setObjectName(u"lblLabelPreview")
        self.lblLabelPreview.setMinimumSize(QSize(0, 100))
        self.lblLabelPreview.setFrameShape(QFrame.Box)

        self.verticalLayout_4.addWidget(self.lblLabelPreview)


        self.verticalLayout_3.addLayout(self.verticalLayout_4)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)

        self.btnGetText = QPushButton(self.frame)
        self.btnGetText.setObjectName(u"btnGetText")
        self.btnGetText.setMaximumSize(QSize(32, 32))
        icon = QIcon()
        icon.addFile(u":/icons/text.svg", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.btnGetText.setIcon(icon)
        self.btnGetText.setFlat(True)

        self.horizontalLayout.addWidget(self.btnGetText)

        self.btnPreview = QPushButton(self.frame)
        self.btnPreview.setObjectName(u"btnPreview")
        self.btnPreview.setMaximumSize(QSize(32, 32))
        icon1 = QIcon()
        icon1.addFile(u":/icons/eye.svg", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.btnPreview.setIcon(icon1)
        self.btnPreview.setFlat(True)

        self.horizontalLayout.addWidget(self.btnPreview)

        self.btnCancel = QPushButton(self.frame)
        self.btnCancel.setObjectName(u"btnCancel")
        self.btnCancel.setMaximumSize(QSize(32, 32))
        icon2 = QIcon()
        icon2.addFile(u":/icons/android-close.svg", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.btnCancel.setIcon(icon2)
        self.btnCancel.setFlat(True)

        self.horizontalLayout.addWidget(self.btnCancel)

        self.btnAccept = QPushButton(self.frame)
        self.btnAccept.setObjectName(u"btnAccept")
        self.btnAccept.setMaximumSize(QSize(32, 32))
        icon3 = QIcon()
        icon3.addFile(u":/icons/android-done.svg", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.btnAccept.setIcon(icon3)
        self.btnAccept.setFlat(True)

        self.horizontalLayout.addWidget(self.btnAccept)


        self.verticalLayout_3.addLayout(self.horizontalLayout)


        self.verticalLayout.addWidget(self.frame)


        self.retranslateUi(Dialog)

        QMetaObject.connectSlotsByName(Dialog)
    # setupUi

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QCoreApplication.translate("Dialog", u"Dialog", None))
        self.chkMathMode.setText(QCoreApplication.translate("Dialog", u"Math Mode", None))
        self.cmbMathStyle.setItemText(0, QCoreApplication.translate("Dialog", u"Inline Style", None))
        self.cmbMathStyle.setItemText(1, QCoreApplication.translate("Dialog", u"Display Style", None))
        self.cmbMathStyle.setItemText(2, QCoreApplication.translate("Dialog", u"Script Style", None))

        self.label.setText(QCoreApplication.translate("Dialog", u"Preview", None))
        self.lblLabelPreview.setText("")
        self.btnGetText.setText("")
        self.btnPreview.setText("")
        self.btnCancel.setText("")
        self.btnAccept.setText("")
    # retranslateUi

