#ifndef VARIABLEEDITOR_H
#define VARIABLEEDITOR_H

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSettings>

class VariableEditorBase : public QObject
{
    Q_OBJECT
public:
    VariableEditorBase(QObject* parent,
                          const QString& variableName,
                          const QString& settingNameCheck,
                          const QString& settingNameValue,
                          QCheckBox* checkBox);

    virtual void connectInterface();
    virtual void loadSettings(QSettings& settings);
    virtual void saveSettings(QSettings& settings);

    virtual void setValue() = 0;
    virtual void resetDefaultValue() = 0;
signals:
    void settingsUpdated();
    void varEnable(QString name, QVariant value);
    void varDisable(QString name);

public slots:
    void onCheckStateChanged(int);

protected:
    QString _variableName;
    QString _settingNameCheck;
    QString _settingNameValue;

    QCheckBox* _checkBox;

    void updateVar();
};

class DoubleSpinVariableEditor : public VariableEditorBase
{
    Q_OBJECT
public:
    DoubleSpinVariableEditor(QObject* parent,
                     const QString& variableName,
                     const QString& settingNameCheck,
                     const QString& settingNameValue,
                     QCheckBox* checkBox,
                     QDoubleSpinBox* doubleSpinBox,
                     double defaultValue,
                     double minValue = 0.0,
                     double maxValue = 100.0);

    virtual void connectInterface();
    virtual void loadSettings(QSettings& settings);
    virtual void saveSettings(QSettings& settings);
    virtual void setValue();

    virtual void resetDefaultValue();

public slots:
    void onDoubleSpinBoxValueChanged(double);

protected:
    QDoubleSpinBox* _doubleSpinBox;
    double _defaultValue;
    double _minValue;
    double _maxValue;
};

class BoolComboVariableEditor : public VariableEditorBase
{
    Q_OBJECT
public:
    BoolComboVariableEditor(QObject* parent,
                     const QString& variableName,
                     const QString& settingNameCheck,
                     const QString& settingNameValue,
                     QCheckBox* checkBox,
                     QComboBox* comboBox,
                     bool defaultValue = true,
                     QVariant valueTrue = QVariant(1),
                     QVariant valueFalse = QVariant(0));

    virtual void connectInterface();
    virtual void loadSettings(QSettings& settings);
    virtual void saveSettings(QSettings& settings);
    virtual void setValue();

    virtual void resetDefaultValue();

public slots:
    void onComboBoxCurrentIndexChanged(int);

protected:
    QComboBox* _comboBox;
    bool _defaultValue;
    QVariant _valueTrue;
    QVariant _valueFalse;
};

class ValueComboVariableEditor : public VariableEditorBase
{
    Q_OBJECT
public:
    ValueComboVariableEditor(QObject* parent,
                     const QString& variableName,
                     const QString& settingNameCheck,
                     const QString& settingNameValue,
                     QCheckBox* checkBox,
                     QComboBox* comboBox,
                     int defaultValueIndex = 0);

    virtual void connectInterface();
    virtual void loadSettings(QSettings& settings);
    virtual void saveSettings(QSettings& settings);
    virtual void setValue();

    virtual void resetDefaultValue();

public slots:
    void onComboBoxCurrentIndexChanged(int);

protected:
    QComboBox* _comboBox;
    int _defaultValueIndex;
};


#endif // VARIABLEEDITOR_H
