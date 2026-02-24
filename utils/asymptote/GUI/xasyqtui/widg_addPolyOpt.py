# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'widg_addPolyOpt.ui'
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
from PySide6.QtWidgets import (QApplication, QCheckBox, QHBoxLayout, QLabel,
    QLineEdit, QSizePolicy, QSpacerItem, QWidget)

class Ui_Form(object):
    def setupUi(self, Form):
        if not Form.objectName():
            Form.setObjectName(u"Form")
        Form.setWindowModality(Qt.NonModal)
        Form.resize(326, 35)
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
        self.chkInscribed = QCheckBox(Form)
        self.chkInscribed.setObjectName(u"chkInscribed")

        self.horizontalLayout.addWidget(self.chkInscribed)

        self.horizontalSpacer_2 = QSpacerItem(19, 20, QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer_2)

        self.label = QLabel(Form)
        self.label.setObjectName(u"label")
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy1)

        self.horizontalLayout.addWidget(self.label)

        self.txtSides = QLineEdit(Form)
        self.txtSides.setObjectName(u"txtSides")

        self.horizontalLayout.addWidget(self.txtSides)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)


        self.horizontalLayout_2.addLayout(self.horizontalLayout)


        self.retranslateUi(Form)

        QMetaObject.connectSlotsByName(Form)
    # setupUi

    def retranslateUi(self, Form):
        Form.setWindowTitle(QCoreApplication.translate("Form", u"Form", None))
        self.chkInscribed.setText(QCoreApplication.translate("Form", u"Start at Vertex", None))
        self.label.setText(QCoreApplication.translate("Form", u"Sides:  ", None))
#if QT_CONFIG(tooltip)
        self.txtSides.setToolTip(QCoreApplication.translate("Form", u"Number of Sides", None))
#endif // QT_CONFIG(tooltip)
        self.txtSides.setPlaceholderText(QCoreApplication.translate("Form", u"Sides", None))
    # retranslateUi

