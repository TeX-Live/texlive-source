# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'custMatTransform.ui'
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
from PySide6.QtWidgets import (QApplication, QDialog, QFrame, QGridLayout,
    QHBoxLayout, QLabel, QLineEdit, QPushButton,
    QSizePolicy, QSpacerItem, QVBoxLayout, QWidget)
from xasyicons import icons_rc

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        if not Dialog.objectName():
            Dialog.setObjectName(u"Dialog")
        Dialog.resize(500, 320)
        sizePolicy = QSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(Dialog.sizePolicy().hasHeightForWidth())
        Dialog.setSizePolicy(sizePolicy)
        Dialog.setMinimumSize(QSize(500, 320))
        Dialog.setMaximumSize(QSize(500, 320))
        Dialog.setMouseTracking(False)
        icon = QIcon()
        icon.addFile(u":/icons/android-expand.svg", QSize(), QIcon.Mode.Normal, QIcon.State.Off)
        Dialog.setWindowIcon(icon)
        Dialog.setSizeGripEnabled(True)
        Dialog.setModal(False)
        self.centralFrame = QFrame(Dialog)
        self.centralFrame.setObjectName(u"centralFrame")
        self.centralFrame.setGeometry(QRect(20, 20, 461, 271))
        sizePolicy1 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Expanding)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.centralFrame.sizePolicy().hasHeightForWidth())
        self.centralFrame.setSizePolicy(sizePolicy1)
        self.centralFrame.setBaseSize(QSize(0, 0))
        self.verticalLayout_3 = QVBoxLayout(self.centralFrame)
        self.verticalLayout_3.setSpacing(4)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.verticalLayout = QVBoxLayout()
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.label = QLabel(self.centralFrame)
        self.label.setObjectName(u"label")
        sizePolicy2 = QSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Fixed)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy2)

        self.verticalLayout.addWidget(self.label)

        self.gridFrame = QFrame(self.centralFrame)
        self.gridFrame.setObjectName(u"gridFrame")
        self.gridFrame.setFrameShape(QFrame.Box)
        self.gridLayout = QGridLayout(self.gridFrame)
        self.gridLayout.setObjectName(u"gridLayout")
        self.lineMat00 = QLineEdit(self.gridFrame)
        self.lineMat00.setObjectName(u"lineMat00")
        self.lineMat00.setMaximumSize(QSize(70, 16777215))

        self.gridLayout.addWidget(self.lineMat00, 1, 0, 1, 1)

        self.lineMat11 = QLineEdit(self.gridFrame)
        self.lineMat11.setObjectName(u"lineMat11")
        self.lineMat11.setMaximumSize(QSize(70, 16777215))

        self.gridLayout.addWidget(self.lineMat11, 2, 1, 1, 1)

        self.lineMat10 = QLineEdit(self.gridFrame)
        self.lineMat10.setObjectName(u"lineMat10")
        self.lineMat10.setMaximumSize(QSize(70, 16777215))

        self.gridLayout.addWidget(self.lineMat10, 2, 0, 1, 1)

        self.lineMat01 = QLineEdit(self.gridFrame)
        self.lineMat01.setObjectName(u"lineMat01")
        self.lineMat01.setMaximumSize(QSize(70, 16777215))

        self.gridLayout.addWidget(self.lineMat01, 1, 1, 1, 1)


        self.verticalLayout.addWidget(self.gridFrame)


        self.horizontalLayout_3.addLayout(self.verticalLayout)

        self.horizontalSpacer_3 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_3.addItem(self.horizontalSpacer_3)

        self.verticalLayout_4 = QVBoxLayout()
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.label_3 = QLabel(self.centralFrame)
        self.label_3.setObjectName(u"label_3")
        sizePolicy.setHeightForWidth(self.label_3.sizePolicy().hasHeightForWidth())
        self.label_3.setSizePolicy(sizePolicy)

        self.verticalLayout_4.addWidget(self.label_3)

        self.gridFrame_2 = QFrame(self.centralFrame)
        self.gridFrame_2.setObjectName(u"gridFrame_2")
        self.gridFrame_2.setFrameShape(QFrame.Box)
        self.gridLayout_2 = QGridLayout(self.gridFrame_2)
        self.gridLayout_2.setObjectName(u"gridLayout_2")
        self.lineMatTy = QLineEdit(self.gridFrame_2)
        self.lineMatTy.setObjectName(u"lineMatTy")
        self.lineMatTy.setMaximumSize(QSize(70, 16777215))

        self.gridLayout_2.addWidget(self.lineMatTy, 2, 1, 1, 1)

        self.lineMatTx = QLineEdit(self.gridFrame_2)
        self.lineMatTx.setObjectName(u"lineMatTx")
        self.lineMatTx.setMaximumSize(QSize(70, 16777215))

        self.gridLayout_2.addWidget(self.lineMatTx, 1, 1, 1, 1)


        self.verticalLayout_4.addWidget(self.gridFrame_2)


        self.horizontalLayout_3.addLayout(self.verticalLayout_4)


        self.horizontalLayout.addLayout(self.horizontalLayout_3)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)

        self.verticalLayout_2 = QVBoxLayout()
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.label_2 = QLabel(self.centralFrame)
        self.label_2.setObjectName(u"label_2")

        self.verticalLayout_2.addWidget(self.label_2)

        self.imgPreview = QLabel(self.centralFrame)
        self.imgPreview.setObjectName(u"imgPreview")
        sizePolicy3 = QSizePolicy(QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Minimum)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.imgPreview.sizePolicy().hasHeightForWidth())
        self.imgPreview.setSizePolicy(sizePolicy3)
        self.imgPreview.setMinimumSize(QSize(150, 150))
        self.imgPreview.setBaseSize(QSize(300, 300))
        self.imgPreview.setFrameShape(QFrame.Box)

        self.verticalLayout_2.addWidget(self.imgPreview)


        self.horizontalLayout.addLayout(self.verticalLayout_2)


        self.verticalLayout_3.addLayout(self.horizontalLayout)

        self.lblAnchor = QLabel(self.centralFrame)
        self.lblAnchor.setObjectName(u"lblAnchor")
        self.lblAnchor.setAlignment(Qt.AlignRight|Qt.AlignTrailing|Qt.AlignVCenter)

        self.verticalLayout_3.addWidget(self.lblAnchor)

        self.lblCoordsMode = QLabel(self.centralFrame)
        self.lblCoordsMode.setObjectName(u"lblCoordsMode")
        self.lblCoordsMode.setAlignment(Qt.AlignRight|Qt.AlignTrailing|Qt.AlignVCenter)

        self.verticalLayout_3.addWidget(self.lblCoordsMode)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout_3.addItem(self.verticalSpacer)

        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout_2.addItem(self.horizontalSpacer_2)

        self.btnReset = QPushButton(self.centralFrame)
        self.btnReset.setObjectName(u"btnReset")

        self.horizontalLayout_2.addWidget(self.btnReset)

        self.btnCancel = QPushButton(self.centralFrame)
        self.btnCancel.setObjectName(u"btnCancel")

        self.horizontalLayout_2.addWidget(self.btnCancel)

        self.btnAccept = QPushButton(self.centralFrame)
        self.btnAccept.setObjectName(u"btnAccept")

        self.horizontalLayout_2.addWidget(self.btnAccept)


        self.verticalLayout_3.addLayout(self.horizontalLayout_2)


        self.retranslateUi(Dialog)

        QMetaObject.connectSlotsByName(Dialog)
    # setupUi

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QCoreApplication.translate("Dialog", u"Set Custom Transformation", None))
        self.label.setText(QCoreApplication.translate("Dialog", u"Transformation Matrix", None))
        self.lineMat00.setText(QCoreApplication.translate("Dialog", u"1", None))
        self.lineMat11.setText(QCoreApplication.translate("Dialog", u"1", None))
        self.lineMat10.setText(QCoreApplication.translate("Dialog", u"0", None))
        self.lineMat01.setText(QCoreApplication.translate("Dialog", u"0", None))
        self.label_3.setText(QCoreApplication.translate("Dialog", u"Translation", None))
        self.lineMatTy.setText(QCoreApplication.translate("Dialog", u"0", None))
        self.lineMatTx.setText(QCoreApplication.translate("Dialog", u"0", None))
        self.label_2.setText(QCoreApplication.translate("Dialog", u"Preview:", None))
#if QT_CONFIG(tooltip)
        self.imgPreview.setToolTip(QCoreApplication.translate("Dialog", u"Shows a red square if transformation determinant is negative.", None))
#endif // QT_CONFIG(tooltip)
        self.imgPreview.setText("")
        self.lblAnchor.setText(QCoreApplication.translate("Dialog", u"Anchor: Top Left", None))
        self.lblCoordsMode.setText(QCoreApplication.translate("Dialog", u"Coordinates: Global", None))
        self.btnReset.setText(QCoreApplication.translate("Dialog", u"Reset", None))
        self.btnCancel.setText(QCoreApplication.translate("Dialog", u"Cancel", None))
        self.btnAccept.setText(QCoreApplication.translate("Dialog", u"Accept", None))
    # retranslateUi

