#include <ra.h>
#include <ra_pl_comm.h>

#include "annot_comment_model.h"


annot_comment_model::annot_comment_model(meas_handle meas, eval_handle eval, QObject *parent/* = 0*/)
	: QAbstractListModel(parent)
{
	num_ev = 0;

	mh = meas;
	eh = eval;

	init();
} // constructor

annot_comment_model::~annot_comment_model()
{
} // destructor


void
annot_comment_model::init()
{
	num_ev = 0;

	if (!mh || !eh)
		return;

	value_handle vh = ra_value_malloc();
	if ((ra_class_get(eh, "annotation", vh) == -1) || (ra_value_get_num_elem(vh) <= 0))
	{
		ra_value_free(vh);
		return;
	}
	clh = (class_handle)(ra_value_get_voidp_array(vh)[0]);
	if (clh == NULL)
	{
		ra_value_free(vh);
		return;
	}
	prop_handle annot_prop = ra_prop_get(clh, "annotation");
	if (annot_prop == NULL)
	{
		ra_value_free(vh);
		return;
	}
	if (ra_class_get_events(clh, -1, -1, 0, 1, vh) < 0)
	{
		ra_value_free(vh);
		return;
	}
	num_ev = ra_value_get_num_elem(vh);
	const long *p = ra_value_get_long_array(vh);
	ev_ids.resize(num_ev);
	for (long l = 0; l < num_ev; l++)
	{
		ev_ids[l] = p[l];
	}

	start_pos.resize(num_ev);
	end_pos.resize(num_ev);
	comments.resize(num_ev);
	for (long l = 0; l < num_ev; l++)
	{
		long s, e;
		ra_class_get_event_pos(clh, ev_ids[l], &s, &e);
		start_pos[l] = s;
		end_pos[l] = e;

		if (ra_prop_get_value(annot_prop, ev_ids[l], -1, vh) == 0)
			comments[l] = ra_value_get_string(vh);
	}
	ra_value_free(vh);
}  // init()


int
annot_comment_model::rowCount(const QModelIndex &/*idx*/) const
{
	return num_ev;
}  // rowCount()


int
annot_comment_model::columnCount(const QModelIndex &/*parent*/) const
{
	return 1;
}  // columnCount()


QVariant
annot_comment_model::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ((orientation != Qt::Horizontal) || (role != Qt::DisplayRole))
		return QVariant();

	switch (section)
	{
/*	case 0:
		return QVariant("Start-Time");
		break;
	case 1:
		return QVariant("End-Time");
		break;
	case 2:
		return QVariant("Lenght");
		break;
	case 3:*/
	case 0:
		return QVariant("Comment");
		break;
	default:
		return QVariant();
		break;
	}
}  // headerData()


QVariant
annot_comment_model::data(const QModelIndex &idx, int role) const
{
	if (!idx.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	int ev = idx.row();
	if (ev > num_ev)
		return QVariant();

	QString s;
	switch(idx.column())
	{
/*	case 0:
		return QVariant();
		break;
	case 1:
		return QVariant();
		break;
	case 2:
		return QVariant();
		break;
	case 3:*/
	case 0:
		s = comments[ev].c_str();
		return QVariant(s);
		break;
	default:
		return QVariant();
		break;
	}
}  // data()


void
annot_comment_model::send_event_signal(int ev_idx)
{
	if ((ev_idx >= num_ev) || (clh == NULL))
		return;

	struct comm_para *para = new struct comm_para[2];
	para[0].type = PARA_LONG;
	para[0].v.l = start_pos[ev_idx];
	para[1].type = PARA_DOUBLE;
	para[1].v.d = 0.0;
		
	ra_comm_emit(mh, "annot-comment-view", "pos-change", 2, para);

	delete[] para;
}  // send_event_signal()

