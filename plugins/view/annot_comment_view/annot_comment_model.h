#ifndef ANNOT_COMMENT_MODEL
#define ANNOT_COMMENT_MODEL

#include <vector>
#include <string>

using namespace std;

#define _LIBRASCH_BUILD
#include <ra_defines.h>

#include <QtCore/QAbstractListModel>

class annot_comment_model : public QAbstractListModel
{
	Q_OBJECT
public:
	annot_comment_model(meas_handle mh, eval_handle eh, QObject *parent = 0);
	~annot_comment_model();

	void init();

	int rowCount(const QModelIndex &idx) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex &idx, int role) const;

	void send_event_signal(int ev_idx);

protected:
	meas_handle mh;
	eval_handle eh;
	class_handle clh;

	long num_ev;
	vector<long> ev_ids;
	vector<long> start_pos;
	vector<long> end_pos;
	vector<string> comments;
	// positions and length in minutes (as strings)
	vector<string> start_time;
	vector<string> end_time;
	vector<string> length;
};  // class annot_comment_model

#endif  /* ANNOT_COMMENT_MODEL */
