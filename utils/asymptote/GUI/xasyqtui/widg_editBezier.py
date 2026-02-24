# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'widg_editBezier.ui'
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
from PySide6.QtWidgets import (QApplication, QCheckBox, QComboBox, QHBoxLayout,
    QPushButton, QSizePolicy, QSpacerItem, QWidget)
from xasyicons import icons_rc

class Ui_Form(object):
    def setupUi(self, Form):
        if not Form.objectName():
            Form.setObjectName(u"Form")
        Form.setWindowModality(Qt.NonModal)
        Form.resize(692, 35)
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
        self.cmbLockMode = QComboBox(Form)
        self.cmbLockMode.addItem("")
        self.cmbLockMode.addItem("")
        self.cmbLockMode.addItem("")
        self.cmbLockMode.setObjectName(u"cmbLockMode")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.MinimumExpanding, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.cmbLockMode.sizePolicy().hasHeightForWidth())
        self.cmbLockMode.setSizePolicy(sizePolicy1)

        self.horizontalLayout.addWidget(self.cmbLockMode)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)

        self.chkRecompute = QCheckBox(Form)
        self.chkRecompute.setObjectName(u"chkRecompute")
        sizePolicy1.setHeightForWidth(self.chkRecompute.sizePolicy().hasHeightForWidth())
        self.chkRecompute.setSizePolicy(sizePolicy1)

        self.horizontalLayout.addWidget(self.chkRecompute)

        self.btnForceRecompute = QPushButton(Form)
        self.btnForceRecompute.setObjectName(u"btnForceRecompute")

        self.horizontalLayout.addWidget(self.btnForceRecompute)

        self.btnOk = QPushButton(Form)
        self.btnOk.setObjectName(u"btnOk")
        sizePolicy.setHeightForWidth(self.btnOk.sizePolicy().hasHeightForWidth())
        self.btnOk.setSizePolicy(sizePolicy)
        self.btnOk.setMaximumSize(QSize(25, 25))
        icon = QIcon()
        icon.addFile(u":/icons/check.svg", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.btnOk.setIcon(icon)
        self.btnOk.setFlat(True)

        self.horizontalLayout.addWidget(self.btnOk)

        self.btnCancel = QPushButton(Form)
        self.btnCancel.setObjectName(u"btnCancel")
        sizePolicy.setHeightForWidth(self.btnCancel.sizePolicy().hasHeightForWidth())
        self.btnCancel.setSizePolicy(sizePolicy)
        self.btnCancel.setMaximumSize(QSize(25, 25))
        icon1 = QIcon()
        icon1.addFile(u":/icons/close-round.svg", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        self.btnCancel.setIcon(icon1)
        self.btnCancel.setFlat(True)

        self.horizontalLayout.addWidget(self.btnCancel)


        self.horizontalLayout_2.addLayout(self.horizontalLayout)


        self.retranslateUi(Form)

        self.cmbLockMode.setCurrentIndex(1)


        QMetaObject.connectSlotsByName(Form)
    # setupUi

    def retranslateUi(self, Form):
        Form.setWindowTitle(QCoreApplication.translate("Form", u"Form", None))
        self.cmbLockMode.setItemText(0, QCoreApplication.translate("Form", u"No Lock", None))
        self.cmbLockMode.setItemText(1, QCoreApplication.translate("Form", u"Lock Angle", None))
        self.cmbLockMode.setItemText(2, QCoreApplication.translate("Form", u"Lock Angle & Scale", None))

        self.chkRecompute.setText(QCoreApplication.translate("Form", u"Recompute Path", None))
        self.btnForceRecompute.setText(QCoreApplication.translate("Form", u"Recompute Once", None))
    # retranslateUi

