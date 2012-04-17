#include <QString>
#include <QStyledItemDelegate>
#include <QDate>

class datum: public QStyledItemDelegate {
	Q_OBJECT

	public:
	datum(QObject * parent = 0):
		QStyledItemDelegate(parent)
		{
		}

	QString displayText(const QVariant &value, const QLocale &locale) const {
		if (value.type() == QVariant::Date) {
			return value.toDate().toString("dd.MM.yyyy");
		}
		return QStyledItemDelegate::displayText(value, locale);
	}

};
