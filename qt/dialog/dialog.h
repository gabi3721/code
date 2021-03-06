#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QErrorMessage;
QT_END_NAMESPACE

class DialogOptionsWidget;

class Dialog : public QWidget
{
  Q_OBJECT

public:
  Dialog(QWidget *parent = 0);
  ~Dialog();

private slots:
  void setInteger();
  void setDouble();
  void setItem();
  void setText();
  void setMultilineText();

  void setColor();
  void setFont();
  void setOpenFileName();
  void setOpenFileNames();
  void setExistingDirectory();
  void setSaveFileName();
  void criticalMessage();
  void informationMessage();
  void questionMessage();
  void warningMessage();
  void errorMessage();

private:
  QLabel 
    *integerLabel, *doubleLabel, *itemLabel, *textLabel,
    *mlTextLabel, *colorLabel, *fontLabel, *openFileNameLabel,
    *directoryLabel, *openFileNamesLabel, *saveFileNameLabel,
    *criticalLabel, *informationLabel, *questionLabel, *warningLabel, *errorLabel;

  QErrorMessage 
    *errorMessageDlg;

  DialogOptionsWidget 
    *colorDlgOptions, *fontDlgOptions, *fileDlgOptions ;

  QString openFilePath;
};

#endif // DIALOG_H
