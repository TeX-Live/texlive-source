# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'GUI/windows/widg_editBezier.ui'
#
# Created by: PyQt5 UI code generator 5.13.1
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.setWindowModality(QtCore.Qt.NonModal)
        Form.resize(692, 35)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(Form.sizePolicy().hasHeightForWidth())
        Form.setSizePolicy(sizePolicy)
        Form.setMinimumSize(QtCore.QSize(0, 35))
        Form.setMaximumSize(QtCore.QSize(16777215, 35))
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout(Form)
        self.horizontalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout_2.setSpacing(0)
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.cmbLockMode = QtWidgets.QComboBox(Form)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.cmbLockMode.sizePolicy().hasHeightForWidth())
        self.cmbLockMode.setSizePolicy(sizePolicy)
        self.cmbLockMode.setObjectName("cmbLockMode")
        self.cmbLockMode.addItem("")
        self.cmbLockMode.addItem("")
        self.cmbLockMode.addItem("")
        self.horizontalLayout.addWidget(self.cmbLockMode)
        spacerItem = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem)
        self.chkRecompute = QtWidgets.QCheckBox(Form)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.MinimumExpanding, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.chkRecompute.sizePolicy().hasHeightForWidth())
        self.chkRecompute.setSizePolicy(sizePolicy)
        self.chkRecompute.setObjectName("chkRecompute")
        self.horizontalLayout.addWidget(self.chkRecompute)
        self.btnForceRecompute = QtWidgets.QPushButton(Form)
        self.btnForceRecompute.setObjectName("btnForceRecompute")
        self.horizontalLayout.addWidget(self.btnForceRecompute)
        self.btnOk = QtWidgets.QPushButton(Form)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.btnOk.sizePolicy().hasHeightForWidth())
        self.btnOk.setSizePolicy(sizePolicy)
        self.btnOk.setMaximumSize(QtCore.QSize(25, 25))
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(":/icons/check.svg"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.btnOk.setIcon(icon)
        self.btnOk.setFlat(True)
        self.btnOk.setObjectName("btnOk")
        self.horizontalLayout.addWidget(self.btnOk)
        self.btnCancel = QtWidgets.QPushButton(Form)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.btnCancel.sizePolicy().hasHeightForWidth())
        self.btnCancel.setSizePolicy(sizePolicy)
        self.btnCancel.setMaximumSize(QtCore.QSize(25, 25))
        icon1 = QtGui.QIcon()
        icon1.addPixmap(QtGui.QPixmap(":/icons/close-round.svg"), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.btnCancel.setIcon(icon1)
        self.btnCancel.setFlat(True)
        self.btnCancel.setObjectName("btnCancel")
        self.horizontalLayout.addWidget(self.btnCancel)
        self.horizontalLayout_2.addLayout(self.horizontalLayout)

        self.retranslateUi(Form)
        self.cmbLockMode.setCurrentIndex(1)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Form"))
        self.cmbLockMode.setItemText(0, _translate("Form", "No Lock"))
        self.cmbLockMode.setItemText(1, _translate("Form", "Lock Angle"))
        self.cmbLockMode.setItemText(2, _translate("Form", "Lock Angle & Scale"))
        self.chkRecompute.setText(_translate("Form", "Recompute Path"))
        self.btnForceRecompute.setText(_translate("Form", "Recompute Once"))
import icons_rc
