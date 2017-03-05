#ifndef SEL_DATA_DLG_QT_h
#define SEL_DATA_DLG_QT_h

#include "sel_data_dlg_ui.h"

#include <ra_plot_structs.h>


class sel_data_dlg_qt : public QDialog, public Ui::sel_data_dlg
{
	Q_OBJECT 
public:
      sel_data_dlg_qt(meas_handle meas, struct ra_plot_options *opt);


 protected:
      meas_handle _meas;
      eval_handle _eval;
      struct ra_plot_options *_opt;
      struct ra_plot_options *_opt_orig;
      
      void _copy_opt_struct(struct ra_plot_options *dest, struct ra_plot_options *src);
      void _copy_pair_struct(struct ra_plot_pair **dest, struct ra_plot_pair *src, int n);
      void _copy_axis_struct(struct ra_plot_axis **dest, struct ra_plot_axis *src, int n);
      void _free_opt_struct(struct ra_plot_options *opt);
      
      
      protected slots:void _init();
      void _do_connect();
      
      void _init_axis();
      void _init_pair_list();
      void format_pair(int idx, QString &entry);
      void get_ch_name(long ch, QString &name);
      
      void _eval_class_select(int index);
      
      void _use_z_axis_click();
      
      void _x_axis_select(int index);
      void _y_axis_select(int index);
      void _z_axis_select(int index);
      
      void _pair_select(int index);
      
      void _add_click();
      void _set_axis(int *idx, QString s, bool allow_same_prop_axis = true);
      
      void _del_click();
      void _opt_click();
      
      void _cancel_click();
      void _ok_click();
};  // class sel_data_dlg_qt


#endif // SEL_DATA_DLG_QT_h
