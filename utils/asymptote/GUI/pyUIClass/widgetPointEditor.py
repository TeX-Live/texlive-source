# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'GUI/windows/widgetPointEditor.ui'
#
# Created by: PyQt5 UI code generator 5.13.1
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(324, 67)
        self.verticalLayout = QtWidgets.QVBoxLayout(Form)
        self.verticalLayout.setObjectName("verticalLayout")
        self.nameLabel = QtWidgets.QLabel(Form)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.nameLabel.sizePolicy().hasHeightForWidth())
        self.nameLabel.setSizePolicy(sizePolicy)
        self.nameLabel.setObjectName("nameLabel")
        self.verticalLayout.addWidget(self.nameLabel)
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.lineXorA = QtWidgets.QLineEdit(Form)
        self.lineXorA.setEnabled(False)
        self.lineXorA.setReadOnly(False)
        self.lineXorA.setObjectName("lineXorA")
        self.horizontalLayout.addWidget(self.lineXorA)
        self.lineYorM = QtWidgets.QLineEdit(Form)
        self.lineYorM.setEnabled(False)
        self.lineYorM.setAutoFillBackground(False)
        self.lineYorM.setReadOnly(False)
        self.lineYorM.setObjectName("lineYorM")
        self.horizontalLayout.addWidget(self.lineYorM)
        self.btnRelative = QtWidgets.QPushButton(Form)
        self.btnRelative.setEnabled(False)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.btnRelative.sizePolicy().hasHeightForWidth())
        self.btnRelative.setSizePolicy(sizePolicy)
        self.btnRelative.setText("")
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(":/icons/android-locate.svg"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.btnRelative.setIcon(icon)
        self.btnRelative.setCheckable(True)
        self.btnRelative.setFlat(False)
        self.btnRelative.setObjectName("btnRelative")
        self.horizontalLayout.addWidget(self.btnRelative)
        self.btnPolar = QtWidgets.QPushButton(Form)
        self.btnPolar.setEnabled(False)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.btnPolar.sizePolicy().hasHeightForWidth())
        self.btnPolar.setSizePolicy(sizePolicy)
        self.btnPolar.setText("")
        icon1 = QtGui.QIcon()
        icon1.addPixmap(QtGui.QPixmap(":/icons/android-radio-button-off.svg"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.btnPolar.setIcon(icon1)
        self.btnPolar.setCheckable(True)
        self.btnPolar.setFlat(False)
        self.btnPolar.setObjectName("btnPolar")
        self.horizontalLayout.addWidget(self.btnPolar)
        self.btnManualAdj = QtWidgets.QPushButton(Form)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.btnManualAdj.sizePolicy().hasHeightForWidth())
        self.btnManualAdj.setSizePolicy(sizePolicy)
        self.btnManualAdj.setText("")
        icon2 = QtGui.QIcon()
        icon2.addPixmap(QtGui.QPixmap(":/icons/edit.svg"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.btnManualAdj.setIcon(icon2)
        self.btnManualAdj.setCheckable(True)
        self.btnManualAdj.setFlat(False)
        self.btnManualAdj.setObjectName("btnManualAdj")
        self.horizontalLayout.addWidget(self.btnManualAdj)
        self.verticalLayout.addLayout(self.horizontalLayout)

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Form"))
        self.nameLabel.setText(_translate("Form", "Left Control Point"))
        self.lineXorA.setToolTip(_translate("Form", "X"))
        self.lineXorA.setPlaceholderText(_translate("Form", "X"))
        self.lineYorM.setToolTip(_translate("Form", "X"))
        self.lineYorM.setPlaceholderText(_translate("Form", "Y"))
import icons_rc
