#pragma once

#include <QComboBox>
#include <QListWidget>
#include <QDebug>

class MultiSelectComboBox : public QComboBox
{
  Q_OBJECT

public:
  MultiSelectComboBox(QWidget* aParent = Q_NULLPTR);
  void addItem(const QString& aText, const QVariant& aUserData = QVariant());
  void addItems(const QStringList& aTexts);
  QStringList currentText();
  int count() const;
  void hidePopup() override;
  void SetSearchBarPlaceHolderText(const QString& aPlaceHolderText);
  void SetPlaceHolderText(const QString& aPlaceHolderText);
  void ResetSelection();
  void ResetCheckBoxState(const QString& text);

signals:
  void selectionChanged();

public:
  void clear();
  void setCurrentText(const QString& aText);
  void setCurrentText(const QStringList& aText);

protected:
  void wheelEvent(QWheelEvent* aWheelEvent) override;
  bool eventFilter(QObject* aObject, QEvent* aEvent) override;
  void keyPressEvent(QKeyEvent* aEvent) override;

private slots:
  void stateChanged(int aState);
  void onSearch(const QString& aSearchString);
  void itemClicked(int aIndex);

private:
  QListWidget* mListWidget;
  QLineEdit* mLineEdit;
  QLineEdit* mSearchBar;
};
